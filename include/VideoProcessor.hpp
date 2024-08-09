#ifndef VIDEOPROCESSOR_HPP
#define VIDEOPROCESSOR_HPP

extern "C" {
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
}

class VideoProcessor {
  public:
    VideoProcessor();
    virtual ~VideoProcessor();

    bool openVideo(const char* filename);
    AVFrame* decodeFrame();

  protected:
    AVFormatContext* pFormatContext;
    AVCodecContext* pCodecContext;
    AVFrame* pFrame;
    AVPacket* pPacket;
    int videoStreamIndex;
};

#endif
