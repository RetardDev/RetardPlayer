// #pragma once

#ifndef AUDIOPROCESSOR_HPP
#define AUDIOPROCESSOR_HPP

#include <SDL_audio.h>
#include <SDL_stdinc.h>
extern "C" {
#include <SDL2/SDL.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
};

#include "MediaProcessor.hpp"

#include <algorithm>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
class AudioProcessor {
public:
  AudioProcessor(std::shared_ptr<MediaProcessor> mediaProcessor);
  ~AudioProcessor();

  bool openAudioStream();
  void startAudioThread();
  void stopAudioThread();
  void update_audio_clock(double pts) { mediaProcessor->clock.set_time(pts); }
  void pauseAudio();
  void resumeAudio();
  void setVolume(float newVolume);
  SDL_AudioDeviceID getAudioDevice() { return audioDevice; }

private:
  static void audio_callback(void *userdata, Uint8 *stream, int len);
  void handleAudioCallBack(Uint8 *stream, int len);
  void audioPlaybackThread();

  bool readNextAudioPacket();
  uint8_t *decodeAudioFrame();

  static int audio_resampling(AVCodecContext *audio_decode_ctx,
                              AVFrame *decoded_audio_frame,
                              enum AVSampleFormat out_sample_fmt,
                              int out_channels, int out_sample_rate,
                              uint8_t *out_buf);
  std::shared_ptr<MediaProcessor> mediaProcessor;
  SDL_AudioDeviceID audioDevice;
  SDL_AudioSpec audioSpecs;
  SDL_AudioSpec specs;

  std::mutex audioQueueMutex;
  std::queue<uint8_t *> audioQueue;
  std::condition_variable audioQueueCondition;
  std::shared_ptr<AVPacket> audioPacket;
  std::thread playbackThread;
  bool isPlaying;
  bool isRunning;
  float volume;
};
#endif
