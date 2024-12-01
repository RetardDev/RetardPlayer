#ifndef APP_HPP
#define APP_HPP

#include "Scene.hpp"
#include <SDL2/SDL.h>
#include <memory>

class App {
public:
  App(int *argc, char ***argv);
  ~App();

  void run();
  void changeScene(std::shared_ptr<Scene> newScene);

  int *getArgc() const { return argc; };
  char ***getArgv() const { return argv; };

  SDL_Renderer *getRenderer() const;
  SDL_Window *getWindow() const { return window; }

private:
  void handleEvents();
  void update() const;
  void render() const;

  int *argc;
  char ***argv;
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::shared_ptr<Scene> currentScene;
  bool isRunning;
};

#endif
