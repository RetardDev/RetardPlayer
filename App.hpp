#include "Scene.hpp"
#include <memory>
#include <SDL2/SDL.h>

class App{
  public:
    App();
    ~App();

    void run();

  private:
    void changeScene(std::unique_ptr<Scene> newScene);

    SDL_Window* window;
    SDL_Renderer* renderer;
    std::unique_ptr<Scene> currentScene;
    bool isRunning;
};
