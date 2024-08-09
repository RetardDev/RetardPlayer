#include "VideoUI.hpp"

VideoUI::VideoUI(VideoRenderer* renderer) : videoRenderer(renderer){
  auto playButton = std::make_shared<UIButton>(50, 50, 100, 50, "Play", [](){
      std::cout << "Play button clicked" << std::endl;
      });

  auto pauseButton = std::make_shared<UIButton>(200, 50, 100, 50, "Pause", [](){  
      std::cout << "Pause button clicked" << std::endl;
      });

  uiManager.addElement(playButton);
  uiManager.addElement(pauseButton);
} 

void VideoUI::run(){
  while(true){
    videoRenderer->handleInput();
    videoRenderer->prepareScene();

    uiManager.render(videoRenderer->getRenderer());
    videoRenderer->presentScene();
    SDL_Delay(33);
  }
}


