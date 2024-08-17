#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP

#include "UIElement.hpp"
#include <functional>
#include <iostream>
class UIButton : public UIElement{
  public:
    UIButton(int x, int y, int w, int h, const std::string& text, const std::function<void()>& onClick,
             SDL_Renderer* renderer, const std::string& iconPath = "", const std::string& clickedIconPath = "");

    ~UIButton();

    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;
  
 private:
    std::string text;
    std::function<void()> onClick;
    bool isHovered;
    SDL_Texture* iconTexture;
    SDL_Texture* clickedIconTexture;
    SDL_Texture* currentTexture;

    bool loadIconTexture(SDL_Renderer* renderer, const std::string& path, SDL_Texture** texture);
    void toggleIcon();
};

#endif 
