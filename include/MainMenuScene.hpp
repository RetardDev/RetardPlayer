#ifndef MAINMENUSCENE_HPP
#define MAINMENUSCENE_HPP

#include "Scene.hpp"
#include "UIManager.hpp"
#include "App.hpp"
#include <memory>
class MainMenuScene : public Scene, public std::enable_shared_from_this<MainMenuScene>{
  public:
    explicit MainMenuScene(App* app);
    ~MainMenuScene(){
      std::cout << "MainMenuScene destroyed" << std::endl;
    }

    void handleInput(const SDL_Event& event) override;
    void update() override;
    void render(SDL_Renderer* renderer) override;
    std::string openFileDialog();
    
    void initialize();

  private:
    App* app;
    UIManager uiManager;

};

#endif


