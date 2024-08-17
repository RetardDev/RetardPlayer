#ifndef VIDEOUI_HPP
#define VIDEOUI_HPP

#include "VideoRenderer.hpp"
#include "UIManager.hpp"
#include <memory>
class VideoUI {
  public:
    explicit VideoUI(SDL_Renderer* renderer, UIManager* uiManager);
    void handleInput(const SDL_Event& event);
    void render();
    void createUIElements();

std::shared_ptr<UIButton> playPauseButton;
  private:
    VideoRenderer* videoRenderer;
    UIManager* uiManager;
    SDL_Renderer* renderer;
    SDL_Texture* defaultTexture;
    SDL_Texture* clickedTexture;
    SDL_Texture* currentTexture;
    SDL_Rect buttonRect;
    bool isClicked;
    
    
    SDL_Texture* loadTexture(const std::string& path);
};

#endif
