#ifndef TOOLBAR_HPP
#define TOOLBAR_HPP

#include <SDL.h>
#include <functional>
#include <map>
#include <memory>
#include <string>

class Toolbar {
public:
  Toolbar(SDL_Renderer *renderer, int width, int height);
  ~Toolbar();

  void render(SDL_Renderer *renderer);
  void handleEvent(const SDL_Event &event);
  void setPlayPauseCallback(std::function<void()> callback);
  void setStopCallback(std::function<void()> callback);
  void setVolumeChangeCallback(std::function<void(float)> callback);
  void setSeekCallback(std::function<void(float)> callback);
  void setProgress(double progress) { this->progress = progress; }
  void setIsPlaying(bool playing);
  void updateDimensions(int windowWidth, int windowHeight);

private:
  SDL_Rect playPauseButtonRect;
  SDL_Rect stopButtonRect;
  SDL_Rect volumeSliderRect;
  SDL_Rect progressBarRect;

  SDL_Texture *playTexture;
  SDL_Texture *pauseTexture;
  SDL_Texture *stopTexture;

  SDL_Renderer *renderer;
  bool isPlaying;
  int windowWidth;
  int windowHeight;

  SDL_Texture *loadTexture(const std::string &filePath);

  std::function<void()> playPauseCallback;
  std::function<void()> stopCallback;
  std::function<void(float)> volumeChangeCallback;
  std::function<void(float)> seekCallback;

  float volume;
  double progress;
  void renderVolumeSlider(SDL_Renderer *renderer);
  void renderProgressBar(SDL_Renderer *renderer);
};

#endif
