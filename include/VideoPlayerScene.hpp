#ifndef VIDEOPLAYERSCENE_HPP
#define VIDEOPLAYERSCENE_HPP

#include "Scene.hpp"
#include "VideoProcessor.hpp"
#include "VideoRenderer.hpp"
#include "UIManager.hpp"
#include "MediaProcessor.hpp"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

class VideoPlayerScene : public Scene{ 
  public:
    VideoPlayerScene(const std::string& videoFile, SDL_Renderer* renderer);
    ~VideoPlayerScene() override = default;

    void handleInput(const SDL_Event& event) override;
    void update() override;
    void render(SDL_Renderer* renderer) override;

  private:
    VideoRenderer videoRenderer;
    //VideoProcessor videoProcessor;
    UIManager uiManager;
    AVFrame* currentFrame;

    std::thread decodeThread;
    std::queue<AVFrame*> frameQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    bool isPlaying;
    bool isDecoding;
    SDL_Rect controlBarRect;
    std::shared_ptr<MediaProcessor> mediaProcessor;
    std::shared_ptr<VideoProcessor> videoProcessor;

    void decodeLoop();
    void startDecoding();
    void stopDecodingThread();
};

#endif
