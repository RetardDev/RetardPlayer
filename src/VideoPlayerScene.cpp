#include "VideoPlayerScene.hpp"
#include "VideoRenderer.hpp"
#include "VideoProcessor.hpp"
#include <chrono>
#include <thread>

VideoPlayerScene::VideoPlayerScene(const std::string& videoFile, SDL_Renderer* renderer)
  : videoRenderer(renderer), videoProcessor(), isPlaying(true){
  if(videoProcessor.openVideo(videoFile.c_str())){
   // stopDecoding();
    startDecoding();
  }else{ std::cerr << "Failed top open video file: " << videoFile << std::endl;
  }
  
  controlBarRect = {0, 720 - 50, 1280, 50};
  
 auto playButton = std::make_shared<UIButton>(
      620, 680, 40, 40, "Play", [this]() {
          isPlaying = !isPlaying;
      }, renderer, "../assets/button/playButton.png", "../assets/button/pauseButton.png");
  
  uiManager.addElement(playButton);
  }



 
void VideoPlayerScene::handleInput(const SDL_Event& event){
  uiManager.handleEvent(event);
}

void VideoPlayerScene::startDecoding(){
  isDecoding = true;
  decodeThread = std::thread(&VideoPlayerScene::decodeLoop, this);
}

void VideoPlayerScene::decodeLoop(){
  while(isDecoding){
      if(isPlaying && videoProcessor.readNextPacket()){
        AVFrame* frame  = videoProcessor.decodeFrame();
        if(frame){
          std::lock_guard<std::mutex> lock(queueMutex);
          frameQueue.push(frame);
          queueCondition.notify_one();
        }
    }else{
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

void VideoPlayerScene::stopDecodingThread(){
  isDecoding = false;
  if(decodeThread.joinable()){decodeThread.join();}

  std::lock_guard<std::mutex> lock(queueMutex);
  while(!frameQueue.empty()){
      AVFrame* frame = frameQueue.front();
      av_frame_free(&frame);
      frameQueue.pop();
  }
}




void VideoPlayerScene::update(){
/*  if(isPlaying){
    auto now = std::chrono::steady_clock::now();
    if(now >= videoProcessor.getNextFrameTime()){
    if (videoProcessor.readNextPacket()){
    AVFrame* frame = videoProcessor.decodeFrame();
        if (frame) {
          currentFrame = frame;
       }
  }
    }
  }*/
}

void VideoPlayerScene::render(SDL_Renderer* renderer){
  //uiManager.render(renderer);
  videoRenderer.prepareScene();
//  SDL_SetRenderDrawColor(renderer, 0xD6, 0xEF, 0xD8, 0xFF);



  SDL_SetRenderDrawColor(renderer, 0x92, 0x37, 0x2C, 0xFF);
  SDL_RenderFillRect(renderer, &controlBarRect);
  uiManager.render(renderer);

/*  static auto lastRenderTime = std::chrono::steady_clock::now();
  auto now = std::chrono::steady_clock::now();
  double frameRate = videoProcessor.getFrameRate();

  if(frameRate <= 0.0){
    std::cerr << "Invalid frame rate: " << frameRate << std::endl;
    return;
  }

  auto frameDuration = std::chrono::milliseconds(static_cast<int>(1000.0/ frameRate));

  auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRenderTime).count();
  std::cerr << "TIme since last frame: "<< timeDiff << " ms, Frame duration: " << frameDuration.count() << " ms " << std::endl;
*/
//if(timeDiff >= 12){   
  {
  std::lock_guard<std::mutex> lock(queueMutex);
        if (!frameQueue.empty()) {
          //  if (currentFrame) {
            //    av_frame_free(&currentFrame);
            //}
            currentFrame = frameQueue.front();
            frameQueue.pop();
    
            if(currentFrame){
              videoRenderer.renderFrame(currentFrame);
            }
//            lastRenderTime = now;
        }
        videoRenderer.presentScene();
  }
        //}else if(timeDiff > 0){    
  //    SDL_Delay(static_cast<int>(timeDiff));
//}
    
 // auto elapsed = std::chrono::steady_clock::now() - now;
  //auto delay = frameDuration - elapsed;
//  if(delay.count() > 0){
  //  SDL_Delay(std::chrono::duration_cast<std::chrono::milliseconds>(delay).count());
  //} 
}

/*
VideoPlayerScene::~VideoPlayerScene() {
    stopDecodingThread();
    if (currentFrame) {
        av_frame_free(&currentFrame);
    }
}*/
