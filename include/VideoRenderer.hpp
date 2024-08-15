#ifndef VIDEORENDERER_HPP
#define VIDEORENDERER_HPP

#include <SDL2/SDL.h>
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
  #include <libavutil/imgutils.h>
}
#include <iostream>
class VideoRenderer{
  public:
    explicit VideoRenderer(SDL_Renderer* renderer);
    virtual ~VideoRenderer();
  
    void prepareScene();
    void presentScene();
    void createTexture(int width, int height);
    void renderFrame(AVFrame* pFrame);

  private:  
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect sdlRect;
    SwsContext* swsContext;


};

#endif
