#include "VideoProcessor.hpp"

VideoProcessor::VideoProcessor() : pFormatContext(nullptr), pCodecContext(nullptr), pFrame(nullptr), pPacket(nullptr), videoStreamIndex(-1){}

VideoProcessor::~VideoProcessor(){
  if(pPacket){}
  if(pFrame){}
  if(pCodecContext){}
  if(pFormatContext){}
}

bool VideoProcessor::openVideo(const char* filename){
  return true;
}

AVFrame* VideoProcessor::decodeFrame(){
  return nullptr;
}

