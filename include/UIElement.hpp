#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP

#include <SDL2/SDL.h>
class UIElement {
  public:
    UIElement(int x, int y, int width, int height);
    virtual ~UIElement() = default;

    virtual void render(SDL_Renderer* renderer) = 0;
    virtual void handleEvent(const SDL_Event& event) = 0;
    bool isMouseOver(int mouseX, int mouseY) const;

  protected:
      int x, y, width, height;
};

#endif
