#ifndef FILEDIALOG_HPP
#define FILEDIALOG_HPP

#include <string>

class FileDialog{
  public:
#if defined(__WIN32)
    FileDialog();
    ~FileDialog();
#elif defined(__APPLE__)
    FileDialog();
    ~FileDialog();
#elif defined __linux__
     FileDialog(int* argc, char*** argv);
    ~FileDialog(); 
#endif
    std::string openFile();
    
  private:
  //to add maybe some configurtion data later on
};

#endif
