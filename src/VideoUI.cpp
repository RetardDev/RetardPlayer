#include "VideoUI.hpp"

VideoUI::VideoUI(SDL_Renderer *renderer, UIManager *uiManager)
    : renderer(renderer), uiManager(uiManager) {}

void VideoUI::createUIElements() {

  int buttonWidth = 50;
  int buttonHeight = 50;
  int screenWidth = 1280;
  int screenHeight = 720;

  int playButtonX = (screenWidth / 2) - (buttonWidth / 2);
  int playButtonY = screenHeight + 20;

  auto controlBar = std::make_shared<UIButton>(
      screenWidth, screenHeight - 100, screenWidth, 100, "", nullptr, renderer);

  uiManager->addElement(controlBar);
}

void VideoUI::handleInput(const SDL_Event &event) {
  uiManager->handleEvent(event);
}

void VideoUI::render() { uiManager->render(renderer); }
