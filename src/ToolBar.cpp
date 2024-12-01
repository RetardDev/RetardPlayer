#include "ToolBar.hpp"
#include "App.hpp"
#include <SDL_image.h>
#include <iostream>

Toolbar::Toolbar(SDL_Renderer *renderer, int width, int height)
    : windowWidth(width), windowHeight(height), isPlaying(true), volume(0.5f),
      progress(0.0f), renderer(renderer) {
  playPauseButtonRect = {10, height - 60, 40, 40};
  stopButtonRect = {60, height - 60, 40, 40};
  volumeSliderRect = {110, height - 50, 200, 20};
  progressBarRect = {10, height - 80, width - 20, 10};

  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    printf("SDL_image could not initialize! SDL_image Error: %s\n",
           IMG_GetError());
  }
  playTexture = loadTexture("/home/cryptic/RetardPlayer/assets/play.png");
  pauseTexture = loadTexture("/home/cryptic/RetardPlayer/assets/pause.png");
  stopTexture = loadTexture("/home/cryptic/RetardPlayer/assets/stop.png");

  if (!playTexture || !stopTexture || !pauseTexture) {
    std::cerr << "Failed to load one or more textures for the toolbar."
              << std::endl;
  }
}

Toolbar::~Toolbar() {
  SDL_DestroyTexture(playTexture);
  SDL_DestroyTexture(pauseTexture);
  SDL_DestroyTexture(stopTexture);
}

SDL_Texture *Toolbar::loadTexture(const std::string &filePath) {
  SDL_Surface *toolbarImageSurface = IMG_Load(filePath.c_str());
  if (!toolbarImageSurface) {
    std::cerr << "Failed to load image: " << filePath << " - " << IMG_GetError()
              << std::endl;
    return nullptr;
  }

  SDL_Texture *toolbarImageTexture =
      SDL_CreateTextureFromSurface(renderer, toolbarImageSurface);
  SDL_FreeSurface(toolbarImageSurface);

  if (!toolbarImageTexture) {
    std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
    return nullptr;
  }
  return toolbarImageTexture;
}

void Toolbar::render(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Light gray background
  SDL_Rect toolbarRect = {0, windowHeight - 70, windowWidth, 70};
  SDL_RenderFillRect(renderer, &toolbarRect);

  SDL_Texture *buttonTexture = isPlaying ? pauseTexture : playTexture;
  SDL_RenderCopy(renderer, buttonTexture, nullptr, &playPauseButtonRect);

  SDL_RenderCopy(renderer, stopTexture, nullptr, &stopButtonRect);

  renderVolumeSlider(renderer);

  renderProgressBar(renderer);
}

void Toolbar::renderVolumeSlider(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gray
  SDL_RenderFillRect(renderer, &volumeSliderRect);

  SDL_Rect volumeLevelRect = volumeSliderRect;
  volumeLevelRect.w = static_cast<int>(volumeSliderRect.w * volume);
  SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255); // Blue color
  SDL_RenderFillRect(renderer, &volumeLevelRect);
}

void Toolbar::renderProgressBar(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gray
  SDL_RenderFillRect(renderer, &progressBarRect);

  SDL_Rect progressLevelRect = progressBarRect;
  progressLevelRect.w = static_cast<int>(progressBarRect.w * progress);
  SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); // Green color
  SDL_RenderFillRect(renderer, &progressLevelRect);
}

void Toolbar::handleEvent(const SDL_Event &event) {
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    int x = event.button.x;
    int y = event.button.y;

    SDL_Point point = {x, y};
    if (SDL_PointInRect(&point, &playPauseButtonRect)) {
      if (playPauseCallback)
        playPauseCallback();
    } else if (SDL_PointInRect(&point, &stopButtonRect)) {
      if (stopCallback)
        stopCallback();
    } else if (SDL_PointInRect(&point, &volumeSliderRect)) {
      volume = static_cast<float>(x - volumeSliderRect.x) / volumeSliderRect.w;
      if (volumeChangeCallback)
        volumeChangeCallback(volume);
    } else if (SDL_PointInRect(&point, &progressBarRect)) {
      progress = static_cast<float>(x - progressBarRect.x) / progressBarRect.w;
      if (seekCallback)
        seekCallback(progress);
    }
  }
}

void Toolbar::setPlayPauseCallback(std::function<void()> callback) {
  playPauseCallback = callback;
}

void Toolbar::setStopCallback(std::function<void()> callback) {
  stopCallback = callback;
}

void Toolbar::setVolumeChangeCallback(std::function<void(float)> callback) {
  volumeChangeCallback = callback;
}

void Toolbar::setSeekCallback(std::function<void(float)> callback) {
  seekCallback = callback;
}

void Toolbar::setIsPlaying(bool playing) { isPlaying = playing; }
void Toolbar::updateDimensions(int windowWidth, int windowHeight) {
  int toolbarHeight = 50;
  playPauseButtonRect = {20, windowHeight - toolbarHeight + 10, 30, 30};
  stopButtonRect = {60, windowHeight - toolbarHeight + 10, 30, 30};
  volumeSliderRect = {120, windowHeight - toolbarHeight + 15, 200, 20};
  progressBarRect = {350, windowHeight - toolbarHeight + 15, windowWidth - 400,
                     20};
  this->windowWidth = windowWidth;
  this->windowHeight = windowHeight;
}
