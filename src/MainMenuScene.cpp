#include "MainMenuScene.hpp"
#include "UIButton.hpp"
#include <iostream>
#include <thread>
#include "VideoPlayerScene.hpp"
#include "FileDialog.hpp"
MainMenuScene::MainMenuScene(App* app) : app(app){
  auto playButton = std::make_shared<UIButton>(540, 335, 200, 50, "Play Video", [this, app](){
    std::thread fileThread([this, app](){
      FileDialog fileDialog(app->getArgc(), app->getArgv());
      std::string filePath = fileDialog.openFile();
  
      if(!filePath.empty()){
      std::cout << "Play button clicked" << std::endl;
      std::cout << "File path is " << filePath << std::endl;
       app->changeScene(std::make_unique<VideoPlayerScene>(filePath));
      }else{
        std::cerr << "No file selected" << std::endl;
      }
    });
      
    fileThread.detach();
  });
  uiManager.addElement(playButton);

   auto exitButton = std::make_shared<UIButton>(
        540, 405, 200, 50, "Exit", []() {
            std::cout << "Exit button clicked" << std::endl;
            exit(0); 
        }
    );
    uiManager.addElement(exitButton);



}

void MainMenuScene::handleInput(const SDL_Event& event){
  uiManager.handleEvent(event);
}

void MainMenuScene::update(){}

void MainMenuScene::render(SDL_Renderer* renderer){
  SDL_SetRenderDrawColor(renderer, 0, 0, 0 , 255);
  SDL_RenderClear(renderer);

  uiManager.render(renderer);

  SDL_RenderPresent(renderer);
}
