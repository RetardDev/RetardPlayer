#include "VideoPlayerScene.hpp"
#include "MediaProcessor.hpp"
#include "ToolBar.hpp"
#include "VideoProcessor.hpp"
#include "VideoRenderer.hpp"
#include <SDL_audio.h>
#include <SDL_events.h>
#include <SDL_mixer.h>
#include <algorithm>
#include <chrono>
#include <libavutil/avutil.h>
#include <libavutil/rational.h>
#include <thread>
VideoPlayerScene::VideoPlayerScene(const std::string &videoFile,
                                   SDL_Renderer *renderer, SDL_Window *window)
    : window(window), videoRenderer(renderer),
      mediaProcessor(std::make_shared<MediaProcessor>()),
      videoProcessor(std::make_shared<VideoProcessor>(mediaProcessor)),
      audioProcessor(std::make_shared<AudioProcessor>(mediaProcessor)),
      isPlaying(true), isDecoding(false), renderer(renderer), toolbar(nullptr) {
  currentFrame = av_frame_alloc();
  if (!mediaProcessor->openMediaFile(videoFile)) {
    std::cerr << "Media Processor could not open the file";
    return;
  }
  windowWidth = mediaProcessor->getCodecContext()->width;
  windowHeight = mediaProcessor->getCodecContext()->height;
  renderWidth = windowWidth;
  renderHeight = windowHeight;
  SDL_SetWindowSize(window, mediaProcessor->getCodecContext()->width,
                    mediaProcessor->getCodecContext()->height);

  if (videoProcessor->openVideo()) {
    // stopDecoding();
    startDecoding();
  } else {
    std::cerr << "Failed top open video file: " << videoFile << std::endl;
  }

  if (audioProcessor->openAudioStream()) {
    audioProcessor->startAudioThread();
  } else {
    std::cerr << "Failed to open audio stream\n";
    return;
  }

  controlBarRect = {0, 720 - 50, 1280, 50};
  toolbar = new Toolbar(renderer, windowWidth, windowHeight);

  toolbar->setPlayPauseCallback([this]() {
    isPlaying = !isPlaying;
    if (isPlaying) {
      SDL_PauseAudioDevice(audioProcessor->getAudioDevice(), 0);
    } else {
      SDL_PauseAudioDevice(audioProcessor->getAudioDevice(), 1);
    }
  });

  toolbar->setStopCallback([this]() {
    isPlaying = false;
    SDL_PauseAudioDevice(audioProcessor->getAudioDevice(), 1);
  });

  toolbar->setVolumeChangeCallback(
      [this](float volume) { audioProcessor->setVolume(volume); });

  toolbar->setSeekCallback([this](float progress) {
    AVRational timeBase = mediaProcessor->getVideoTimeBase();
    double currentTimeInSeconds = currentTime;
    int64_t duration = mediaProcessor->getFormatContext()->duration;
    double targetTimestamp = currentTimeInSeconds + progress * AV_TIME_BASE;
    if (targetTimestamp > duration)
      targetTimestamp = duration;

    std::cout << "Target timestamp " << targetTimestamp << std::endl;

    if (av_seek_frame(mediaProcessor->getFormatContext().get(),
                      mediaProcessor->getVideoStreamIndex(), targetTimestamp,
                      AVSEEK_FLAG_ANY) < 0) {
      std::cerr << "Error while seeking forward\n";
      return -1;
    }

    auto codecContext = mediaProcessor->getCodecContext().get();
    if (codecContext) {
      avcodec_flush_buffers(codecContext);
      for (; !frameQueue.empty();)
        frameQueue.pop();
    } else {
      std::cerr << "Error: Codec context is null\n";
      return -1;
    }

    return 1;
  });
  bool running = true;
}

void VideoPlayerScene::handleInput(const SDL_Event &event) {
  uiManager.handleEvent(event);
  toolbar->handleEvent(event);

  if (event.type == SDL_WINDOWEVENT) {
    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
      int newWidth = event.window.data1;
      int newHeight = event.window.data2;
      resizeRenderer(newWidth, newHeight);
    }
  }
}

void VideoPlayerScene::startDecoding() {
  std::cout << "Start Decoding Executed\n";
  isDecoding = true;
  decodeThread = std::thread(&VideoPlayerScene::decodeLoop, this);
}

void VideoPlayerScene::decodeLoop() {
  std::cout << "Decode Loop Executed\n";
  double frame_duration = 0.0;

  double avg_frame_duration =

      av_q2d(mediaProcessor->getCodecContext().get()->framerate);

  std::chrono::steady_clock::time_point last_frame_time =
      std::chrono::steady_clock::now();

  while (isDecoding) {
    if (isPlaying && videoProcessor->readNextPacket()) {
      AVFrame *videoFrame = videoProcessor->decodeFrame();

      if (videoFrame) {
        std::lock_guard<std::mutex> lock(queueMutex);
        frameQueue.push(videoFrame);
        queueCondition.notify_one();

        if (videoFrame->pts != AV_NOPTS_VALUE) {
          frame_duration =
              videoFrame->pts *
              av_q2d(mediaProcessor->getCodecContext().get()->time_base);
          std::cout << "Frame duration: " << frame_duration << std::endl;

        } else {
          frame_duration = avg_frame_duration;
        }

        std::chrono::steady_clock::time_point current_time =
            std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_time =
            current_time - last_frame_time;

        double wait_time = frame_duration - elapsed_time.count();
        if (wait_time > 0) {
          std::this_thread::sleep_for(
              std::chrono::milliseconds(static_cast<int>(wait_time * 1000)));
        }
        last_frame_time = std::chrono::steady_clock::now();
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void VideoPlayerScene::stopDecodingThread() {
  isDecoding = false;
  if (decodeThread.joinable()) {
    decodeThread.join();
  }

  std::lock_guard<std::mutex> lock(queueMutex);
  while (!frameQueue.empty()) {
    AVFrame *frame = frameQueue.front();
    av_frame_free(&frame);
    frameQueue.pop();
  }
}

void VideoPlayerScene::update() {}

void VideoPlayerScene::render(SDL_Renderer *renderer) {
  videoRenderer.prepareScene();

  {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (!frameQueue.empty()) {
      currentFrame = frameQueue.front();

      if (currentFrame->pts != AV_NOPTS_VALUE) {
        double framePts =
            currentFrame->pts * av_q2d(mediaProcessor->getVideoTimeBase());
        double clockTime = mediaProcessor->clock.get_time();

        double waitTime = framePts - clockTime;
        if (waitTime > 0) {
          std::this_thread::sleep_for(
              std::chrono::milliseconds(static_cast<int>(waitTime * 1000)));
        } else if (waitTime < -0.1) {
          std::cerr << "Dropping late frame: PTS = " << framePts
                    << ", Clock = " << clockTime << "\n";
          av_frame_free(&currentFrame);
          frameQueue.pop();
          return;
        }
      }

      frameQueue.pop();
    }
  }

  if (currentFrame) {
    SDL_Rect renderRect = {renderX, renderY, renderWidth, renderHeight};
    videoRenderer.renderFrame(currentFrame, &renderRect);
    currentTime = currentFrame->best_effort_timestamp *
                  av_q2d(mediaProcessor->getFormatContext()
                             ->streams[mediaProcessor->getVideoStreamIndex()]
                             ->time_base);

    double progress =
        currentTime /
        (mediaProcessor->getFormatContext()->duration / (double)AV_TIME_BASE);
    progress = std::clamp(progress, 0.0, 1.0);
    std::cout << "Progress : " << progress << std::endl;
    toolbar->setProgress(progress);
  }

  toolbar->render(renderer);
  SDL_RenderPresent(renderer);
}

void VideoPlayerScene::resizeRenderer(int newWidth, int newHeight) {
  SDL_RenderSetViewport(renderer, nullptr);
  windowWidth = newWidth;
  windowHeight = newHeight;

  double videoAspect =
      static_cast<double>(mediaProcessor->getCodecContext()->width) /
      mediaProcessor->getCodecContext()->height;

  if (static_cast<double>(newWidth) / newHeight > videoAspect) {
    renderHeight = newHeight;
    renderWidth = static_cast<int>(newHeight * videoAspect);
  } else {
    renderWidth = newWidth;
    renderHeight = static_cast<int>(newWidth / videoAspect);
  }
  toolbar->updateDimensions(newWidth, newHeight);
  renderX = (newWidth - renderWidth) / 2;
  renderY = (newHeight - renderHeight) / 2;
}
