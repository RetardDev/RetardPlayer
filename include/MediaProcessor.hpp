#ifndef MEDIAPROCESSOR_HPP
#define MEDIAPROCESSOR_HPP

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};

#include <memory>
#include <iostream>
#include <string>

class MediaProcessor {
private: 
  std::shared_ptr<AVFormatContext> pFormatContext;
  std::shared_ptr<AVCodecContext> pCodecContext;
  std::shared_ptr<AVFrame> pFrameYUV;
  std::shared_ptr<AVFrame> pFrame;
  std::shared_ptr<AVPacket> pPacket;
  std::shared_ptr<uint8_t> buffer;


  const AVCodec* pCodec;
  AVCodecParameters* pCodecParam;

/*  AVPacket* pPacket;
  AVFrame* pFrame;
  AVFrame* pFrameYUV;
  uint8_t* buffer; */
  int videoStreamIndex;
  int audioStreamIndex;

public:
  MediaProcessor();
  virtual ~MediaProcessor() = default;
  bool openMediaFile(const std::string& filepath);
   std::shared_ptr<AVFrame> getFrameYUV();
    std::shared_ptr<uint8_t> getBuffer();
    std::shared_ptr<AVCodecContext> getCodecContext();
    std::shared_ptr<AVFormatContext> getFormatContext();
    int getVideoStreamIndex();
    std::shared_ptr<AVPacket> getPacket();
    std::shared_ptr<AVFrame> getFrame();

};

#endif
