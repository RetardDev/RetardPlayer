#include "MediaProcessor.hpp"
#include <libavformat/avformat.h>
#include <libavutil/rational.h>

std::mutex frameMutex;

MediaProcessor::MediaProcessor()
    : pFormatContext(nullptr,
                     [](AVFormatContext *ctx) {
                       if (ctx) {
                         avformat_close_input(&ctx);
                         avformat_free_context(ctx);
                       }
                     }),
      aFormatContext(nullptr,
                     [](AVFormatContext *ctx) {
                       if (ctx) {
                         avformat_close_input(&ctx);
                         avformat_free_context(ctx);
                       }
                     }),
      pCodecContext(nullptr,
                    [](AVCodecContext *ctx) {
                      if (ctx) {
                        avcodec_free_context(&ctx);
                      }
                    }),
      pFrame(nullptr,
             [](AVFrame *frame) {
               if (frame) {
                 av_frame_free(&frame);
               }
             }),
      pPacket(nullptr,
              [](AVPacket *pkt) {
                if (pkt) {
                  av_packet_free(&pkt);
                }
              }),
      aPacket(nullptr,
              [](AVPacket *pkt) {
                if (pkt) {
                  av_packet_free(&pkt);
                }
              }),
      pCodec(nullptr), pCodecParam(nullptr), videoStreamIndex(-1),
      audioStreamIndex(-1) {

  pPacket.reset(av_packet_alloc());
  aPacket.reset(av_packet_alloc());

  pFrame.reset(av_frame_alloc());
}

std::shared_ptr<AVCodecContext> MediaProcessor::getCodecContext() {
  return pCodecContext;
}

std::shared_ptr<AVCodecContext> MediaProcessor::getAudioCodecContext() const {
  return aCodecContext;
}

const AVCodec *MediaProcessor::getAudioCodec() { return aCodec; }

std::shared_ptr<AVFormatContext> MediaProcessor::getFormatContext() const {
  return pFormatContext;
}
std::shared_ptr<AVFormatContext> MediaProcessor::getAudioFormatContext() const {
  return aFormatContext;
}

int MediaProcessor::getVideoStreamIndex() const { return videoStreamIndex; }

int MediaProcessor::getAudioStreamIndex() const { return audioStreamIndex; }

std::shared_ptr<AVPacket> MediaProcessor::getPacket() { return pPacket; }
std::shared_ptr<AVPacket> MediaProcessor::getAudioPacket() { return aPacket; }

std::shared_ptr<AVFrame> MediaProcessor::getFrame() { return pFrame; }

void MediaProcessor::lockFrame() { frameMutex.lock(); }

void MediaProcessor::unlockFrame() { frameMutex.unlock(); }

bool MediaProcessor::openMediaFile(const std::string &filepath) {
  AVFormatContext *rawformatcontext = nullptr;
  if (avformat_open_input(&rawformatcontext, filepath.c_str(), NULL, NULL) !=
      0) {
    std::cerr << "could not open input stream\n";
    return false;
  }

  pFormatContext.reset(rawformatcontext, [](AVFormatContext *ctx) {
    if (ctx) {
      avformat_close_input(&ctx);
    }
  });

  AVFormatContext *rawaudioformatcontext = nullptr;
  if (avformat_open_input(&rawaudioformatcontext, filepath.c_str(), NULL,
                          NULL) != 0) {
    std::cerr << "Could not open input stream\n";
    return false;
  }

  aFormatContext.reset(rawaudioformatcontext, [](AVFormatContext *ctx) {
    if (ctx) {
      avformat_close_input(&ctx);
    }
  });

  if (avformat_find_stream_info(pFormatContext.get(), NULL) < 0) {
    std::cerr << "could not find stream info ";
    return false;
  }

  for (unsigned i = 0; i < pFormatContext->nb_streams; i++) {
    if (pFormatContext->streams[i]->codecpar->codec_type ==
        AVMEDIA_TYPE_VIDEO) {
      videoStreamIndex = i;
    } else if (pFormatContext->streams[i]->codecpar->codec_type ==
               AVMEDIA_TYPE_AUDIO) {
      audioStreamIndex = i;
    }
  }

  // aFormatContext = std::make_shared<AVFormatContext>(*pFormatContext);
  if (videoStreamIndex == -1) {
    std::cerr << "could not find a video stream\n";
    return false;
  }

  if (audioStreamIndex == -1) {
    std::cerr << "could not find an audio stream\n";
    return false;
  }

  pCodecParam = pFormatContext->streams[videoStreamIndex]->codecpar;
  pCodec = avcodec_find_decoder(pCodecParam->codec_id);
  if (!pCodec) {
    std::cerr << "video codec not found\n";
    return false;
  }

  aCodec = avcodec_find_decoder(
      pFormatContext->streams[audioStreamIndex]->codecpar->codec_id);
  if (!aCodec) {
    std::cerr << "audio codec not found\n";
    return false;
  }

  pCodecContext.reset(avcodec_alloc_context3(pCodec));
  if (!pCodecContext) {
    std::cerr << "could not allocate video codec context\n";
    return false;
  }

  if (avcodec_parameters_to_context(pCodecContext.get(), pCodecParam) < 0) {
    std::cerr << "failed to copy codec parameters to codec context\n";
    return false;
  }

  aCodecContext.reset(avcodec_alloc_context3(aCodec));
  if (avcodec_parameters_to_context(
          aCodecContext.get(),
          pFormatContext->streams[audioStreamIndex]->codecpar) < 0) {
    std::cerr
        << "failed to copy audio codec parameters to audio codec context\n";
    return false;
  }

  if (avcodec_open2(pCodecContext.get(), pCodec, NULL) < 0) {
    std::cerr << "could not open codec\n";
    return false;
  }

  sws_ctx.reset(sws_getContext(pCodecContext->width, pCodecContext->height,
                               pCodecContext->pix_fmt, pCodecContext->width,
                               pCodecContext->height, AV_PIX_FMT_YUV420P,
                               SWS_BILINEAR, nullptr, nullptr, nullptr),
                [](SwsContext *ctx) { sws_freeContext(ctx); });

  if (!sws_ctx) {
    std::cerr << "could not initialize sws context\n";
    return false;
  }

  return true;
}

bool MediaProcessor::convertFrame(std::shared_ptr<AVFrame> srcFrame,
                                  std::shared_ptr<AVFrame> dstFrame) {
  std::unique_lock<std::mutex> lock(frameMutex);
  if (!sws_ctx) {
    std::cerr << "Sws context is not initialized\n";
    return false;
  }

  dstFrame->width = srcFrame->width;
  dstFrame->height = srcFrame->height;
  dstFrame->format = AV_PIX_FMT_YUV420P;

  int numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, srcFrame->width,
                                          srcFrame->height, 1);
  uint8_t *buffer = (uint8_t *)av_malloc(numBytes);
  if (!buffer) {
    std::cerr << "could not allocate buffer for YUV420P frame\n";
    return false;
  }

  av_image_fill_arrays(dstFrame->data, dstFrame->linesize, buffer,
                       AV_PIX_FMT_YUV420P, srcFrame->width, srcFrame->height,
                       1);

  sws_scale(sws_ctx.get(), srcFrame->data, srcFrame->linesize, 0,
            srcFrame->height, dstFrame->data, dstFrame->linesize);

  return true;
}

std::mutex &MediaProcessor::getFrameMutex() { return frameMutex; }

AVRational MediaProcessor::getAudioTimeBase() const {
  int audioStreamIndex = getAudioStreamIndex();
  if (audioStreamIndex >= 0) {
    return getAudioFormatContext().get()->streams[audioStreamIndex]->time_base;
  }
  return {0, 1};
}

AVRational MediaProcessor::getVideoTimeBase() const {
  int videoStreamIndex = getVideoStreamIndex();
  if (videoStreamIndex >= 0) {
    return getFormatContext().get()->streams[videoStreamIndex]->time_base;
  }
  return {0, 1};
}
