#include "UIElement.hpp"

UIElement::UIElement(int x, int y, int width, int height)
  : x(x), y(y), width(width), height(height) {}

bool UIElement::isMouseOver(int mouseX, int mouseY) const {
  return mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
}
