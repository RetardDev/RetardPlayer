#ifndef MEDIAPROCESSOR_HPP
#define MEDIAPROCESSOR_HPP

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
};
#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

class MediaProcessor {
private:
  std::shared_ptr<AVFormatContext> pFormatContext;
  std::shared_ptr<AVCodecContext> pCodecContext;
  std::shared_ptr<AVFrame> pFrame;
  std::shared_ptr<AVPacket> pPacket;

  // Audio-related components
  std::shared_ptr<AVFormatContext> aFormatContext;
  std::shared_ptr<AVCodecContext> aCodecContext;
  const AVCodec *aCodec;
  const AVCodec *pCodec;
  AVCodecParameters *pCodecParam;
  std::shared_ptr<SwsContext> sws_ctx;
  std::shared_ptr<AVPacket> aPacket;

  int videoStreamIndex;
  int audioStreamIndex;

  std::mutex frameMutex;

public:
  class Clock {
  public:
    std::atomic<double> current_time{0.0};
    std::mutex clock_mutex;

    void set_time(double time) {
      std::lock_guard<std::mutex> lock(clock_mutex);
      current_time = time;
    }

    double get_time() {
      std::lock_guard<std::mutex> lock(clock_mutex);
      return current_time;
    }
  };
  Clock clock;
  MediaProcessor();
  virtual ~MediaProcessor() = default;

  bool openMediaFile(const std::string &filepath);
  std::shared_ptr<AVFrame> getFrame();
  std::shared_ptr<AVCodecContext> getCodecContext();
  std::shared_ptr<AVCodecContext> getAudioCodecContext() const;
  const AVCodec *getAudioCodec();
  std::shared_ptr<AVFormatContext> getFormatContext() const;
  std::shared_ptr<AVFormatContext> getAudioFormatContext() const;

  int getVideoStreamIndex() const;
  int getAudioStreamIndex() const;

  AVRational getAudioTimeBase() const;
  AVRational getVideoTimeBase() const;
  std::shared_ptr<AVPacket> getAudioPacket();

  std::shared_ptr<AVPacket> getPacket();
  bool convertFrame(std::shared_ptr<AVFrame> srcFrame,
                    std::shared_ptr<AVFrame> dstFrame);

  void lockFrame();
  void unlockFrame();
  std::mutex &getFrameMutex();
};

#endif
