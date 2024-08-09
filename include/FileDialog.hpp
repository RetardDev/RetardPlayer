#ifndef FILEDIALOG_HPP
#define FILEDIALOG_HPP

#include <string>

class FileDialog{
  public:
    FileDialog();
    ~FileDialog();

    std::string openFile();
    
  private:
  //to add maybe some configurtion data later on
};

#endif
