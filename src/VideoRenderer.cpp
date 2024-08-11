#include "VideoRenderer.hpp"  

VideoRenderer::VideoRenderer(SDL_Renderer* renderer): renderer(renderer), texture(nullptr){}

VideoRenderer::~VideoRenderer(){
  if(texture){SDL_DestroyTexture(texture);}
}

void VideoRenderer::prepareScene(){
  SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);

  SDL_RenderClear(renderer);
}

void VideoRenderer::presentScene(){
  SDL_RenderPresent(renderer);
}

void VideoRenderer::renderFrame(AVFrame* pFrame){}

