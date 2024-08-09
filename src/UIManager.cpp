#include "UIManager.hpp"

UIManager::UIManager(){}

UIManager::~UIManager(){}

void UIManager::addElement(std::shared_ptr<UIElement> element){
  elements.push_back(element);
}

void UIManager::render(SDL_Renderer* renderer){
  for (const auto& element : elements){
    element->render(renderer);
  }
}

void UIManager::handleEvent(const SDL_Event& event){
  for(const auto& element : elements){
    element->handleEvent(event);
  }
}
