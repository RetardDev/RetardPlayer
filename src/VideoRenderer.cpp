#include "VideoRenderer.hpp"  

VideoRenderer::VideoRenderer(): window(nullptr), renderer(nullptr), texture(nullptr), SCREEN_WIDTH(1280), SCREEN_HEIGHT(720){
  initSDL();
}

VideoRenderer::~VideoRenderer(){
  if(texture){}

  if(renderer){
    SDL_DestroyRenderer(renderer);
  }
  if(window){
    SDL_DestroyWindow(window);
  }

  SDL_Quit();
}

void VideoRenderer::initSDL(){

  if(SDL_Init(SDL_INIT_VIDEO)){
    std::cerr << "Could not initialise SDL" << SDL_GetError();
    return;
  }
  
  window = SDL_CreateWindow("Retard Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if(!window){
    std::cerr << "Could not create window " << SDL_GetError();
    return;
  }  

  renderer = SDL_CreateRenderer(window, -1, 0);

  if(!renderer){
    std::cerr << "Could not create renderer " << SDL_GetError();
    return;
  }

}

void VideoRenderer::handleInput(){  
  while(SDL_PollEvent(&event)){
    switch(event.type){
      case SDL_QUIT:
        exit(0);
        break;

      default:
        break;
    }
  }
}

void VideoRenderer::prepareScene(){
  SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);

  SDL_RenderClear(renderer);
}

void VideoRenderer::presentScene(){
  SDL_RenderPresent(renderer);
}

void VideoRenderer::renderFrame(AVFrame* pFrame){}

SDL_Renderer* VideoRenderer::getRenderer(){

  return renderer;
}
