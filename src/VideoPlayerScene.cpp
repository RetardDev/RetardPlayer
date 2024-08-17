#include "VideoPlayerScene.hpp"
#include "VideoRenderer.hpp"
#include "VideoProcessor.hpp"
#include <chrono>
#include <thread>

VideoPlayerScene::VideoPlayerScene(const std::string& videoFile, SDL_Renderer* renderer)
  : videoRenderer(renderer), videoProcessor(), isPlaying(false){
  if(!videoProcessor.openVideo(videoFile.c_str())){
    std::cerr << "Failed top open video file: " << videoFile << std::endl;
  }
  
  controlBarRect = {0, 720 - 50, 1280, 50};

  auto playButton = std::make_shared<UIButton>(
      10, 730, 40, 40, "", [this]() {
          isPlaying = !isPlaying;
      }, renderer, "../assets/button/playButton.png", "../assets/button/pauseButton.png" 
    );
  
  uiManager.addElement(playButton);
}

 
void VideoPlayerScene::handleInput(const SDL_Event& event){
  uiManager.handleEvent(event);
}

void VideoPlayerScene::update(){
  if(isPlaying){
  if (videoProcessor.readNextPacket()){
    AVFrame* frame = videoProcessor.decodeFrame();
        if (frame) {
          currentFrame = frame;
       }
  }
  }
}

void VideoPlayerScene::render(SDL_Renderer* renderer){
  videoRenderer.prepareScene();
  if(currentFrame){
    videoRenderer.renderFrame(currentFrame);
  }
  uiManager.render(renderer);

  videoRenderer.presentScene();
}
