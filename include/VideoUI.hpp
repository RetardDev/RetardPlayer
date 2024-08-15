#ifndef VIDEOUI_HPP
#define VIDEOUI_HPP

#include "VideoRenderer.hpp"
#include "UIManager.hpp"

class VideoUI {
  public:
    explicit VideoUI(SDL_Renderer* renderer, UIManager uiManager);
    void run();

  private:
    VideoRenderer* videoRenderer;
    UIManager uiManager;
    SDL_Renderer* renderer;
};

#endif
