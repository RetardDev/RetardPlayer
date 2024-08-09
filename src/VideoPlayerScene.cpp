#include "VideoPlayerScene.hpp"

VideoPlayerScene::VideoPlayerScene(const std::string& videoFile)
  : videoRenderer(){
  if(!videoRenderer.openVideo(videoFile.c_str())){
    std::cerr << "Failed top open video file: " << videoFile << std::endl;
  }

    auto backButton = std::make_shared<UIButton>(10, 10, 100, 50, "Back", [](){
      std::cout << "Back button clicked" << std::endl;
    });

    uiManager.addElement(backButton);
}

void VideoPlayerScene::handleInput(const SDL_Event& event){
  uiManager.handleEvent(event);
}

void VideoPlayerScene::update(){}

void VideoPlayerScene::render(SDL_Renderer* renderer){
  videoRenderer.prepareScene();

  
  

}
