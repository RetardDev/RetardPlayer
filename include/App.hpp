#ifndef APP_HPP
#define APP_HPP

#include "Scene.hpp"
#include <memory>
#include <SDL2/SDL.h>
#ifdef __linux__
#include <gtk-3.0/gtk/gtk.h>
#endif
class App{
  public:
    App(int* argc, char*** argv);
    ~App();

    void run();
    void changeScene(std::unique_ptr<Scene> newScene);
    
    int* getArgc() const {return argc;};
    char*** getArgv() const {return argv;};


  private:
    void handleEvents();
    void update();
    void render();

    int* argc;
    char*** argv;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::unique_ptr<Scene> currentScene;
    bool isRunning;

    


};

#endif
