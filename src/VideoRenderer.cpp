#include "VideoRenderer.hpp"  

VideoRenderer::VideoRenderer(SDL_Renderer* renderer): renderer(renderer), texture(nullptr), swsContext(nullptr){}

VideoRenderer::~VideoRenderer(){
  if(texture){SDL_DestroyTexture(texture);}
  if(swsContext){sws_freeContext(swsContext);}
}

void VideoRenderer::prepareScene(){
  SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);

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

   // uint8_t* pixels[3];
    //int pitch[3];

    /*if(SDL_LockTexture(texture, nullptr, (void**)pixels, pitch) < 0){
      std::cerr << "SDL Lock Texture Error: " << SDL_GetError() << std::endl;
      return;
    }*/
      
    SDL_UpdateTexture(texture, NULL, pFrame->data[0], pFrame->linesize[0]);
   // SDL_UnlockTexture(texture);
      SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer); 

}

