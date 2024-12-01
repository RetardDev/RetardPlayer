#ifndef VIDEOPROCESSOR_HPP
#define VIDEOPROCESSOR_HPP

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
#include "MediaProcessor.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

class VideoProcessor {
public:
  VideoProcessor(std::shared_ptr<MediaProcessor> mediaProcessor);
  ~VideoProcessor();

  void initializeVideo();

  // void setFrameDuration(const std::chrono::milliseconds& duration);
  // std::chrono::steady_clock::time_point getNextFrameTime();
  // double getFrameRate() const;
  /// void updateLastFrameTime();
  // double getFrameDelay();
  void cleanUp();

  bool openVideo();
  bool readNextPacket();
  AVFrame *decodeFrame();

  void synchronise_video(double frame_pts) {
    double current_time = mediaProcessor->clock.get_time();
    if (frame_pts > current_time) {
      double delay = frame_pts - current_time;
      std::this_thread::sleep_for(
          std::chrono::milliseconds(static_cast<int>(delay * 1000)));
    }
  }

private:
  std::shared_ptr<MediaProcessor> mediaProcessor;
  std::shared_ptr<AVPacket>
      videoPacket; // New member variable for audio packets

  // double frameDelay;
  //    std::chrono::steady_clock::time_point lastFrameTime;
  //  std::chrono::milliseconds frameDuration;

  std::unique_ptr<SwsContext, void (*)(SwsContext *)> img_convet_ctx;
};

#endif
