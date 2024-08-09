#ifndef VIDEORENDERER_HPP
#define VIDEORENDERER_HPP

#include <iostream>
#include <SDL2/SDL.h>
#include "VideoProcessor.hpp"

class VideoRenderer : public VideoProcessor{
  public:
    VideoRenderer();
    virtual ~VideoRenderer();
  
    void initSDL();
    void handleInput();
    void prepareScene();
    void presentScene();
    void renderFrame(AVFrame* pFrame);
    SDL_Renderer* getRenderer();

  private:
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Event event;
};

#endif
