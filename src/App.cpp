#include "App.hpp"
#include "MainMenuScene.hpp"
#include "VideoPlayerScene.hpp"
#include <iostream>

App::App(int* argc, char*** argv):argc(argc), argv(argv), window(nullptr), renderer(nullptr), isRunning(true)
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
      currentScene = std::make_shared<MainMenuScene>(this);

     auto scene = std::dynamic_pointer_cast<MainMenuScene>(currentScene);
  if(scene){scene->initialize();}
  
   }

App::~App(){
  std::cout << "App being deleted";
  if(argc){argc = nullptr;}
  if(argv){argv = nullptr;}
  
  currentScene.reset();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void App::run(){
  while(isRunning){
    handleEvents();
    update();
    render();

    //SDL_Delay(16);
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

void App::update() const{
  if(currentScene){
    currentScene->update();
  }
}

void App::render() const{
  if(currentScene){
    currentScene->render(renderer);
  }
}

void App::changeScene(std::shared_ptr<Scene> newScene){
 
  currentScene = std::move(newScene);
}

SDL_Renderer* App::getRenderer() const {return this->renderer;}

