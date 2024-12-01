#ifndef VIDEORENDERER_HPP
#define VIDEORENDERER_HPP

#include <SDL2/SDL.h>
#include <memory>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <iostream>
#include <mutex>
class VideoRenderer {
public:
  explicit VideoRenderer(SDL_Renderer *renderer);
  virtual ~VideoRenderer();

  void prepareScene();
  void presentScene();
  void createTexture(int width, int height);
  void renderFrame(AVFrame *pFrame, SDL_Rect *targetRect);

private:
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  SDL_Rect videoRect;
  SwsContext *swsContext;
  AVFrame *rgbFrame;
  std::shared_ptr<uint8_t> rgbBuffer;
  std::mutex frameMutex;
};

#endif
