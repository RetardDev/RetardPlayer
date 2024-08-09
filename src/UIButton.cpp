#include "UIButton.hpp"
#include <SDL2/SDL_ttf.h>

UIButton::UIButton(int x, int y, int width, int height, const std::string& text, std::function<void()> onClick)
    : UIElement(x, y, width, height), text(text), onClick(onClick) {}

void UIButton::render(SDL_Renderer* renderer){
  SDL_SetRenderDrawColor(renderer, isHovered ? 150 : 100, isHovered ? 150 : 100, isHovered ? 255 : 255, 255);
  SDL_Rect rect = {x, y, width, height};
  SDL_RenderFillRect(renderer, &rect);

  if(TTF_WasInit() == 0){ TTF_Init(); }

  TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf", 24);
  if(font){
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_Rect textRect = {x + (width - textWidth) / 2, y + (height - textHeight) / 2, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
  }
}

void UIButton::handleEvent(const SDL_Event& event){
  if(event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN){
    int mouseX = event.motion.x;
    int mouseY = event.motion.y;
    isHovered = isMouseOver(mouseX, mouseY);

    if(isHovered && event.type  == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT){
      onClick();
    }
  }
}

