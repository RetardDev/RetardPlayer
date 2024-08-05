#include <iostream>
#include <SDL2/SDL.h>

class VideoRenderer{
  public:
    VideoRenderer();
    ~VideoRenderer();

    void handleInput();
    void prepareScene();
    void presentScene();

  private:
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texure;
    SDL_Event event;

    void initSDL();
    
};
