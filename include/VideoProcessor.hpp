#ifndef VIDEOPROCESSOR_HPP
#define VIDEOPROCESSOR_HPP

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

}
#include <iostream>
#include <chrono>

class VideoProcessor {
  public:
    VideoProcessor();
    virtual ~VideoProcessor();

    void setFrameDuration(const std::chrono::milliseconds& duration);
    std::chrono::steady_clock::time_point getNextFrameTime();
    void updateLastFrameTime();
    bool openVideo(const char* filename);
    bool readNextPacket();
    AVFrame* decodeFrame();

  protected:
    AVFormatContext* pFormatContext;
    AVCodecContext* pCodecContext;
    const AVCodec *pCodec;
    AVFrame* pFrame;
    AVFrame* pFrameYUV;
    uint8_t* buffer;
    AVPacket* pPacket;
    int videoStreamIndex;
    
    std::chrono::steady_clock::time_point lastFrameTime;
    std::chrono::milliseconds frameDuration;


    struct SwsContext *img_convet_ctx;

};

#endif
