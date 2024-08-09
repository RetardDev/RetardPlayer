#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include "UIButton.hpp"
#include <vector>
#include <memory>

class UIManager{
  public:
    UIManager();
    ~UIManager();

    void addElement(std::shared_ptr<UIElement> element);
    void render(SDL_Renderer* renderer);
    void handleEvent(const SDL_Event& event);

  private:
    std::vector<std::shared_ptr<UIElement>> elements;
};

#endif
