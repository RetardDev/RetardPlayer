#include "App.hpp"
#include "MainMenuScene.hpp"
#include <iostream>

App::App(int* argc, char*** argv):window(nullptr), renderer(nullptr), isRunning(true),
  argc(argc), argv(argv)
{

  if(SDL_Init(SDL_INIT_VIDEO) < 0){
      std::cerr << "Failed to initialise SDL: " << SDL_GetError() << std::endl;
      isRunning = false;
      return;
    }
    
    window = SDL_CreateWindow("Retard Player" , SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
    if(!window){
      std::cerr << "Failed to create window: " << SDL_GetError();
      isRunning = false;
      return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer){
      std::cerr << "Failed to create renderer: " << SDL_GetError();
      isRunning = false;
      return;
    }

    currentScene = std::make_unique<MainMenuScene>(this);
}

App::~App(){
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void App::run(){
  while(isRunning){
    handleEvents();
    update();
    render();

    SDL_Delay(16);
  }
}

void App::handleEvents(){
  SDL_Event event;
  while(SDL_PollEvent(&event)){
    if(event.type == SDL_QUIT){
      isRunning = false;
    }else if(currentScene){
      currentScene->handleInput(event);
    }
  }
}

void App::update(){
  if(currentScene){
    currentScene->update();
  }
}

void App:render(){
  if(currentScene){
    currentScene->render(renderer);
  }
}

void App::changeScene(std::unique_ptr<Scene> newScene){
  currentScene = std::move(newScene);
}



