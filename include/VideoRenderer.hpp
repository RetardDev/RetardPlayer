#ifndef VIDEORENDERER_HPP
#define VIDEORENDERER_HPP

#include <iostream>
#include <SDL2/SDL.h>
#include "VideoProcessor.hpp"

class VideoRenderer : public VideoProcessor{
  public:
    VideoRenderer();
    virtual ~VideoRenderer();
  
    void prepareScene();
    void presentScene();
    void renderFrame(AVFrame* pFrame);

  private:
    SDL_Renderer *renderer;
    SDL_Texture *texture;
};

#endif
