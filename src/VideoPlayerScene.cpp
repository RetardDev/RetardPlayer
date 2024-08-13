#include "VideoPlayerScene.hpp"
#include "VideoRenderer.hpp"
#include "VideoProcessor.hpp"
#include <chrono>
#include <thread>
VideoPlayerScene::VideoPlayerScene(const std::string& videoFile, SDL_Renderer* renderer)
  : videoRenderer(renderer), videoProcessor(){
  if(!videoProcessor.openVideo(videoFile.c_str())){
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

void VideoPlayerScene::update(){

  auto now = std::chrono::steady_clock::now();
  auto nextFrameTime = videoProcessor.getNextFrameTime();

  while(videoProcessor.readNextPacket()){
  AVFrame* frame = videoProcessor.decodeFrame();
        if (frame) {

            auto waitTime = std::chrono::duration_cast<std::chrono::milliseconds>(nextFrameTime-now);

            
            if(waitTime.count() > 0){std::this_thread::sleep_for(waitTime);}

            videoRenderer.renderFrame(frame);

            videoProcessor.updateLastFrameTime();
            now = std::chrono::steady_clock::now();
            nextFrameTime = videoProcessor.getNextFrameTime();
        }
  }

}

void VideoPlayerScene::render(SDL_Renderer* renderer){








  //  videoRenderer.prepareScene();

  //AVFrame* pFrame = videoProcessor.decodeFrame();
  //videoRenderer.renderFrame(frame);
  //videoRenderer.presentScene();

}
