#include "MainMenuScene.hpp"
#include "UIButton.hpp"
#include <iostream>
#include <thread>
#include <future>
#include "VideoPlayerScene.hpp"
#include "FileDialog.hpp"
#include "nfd.hpp"
#include <memory>
#include <iostream>
std::string MainMenuScene::openFileDialog(){
      NFD_Init();

    nfdchar_t* outPath;

   
    nfdfilteritem_t filterItem[2] = {{"Video Files", "mp4,avi,mkv"}, {"Audio Files", "mp3,wav,flac"}};

  
    nfdopendialogu8args_t args = {0};
    args.filterList = filterItem;
    args.filterCount = 2;
    nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
    
    std::string filePath;
    if (result == NFD_OKAY) {
        filePath = outPath;
        NFD_FreePath(outPath);
    } else if (result == NFD_CANCEL) {
        filePath = "User pressed cancel.";
    } else {
        filePath = "Error: " + std::string(NFD_GetError());
    }

    NFD_Quit();

    return filePath;}

MainMenuScene::MainMenuScene(App* app) : app(app){}

void MainMenuScene::initialize(){
  auto sharedThis = shared_from_this();
    std::cout << "Shared This Address: " << sharedThis.get() << std::endl;
 
  auto playButton = std::make_shared<UIButton>(540, 335, 200, 50, "Play Video", [sharedThis]() {
 
    std::thread([sharedThis]() {
        std::string filePath = sharedThis->openFileDialog();
        if (!filePath.empty()) { sharedThis->app->changeScene(std::make_unique<VideoPlayerScene>(filePath, sharedThis->app->getRenderer()));}
    }).detach();  
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
