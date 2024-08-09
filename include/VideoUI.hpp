#ifndef VIDEOUI_HPP
#define VIDEOUI_HPP

#include "VideoRenderer.hpp"
#include "UIManager.hpp"

class VideoUI {
  public:
    VideoUI(VideoRenderer* renderer);
    void run();

  private:
    VideoRenderer* videoRenderer;
    UIManager uiManager;
};

#endif
