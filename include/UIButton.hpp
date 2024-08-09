#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP

#include "UIElement.hpp"
#include <functional>
#include <iostream>
class UIButton : public UIElement{
  public:
    UIButton(int x, int y, int width, int height, const std::string& text, std::function<void()> onClick);
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;
  
 private:
    std::string text;
    std::function<void()> onClick;
    bool isHovered;
};

#endif 
