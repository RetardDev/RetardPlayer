#ifndef VIDEOPROCESSOR_HPP
#define VIDEOPROCESSOR_HPP

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
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

    //void setFrameDuration(const std::chrono::milliseconds& duration);
    //std::chrono::steady_clock::time_point getNextFrameTime();
    //double getFrameRate() const;
    ///void updateLastFrameTime();
    //double getFrameDelay();
    void cleanUp();

    bool openVideo();
    bool readNextPacket();
    AVFrame* decodeFrame();

  private:
   std::shared_ptr<MediaProcessor> mediaProcessor;
    //double frameDelay;    
//    std::chrono::steady_clock::time_point lastFrameTime;
  //  std::chrono::milliseconds frameDuration;

   std::unique_ptr<SwsContext, void(*)(SwsContext*)> img_convet_ctx;

};

#endif
