#ifndef SCENE_HPP
#define SCENE_HPP

#include <SDL2/SDL.h>

class Scene{
  public:
    virtual ~Scene() = default;
    virtual void handleInput(const SDL_Event& event) = 0;
    virtual void update() = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
};

#endif 
