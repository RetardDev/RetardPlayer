#include "VideoRenderer.hpp"  

VideoRenderer::VideoRenderer(SDL_Renderer* renderer): renderer(renderer), texture(nullptr), swsContext(nullptr){
  videoRect = {0, 0, 1280, 680};
}

VideoRenderer::~VideoRenderer(){
  if(texture){SDL_DestroyTexture(texture);}
  if(swsContext){sws_freeContext(swsContext);}
}

void VideoRenderer::prepareScene(){
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
}

void VideoRenderer::presentScene(){
  SDL_RenderPresent(renderer);
}

void VideoRenderer::createTexture(int width, int height){
  if(texture){SDL_DestroyTexture(texture);}

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);
  if(!texture){
    std::cerr << "SDL Create Texture Error : " << SDL_GetError() << std::endl;
  }
}

void VideoRenderer::renderFrame(AVFrame* pFrame){
   if (pFrame->width == 0 || pFrame->height == 0) {
        std::cerr << "Invalid frame dimensions: " << pFrame->width << "x" << pFrame->height << std::endl;
        return;
    }

  if (!texture) {
        createTexture(pFrame->width, pFrame->height);
    }
/*    if(!swsContext){
  swsContext = sws_getContext(pFrame->width, pFrame->height,  AV_PIX_FMT_YUV420P, pFrame->width, pFrame->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);
  if(!swsContext){std::cerr << "failed to initialise the covnersion of swscontext\b"; return;}
  }  

 sws_scale(swsContext,(const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pFrame->height, pFrame->data,  pFrame->linesize);
*/
  SDL_UpdateTexture(texture, NULL, pFrame->data[0], pFrame->linesize[0]);
  SDL_RenderCopy(renderer, texture, nullptr, &videoRect);

 ///` av_freep(&rgbData[0]);
}

