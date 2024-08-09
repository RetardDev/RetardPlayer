#ifndef VIDEOPLAYERSCENE_HPP
#define VIDEOPLAYERSCENE_HPP

#include "Scene.hpp"
#include "VideoRenderer.hpp"
#include "UIManager.hpp"
#include <memory>

class VideoPlayerScene : public Scene{ 
  public:
    VideoPlayerScene(const std::string& videoFile);
    ~VideoPlayerScene() = default;

    void handleInput(const SDL_Event& event) override;
    void update() override;
    void render(SDL_Renderer* renderer) override;

  private:
    VideoRenderer videoRenderer;
    UIManager uiManager;
};

#endif
