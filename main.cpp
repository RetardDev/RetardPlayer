#include <iostream>
#include "VideoRenderer.hpp"

int main(){
  
  VideoRenderer app;

  while(true){
    
    app.prepareScene();
    app.handleInput();
    app.presentScene();
    SDL_Delay(16);
  }
  return 0;
}
