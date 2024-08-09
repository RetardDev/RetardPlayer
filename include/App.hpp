#ifndef APP_HPP
#define APP_HPP

#include "Scene.hpp"
#include <memory>
#include <SDL2/SDL.h>

class App{
  public:
    App();
    ~App();

    void run();
    void changeScene(std::unique_ptr<Scene> newScene);
  
  private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::unique_ptr<Scene> currentScene;
    bool isRunning;
};

#endif
