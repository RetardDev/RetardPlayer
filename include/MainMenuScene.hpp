#ifndef MAINMENUSCENE_HPP
#define MAINMENUSCENE_HPP

#include "Scene.hpp"
#include "UIManager.hpp"
#include <memory>
#include "App.hpp"

class MainMenuScene : public Scene {
  public:
    MainMenuScene(App* app);
    ~MainMenuScene() = default;

    void handleInput(const SDL_Event& event) override;
    void update() override;
    void render(SDL_Renderer* renderer) override;

  private:
    App* app;
    UIManager uiManager;

};

#endif


