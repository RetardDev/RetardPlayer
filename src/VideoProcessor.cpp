#include "VideoProcessor.hpp"

VideoProcessor::VideoProcessor(std::shared_ptr<MediaProcessor> mediaProcessor) :  mediaProcessor(mediaProcessor), img_convet_ctx(nullptr, sws_freeContext){}

VideoProcessor::~VideoProcessor(){
   
}

void VideoProcessor::cleanUp(){}

bool VideoProcessor::openVideo(){
    auto frameYUV = mediaProcessor->getFrameYUV().get();
    auto buffer = mediaProcessor->getBuffer();
    auto codecContext = mediaProcessor->getCodecContext();

  if (frameYUV && buffer && codecContext) {
      av_image_fill_arrays(
            frameYUV->data,
            frameYUV->linesize,
            buffer.get(),
            AV_PIX_FMT_YUV420P,  // Match the format of the allocated frame
            codecContext->width,
            codecContext->height,
            32
        );
  } else {
      std::cerr << "Error: Invalid media processor components." << std::endl;
      return false;
  }

    img_convet_ctx.reset(sws_getContext(mediaProcessor->getCodecContext()->width, mediaProcessor->getCodecContext()->height, mediaProcessor->getCodecContext()->pix_fmt,
                                      mediaProcessor->getCodecContext()->width, mediaProcessor->getCodecContext()->height, AV_PIX_FMT_RGB24, SWS_LANCZOS, NULL, NULL, NULL));
      
      if (!img_convet_ctx) { std::cerr << "Could not initialize sws context\n"; return 0; }

    std::cout << "VideoProcessor OpenVideo Executed\n";
    
    return true;
}


//double VideoProcessor::getFrameDelay(){return frameDelay;}

bool VideoProcessor::readNextPacket(){
  //av_packet_unref(mediaProcessor->getPacket().get());
  int ret = av_read_frame(mediaProcessor->getFormatContext().get(), mediaProcessor->getPacket().get());
  if(ret < 0){
    if(ret == AVERROR_EOF){std::cerr << "End of video reached\n";
    }else{
      std::cerr << "Error reading frame: ";
    }
    return 0;  
  }
  
  if(mediaProcessor->getPacket()->stream_index != mediaProcessor->getVideoStreamIndex()){
   av_packet_unref(mediaProcessor->getPacket().get());
return 0;
  }
  return true;
}

AVFrame* VideoProcessor::decodeFrame(){
    if(!mediaProcessor->getPacket() || mediaProcessor->getPacket()->size <= 0){std::cerr << "No packet to decode\n"; return nullptr;}

    int ret = avcodec_send_packet(mediaProcessor->getCodecContext().get(), mediaProcessor->getPacket().get());
    if(ret < 0){
        std::cerr << "Error sending packet to decoder\n";
        return nullptr;
    }

    ret = avcodec_receive_frame(mediaProcessor->getCodecContext().get(), mediaProcessor->getFrame().get());
    if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
        return nullptr;
    }else if (ret < 0){
        std::cerr << "Error receiving frame from decoder\n";
        return nullptr;
    }

    if(mediaProcessor->getFrame()->width == 0 || mediaProcessor->getFrame()->height == 0){
        std::cerr << "Invalid source frame dimensions: " << mediaProcessor->getFrame()->width << "x" << mediaProcessor->getFrame()->height << std::endl;
        return nullptr;
    } 

    mediaProcessor->getFrameYUV()->width = mediaProcessor->getFrame()->width;
    mediaProcessor->getFrameYUV()->height = mediaProcessor->getFrame()->height;
    mediaProcessor->getFrameYUV()->format = AV_PIX_FMT_YUV420P; 

    return mediaProcessor->getFrameYUV().get();
}
