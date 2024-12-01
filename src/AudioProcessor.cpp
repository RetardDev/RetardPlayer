
#include "AudioProcessor.hpp"
#include <SDL2/SDL_mixer.h>
#include <SDL_audio.h>
#include <cstdint>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
AudioProcessor::AudioProcessor(std::shared_ptr<MediaProcessor> mediaProcessor)
    : mediaProcessor(mediaProcessor), isPlaying(true), isRunning(false),
      volume(1.0f) {}

AudioProcessor::~AudioProcessor() {
  stopAudioThread();
  if (audioDevice > 0) {
    SDL_CloseAudioDevice(audioDevice);
  }
}

bool AudioProcessor::openAudioStream() {
  auto aCodecCtx = mediaProcessor->getAudioCodecContext().get();
  auto aCodec = mediaProcessor->getAudioCodec();

  if (avcodec_open2(aCodecCtx, aCodec, NULL) < 0) {
    std::cerr << "Could not open audio codec\n";
    return false;
  }

  audioSpecs.freq = aCodecCtx->sample_rate;
  audioSpecs.format = AUDIO_S16SYS;
  audioSpecs.channels = aCodecCtx->ch_layout.nb_channels;
  audioSpecs.silence = 0;
  audioSpecs.samples = 1024;
  audioSpecs.callback = AudioProcessor::audio_callback;
  audioSpecs.userdata = this;

  audioDevice = SDL_OpenAudioDevice(NULL, 0, &audioSpecs, &specs,
                                    SDL_AUDIO_ALLOW_FORMAT_CHANGE);

  if (audioDevice == 0) {
    std::cerr << "Failed to open audio device: " << SDL_GetError() << "\n";
    return false;
  }

  SDL_PauseAudioDevice(audioDevice, 0);
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "SDL_Mixer could not initialize! SDL_Mixer Error: "
              << Mix_GetError() << std::endl;
  }

  return true;
}

void AudioProcessor::startAudioThread() {
  isRunning = true;
  playbackThread = std::thread(&AudioProcessor::audioPlaybackThread, this);
}

void AudioProcessor::stopAudioThread() {
  isRunning = false;
  if (playbackThread.joinable()) {
    playbackThread.join();
  }
}

void AudioProcessor::audioPlaybackThread() {
  while (isRunning) {
    if (readNextAudioPacket()) {
      uint8_t *audioBuffer = decodeAudioFrame();
      if (audioBuffer) {
        std::lock_guard<std::mutex> lock(audioQueueMutex);
        audioQueue.push(audioBuffer);
        audioQueueCondition.notify_one();
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void AudioProcessor::audio_callback(void *userdata, Uint8 *stream, int len) {
  AudioProcessor *processor = static_cast<AudioProcessor *>(userdata);

  processor->handleAudioCallBack(stream, len);
}

void AudioProcessor::handleAudioCallBack(Uint8 *stream, int len) {
  double audioPts = mediaProcessor->clock.get_time();
  double playback_time =
      (double)len / (audioSpecs.freq * audioSpecs.channels *
                     SDL_AUDIO_BITSIZE(audioSpecs.format) / 8);
  mediaProcessor->clock.set_time(audioPts + playback_time);
  std::unique_lock<std::mutex> lock(audioQueueMutex);

  if (!audioQueueCondition.wait_for(lock, std::chrono::milliseconds(100),
                                    [&]() { return !audioQueue.empty(); })) {
    SDL_memset(stream, 0, len);
    return;
  }

  if (audioQueue.empty()) {
    return;
  }

  if (!isRunning) {
    return;
  }

  uint8_t *audio_buf = audioQueue.front();
  audioQueue.pop();

  if (audio_buf) {
    int buf_size =
        av_samples_get_buffer_size(nullptr, 2, 1024, AV_SAMPLE_FMT_S16, 1);
    int len1 = std::min(len, buf_size);

    int16_t *samples = reinterpret_cast<int16_t *>(audio_buf);
    int sampleCount = len1 / sizeof(int16_t);

    for (int i = 0; i < sampleCount; ++i) {
      samples[i] = static_cast<int16_t>(samples[i] * volume);
    }

    memcpy(stream, audio_buf, len1);
    av_free(audio_buf);
  }
}

bool AudioProcessor::readNextAudioPacket() {
  int ret = av_read_frame(mediaProcessor->getAudioFormatContext().get(),
                          mediaProcessor->getAudioPacket().get());

  if (ret < 0) {
    if (ret == AVERROR_EOF) {
      std::cerr << "End of audio stream\n";
    } else {
      std::cerr << "Error reading audio packet: " << ret << "\n";
    }
    return false;
  }

  if (mediaProcessor->getAudioPacket()->stream_index !=
      mediaProcessor->getAudioStreamIndex()) {
    av_packet_unref(mediaProcessor->getAudioPacket().get());
    return false;
  }

  return true;
}

uint8_t *AudioProcessor::decodeAudioFrame() {
  AVFrame *audioFrame = av_frame_alloc();
  if (!audioFrame) {
    std::cerr << "Failed to allocate audio frame\n";
    return nullptr;
  }

  AVPacket *packet = mediaProcessor->getAudioPacket().get();

  int ret =
      avcodec_send_packet(mediaProcessor->getAudioCodecContext().get(), packet);
  if (ret < 0) {
    std::cerr << "Error sending packet to audio decoder: " << av_err2str(ret)
              << "\n";
    av_packet_unref(packet);
    av_frame_free(&audioFrame);
    return nullptr;
  }

  if (audioFrame->pts != AV_NOPTS_VALUE) {
    double audioPts =
        audioFrame->pts * av_q2d(mediaProcessor->getAudioTimeBase());
    if (mediaProcessor->clock.get_time() == 0) {
      mediaProcessor->clock.set_time(audioPts);
    }
    mediaProcessor->clock.set_time(audioPts);

    std::cout << "Audio PTS: " << audioPts << " seconds" << std::endl;
  }

  ret = avcodec_receive_frame(mediaProcessor->getAudioCodecContext().get(),
                              audioFrame);
  if (ret < 0) {
    if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
      std::cerr << "Error receiving audio frame from decoder: "
                << av_err2str(ret) << "\n";
    }
    av_frame_free(&audioFrame);
    return nullptr;
  }

  uint8_t *audioBuffer = nullptr;
  int data_size = audio_resampling(
      mediaProcessor->getAudioCodecContext().get(), audioFrame,
      AV_SAMPLE_FMT_S16,
      mediaProcessor->getAudioCodecContext().get()->ch_layout.nb_channels,
      mediaProcessor->getAudioCodecContext().get()->sample_rate, nullptr);

  if (data_size < 0) {
    std::cerr << "Error calculating audio buffer size\n";
    av_frame_free(&audioFrame);
    return nullptr;
  }

  audioBuffer = static_cast<uint8_t *>(av_malloc(data_size));
  if (!audioBuffer) {
    std::cerr << "Failed to allocate audio buffer\n";
    av_frame_free(&audioFrame);
    return nullptr;
  }

  data_size = audio_resampling(
      mediaProcessor->getAudioCodecContext().get(), audioFrame,
      AV_SAMPLE_FMT_S16,
      mediaProcessor->getAudioCodecContext().get()->ch_layout.nb_channels,
      mediaProcessor->getAudioCodecContext().get()->sample_rate, audioBuffer);

  if (data_size < 0) {
    std::cerr << "Error resampling audio data\n";
    av_free(audioBuffer);
    av_frame_free(&audioFrame);
    return nullptr;
  }

  av_packet_unref(packet);
  av_frame_free(&audioFrame);

  return audioBuffer;
}

int AudioProcessor::audio_resampling(AVCodecContext *audio_decode_ctx,
                                     AVFrame *decoded_audio_frame,
                                     enum AVSampleFormat out_sample_fmt,
                                     int out_channels, int out_sample_rate,
                                     uint8_t *out_buf) {
  if (!audio_decode_ctx || !decoded_audio_frame) {
    printf("Invalid input to audio_resampling.\n");
    return -1;
  }

  SwrContext *swr_ctx = nullptr;
  int ret = 0;
  AVChannelLayout in_channel_layout, out_channel_layout;

  av_channel_layout_default(&in_channel_layout,
                            audio_decode_ctx->ch_layout.nb_channels);
  av_channel_layout_default(&out_channel_layout, out_channels);

  int in_nb_samples = decoded_audio_frame->nb_samples;
  if (in_nb_samples <= 0) {
    printf("Invalid number of input samples.\n");
    return -1;
  }

  int max_out_nb_samples =
      av_rescale_rnd(in_nb_samples, out_sample_rate,
                     audio_decode_ctx->sample_rate, AV_ROUND_UP);

  uint8_t **resampled_data = nullptr;
  ret = av_samples_alloc_array_and_samples(
      &resampled_data, nullptr, out_channel_layout.nb_channels,
      max_out_nb_samples, out_sample_fmt, 0);
  if (ret < 0) {
    printf("Error allocating resampled data.\n");
    return -1;
  }

  swr_ctx = swr_alloc();
  ret = swr_alloc_set_opts2(&swr_ctx, &out_channel_layout, out_sample_fmt,
                            out_sample_rate, &in_channel_layout,
                            audio_decode_ctx->sample_fmt,
                            audio_decode_ctx->sample_rate, 0, nullptr);
  if (ret < 0 || swr_init(swr_ctx) < 0) {
    printf("Error initializing SwrContext.\n");
    av_freep(&resampled_data[0]);
    av_freep(&resampled_data);
    return -1;
  }

  ret = swr_convert(swr_ctx, resampled_data, max_out_nb_samples,
                    (const uint8_t **)decoded_audio_frame->data, in_nb_samples);
  if (ret < 0) {
    printf("Error in swr_convert.\n");
    av_freep(&resampled_data[0]);
    av_freep(&resampled_data);
    swr_free(&swr_ctx);
    return -1;
  }

  int resampled_data_size = av_samples_get_buffer_size(
      nullptr, out_channel_layout.nb_channels, ret, out_sample_fmt, 1);
  if (resampled_data_size < 0) {
    printf("Error calculating buffer size.\n");
    av_freep(&resampled_data[0]);
    av_freep(&resampled_data);
    swr_free(&swr_ctx);
    return -1;
  }

  if (!out_buf) {
    out_buf = (uint8_t *)av_malloc(resampled_data_size);
    if (!out_buf) {
      printf("Error allocating output buffer.\n");
      av_freep(&resampled_data[0]);
      av_freep(&resampled_data);
      swr_free(&swr_ctx);
      return -1;
    }
  }

  if (resampled_data[0]) {
    memcpy(out_buf, resampled_data[0], resampled_data_size);
  } else {
    printf("Resampled data is null.\n");
    resampled_data_size = -1;
  }

  av_freep(&resampled_data[0]);
  av_freep(&resampled_data);
  swr_free(&swr_ctx);

  return resampled_data_size;
}

void AudioProcessor::pauseAudio() { SDL_PauseAudioDevice(audioDevice, 1); }

void AudioProcessor::resumeAudio() {
  SDL_PauseAudioDevice(audioDevice, 0);
  isPlaying = true;
}
void AudioProcessor::setVolume(float newVolume) {
  volume = std::max(0.0f, std::min(1.0f, newVolume));
}
