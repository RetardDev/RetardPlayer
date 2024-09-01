#include "MediaProcessor.hpp"

MediaProcessor::MediaProcessor()
  : pFormatContext(nullptr, [](AVFormatContext* ctx) { if (ctx) { avformat_close_input(&ctx); avformat_free_context(ctx); } }),
          pCodecContext(nullptr, [](AVCodecContext* ctx) { if (ctx) { avcodec_free_context(&ctx); } }),
          pFrameYUV(nullptr, [](AVFrame* frame) { if (frame) { av_frame_free(&frame); } }),
          pFrame(nullptr, [](AVFrame* frame) { if (frame) { av_frame_free(&frame); } }),
          pPacket(nullptr, [](AVPacket* pkt) { if (pkt) { av_packet_free(&pkt); } }),
          buffer(nullptr, [](uint8_t* buf) { if (buf) { av_free(buf); } }),
          pCodec(nullptr), pCodecParam(nullptr),
          videoStreamIndex(-1), audioStreamIndex(-1)
    {
      pPacket.reset(av_packet_alloc());
      pFrame.reset(av_frame_alloc());
      pFrameYUV.reset(av_frame_alloc());
    }


std::shared_ptr<AVFrame> MediaProcessor::getFrameYUV() {
    return pFrameYUV;
}

std::shared_ptr<uint8_t> MediaProcessor::getBuffer() {
    return buffer;
}

std::shared_ptr<AVCodecContext> MediaProcessor::getCodecContext() {
    return pCodecContext;
}

std::shared_ptr<AVFormatContext> MediaProcessor::getFormatContext() {
    return pFormatContext;
}

int MediaProcessor::getVideoStreamIndex() {
    return videoStreamIndex;
}

std::shared_ptr<AVPacket> MediaProcessor::getPacket() {
    return pPacket;
}

std::shared_ptr<AVFrame> MediaProcessor::getFrame() {
    return pFrame;
}


bool MediaProcessor::openMediaFile(const std::string& filepath){
    AVFormatContext* rawformatcontext = nullptr;
   if(avformat_open_input(&rawformatcontext, filepath.c_str(), NULL, NULL) != 0){
    std::cerr << "could not open input stream\n"; 
      return false;
   }

   pFormatContext.reset(rawformatcontext, [](AVFormatContext* ctx){ if(ctx){avformat_close_input(&ctx);}});
  
   if(avformat_find_stream_info(pFormatContext.get(), NULL) < 0){
    std::cerr << "could not find stream info";
    return false;
   }
    
   for(unsigned i = 0; i < pFormatContext->nb_streams; i++){
      if(pFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
        videoStreamIndex = i;
      }else if(pFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
        audioStreamIndex = i;
      }
   }

   if(videoStreamIndex == -1){
    std::cerr << "could not find a video stream\n";
    return false;
   }

   if(audioStreamIndex == -1){
    std::cerr << "could not find an audio stream\n";
    return false;
   }

  pCodecParam = pFormatContext->streams[videoStreamIndex]->codecpar;
  pCodec = avcodec_find_decoder(pCodecParam->codec_id);
  if(!pCodec){std::cerr << "codec not found\n"; return -1;}

  pCodecContext.reset(avcodec_alloc_context3(pCodec));
  if(!pCodecContext){std::cerr << "could not allocate video codec context\n"; return -1;}

  if(avcodec_parameters_to_context(pCodecContext.get(), pCodecParam) < 0){
    std::cerr << "failed to copy codec parameters to codec context\n";
    return -1;
  }

  if(avcodec_open2(pCodecContext.get(), pCodec, NULL) < 0){
    std::cerr << "could not open codec\n";
    return -1;
  }

  //pFrameYUV.reset(av_frame_alloc(), [](AVFrame* frame) {if(frame){av_frame_free(&frame);}});
  int numbytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecContext->width, pCodecContext->height, 32);

   buffer.reset(static_cast<uint8_t*>(av_malloc(numbytes * sizeof(uint8_t))), [](uint8_t * buf){if (buf){ av_free(buf);}});
   return true;
  }
