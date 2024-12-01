// #pragma once
#ifndef VIDEOPLAYERSCENE_HPP
#define VIDEOPLAYERSCENE_HPP

#include "AudioProcessor.hpp"
#include "MediaProcessor.hpp"
#include "Scene.hpp"
#include "ToolBar.hpp"
#include "UIManager.hpp"
#include "VideoProcessor.hpp"
#include "VideoRenderer.hpp"

#include <SDL_render.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

// class AudioProcessor;

class VideoPlayerScene : public Scene {
public:
  VideoPlayerScene(const std::string &videoFile, SDL_Renderer *renderer,
                   SDL_Window *window);
  ~VideoPlayerScene() override = default;

  void handleInput(const SDL_Event &event) override;
  void update() override;
  void render(SDL_Renderer *renderer) override;
  // std::queue<uint8_t *> audioQueue;
  // std::mutex audioQueueMutex;
  // std::condition_variable audioQueueCondition;
  std::chrono::steady_clock::time_point last_frame_time;
  double currentTime;

private:
  VideoRenderer videoRenderer;
  UIManager uiManager;
  AVFrame *currentFrame;
  uint8_t *currentAudioFrame;
  SDL_Window *window;
  SDL_Renderer *renderer;
  Toolbar *toolbar;
  int windowWidth;
  int windowHeight;
  int renderHeight;
  int renderWidth;
  int renderX;
  int renderY;

  std::thread decodeThread;
  std::queue<AVFrame *> frameQueue;
  std::mutex queueMutex;
  std::condition_variable queueCondition;
  bool isPlaying;
  bool isDecoding;
  SDL_Rect controlBarRect;
  std::shared_ptr<MediaProcessor> mediaProcessor;
  std::shared_ptr<VideoProcessor> videoProcessor;
  std::shared_ptr<AudioProcessor> audioProcessor;
  void decodeLoop();
  void startDecoding();
  void stopDecodingThread();

  void startAudio();
  void audioCallback(Uint8 *stream, int len);
  void resizeRenderer(int newWidth, int newHeight);
};

#endif
