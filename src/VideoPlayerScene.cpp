#include "VideoPlayerScene.hpp"
#include "VideoRenderer.hpp"
#include "VideoProcessor.hpp"
#include <chrono>
#include <thread>

VideoPlayerScene::VideoPlayerScene(const std::string& videoFile, SDL_Renderer* renderer)
  : videoRenderer(renderer), 
  mediaProcessor(std::make_shared<MediaProcessor>()), 
  videoProcessor(std::make_shared<VideoProcessor>(mediaProcessor)), 
  isPlaying(true), 
  isDecoding(false)
{
    if(!mediaProcessor->openMediaFile(videoFile)){
      std::cerr << "Media Processor could not open the file";
      return;
    }
    
    if(videoProcessor->openVideo()){
    //stopDecoding();
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
  std::cout << "Start Decoding Executed\n";
  isDecoding = true;
  decodeThread = std::thread(&VideoPlayerScene::decodeLoop, this);
}

void VideoPlayerScene::decodeLoop(){
  std::cout << "Decode Loop Executed\n";
  while(isDecoding){
      if(isPlaying && videoProcessor->readNextPacket()){
        AVFrame* frame  = videoProcessor->decodeFrame();
        if(frame){
          std::cout << "Frame successfully decodec and put in the queue\n";
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




void VideoPlayerScene::update(){}


void VideoPlayerScene::render(SDL_Renderer* renderer){
 videoRenderer.prepareScene();
    
    SDL_SetRenderDrawColor(renderer, 0x92, 0x37, 0x2C, 0xFF);
    SDL_RenderFillRect(renderer, &controlBarRect);
    uiManager.render(renderer);
   
    {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (!frameQueue.empty()) {
        currentFrame = frameQueue.front();
        frameQueue.pop();
    }
    if (currentFrame) {
        videoRenderer.renderFrame(currentFrame);
        videoRenderer.presentScene();
    }
    }
   
}

/*
VideoPlayerScene::~VideoPlayerScene() {
    stopDecodingThread();
    if (currentFrame) {
        av_frame_free(&currentFrame);
    }
}*/
