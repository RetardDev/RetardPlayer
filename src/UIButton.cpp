#include "UIButton.hpp"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
UIButton::UIButton(int x, int y, int w, int h, const std::string& text, const std::function<void()>& onClick,
                   SDL_Renderer* renderer, const std::string& iconPath, const std::string& clickedIconPath)
    : UIElement(x, y, w, h), text(text), onClick(onClick), iconTexture(nullptr), clickedIconTexture(nullptr), currentTexture(nullptr){

      if(!iconPath.empty()){
        loadIconTexture(renderer, iconPath, &iconTexture);
        currentTexture = iconTexture;
      }

      if(!clickedIconPath.empty()){
        loadIconTexture(renderer, clickedIconPath, &clickedIconTexture);
      }
}

UIButton::~UIButton(){
  if(iconTexture){SDL_DestroyTexture(iconTexture);}
  if(clickedIconTexture){SDL_DestroyTexture(clickedIconTexture);}
}

bool UIButton::loadIconTexture(SDL_Renderer* renderer, const std::string& path, SDL_Texture** texture){
  SDL_Surface* surface = IMG_Load(path.c_str());
  if(!surface){std::cerr << "Failed to load icon: " << IMG_GetError() << std::endl; return false;}
  *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  if(!*texture){std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl; return false;}
  return true;
}

void UIButton::render(SDL_Renderer* renderer){
  if(currentTexture){
    SDL_Rect dstRect = {x, y, width, height};
    SDL_RenderCopy(renderer, currentTexture, nullptr, &dstRect);
  }else{
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

void UIButton::toggleIcon(){
  if(clickedIconTexture && currentTexture == iconTexture){
      currentTexture = clickedIconTexture;
  }else if(iconTexture){currentTexture = iconTexture;}

}

