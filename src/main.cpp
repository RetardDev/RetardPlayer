#include <iostream>
#include "App.hpp"
#include <gtk-3.0/gtk/gtk.h>
int main(int argc, char* argv[]){

//  gtk_init(&argc, &argv); 

  App app(&argc, &argv);
  app.run();

  return 0;
}
