#ifndef VIDEOPLAYERSCENE_HPP
#define VIDEOPLAYERSCENE_HPP

#include "Scene.hpp"
#include "VideoProcessor.hpp"
#include "VideoRenderer.hpp"
#include "UIManager.hpp"
#include "VideoUI.hpp"
class VideoPlayerScene : public Scene{ 
  public:
    VideoPlayerScene(const std::string& videoFile, SDL_Renderer* renderer);
    ~VideoPlayerScene() override = default;

    void handleInput(const SDL_Event& event) override;
    void update() override;
    void render(SDL_Renderer* renderer) override;

  private:
    VideoRenderer videoRenderer;
    VideoProcessor videoProcessor;
    UIManager uiManager;
    VideoUI* videoUI;
};

#endif
