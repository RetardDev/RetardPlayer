#include "FileDialog.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <commdlg.h>
#include <shobjidl.h>
#include <string.h>
FileDialog::FileDialog(){}

FileDialog::~FileDialog(){}

//not tested
std::string FileDialog::openFile(){
  HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  std::string filePath;
  if(SUCCEEDED(hr))
  {
    IFileOpenDialog *pFileOpen;

    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    if(SUCCEEDED(hr))
    {
      hr = pFileOpen->Show(NULL);

      if(SUCCEEDED(hr))
      {
        IShellItem *pItem;
        hr = pFileOpen->GetResult(&pItem);
        if(SUCCEEDED(hr)){
          PWSTR pszFilePath;
          hr = pItem->GetDisplayName(SIGN_FILESYSPATH, &pszFilePath);

          if(SUCCEEDED(hr)){
            char path[MAX_PATH];
            wcstombs(path, pszFilePath, MAX_PATH);
            filePath = std::string(path);
            CoTaskMemFree(pszFilePath);
          }
          pItem->Release();
        }
      }
      pFileOpen->Release();
    }
    CoInitialize();
  }
  return filePath;
}

#elif defined(__APPLE__)
#include <Cocoa/Cocoa.h>

FileDialog::FileDialog(){}

FileDialog::~FileDialog(){}

std::string FileDialog::openFile(){
}


#elif defined __linux__
#include <gtk-3.0/gtk/gtk.h>

//tested on one machine
FileDialog::FileDialog(int* argc, char*** argv){
  gtk_init(argc, argv);
}

FileDialog::~FileDialog(){}

std::string FileDialog::openFile(){
  GtkWidget* dialog;
  std::string filePath;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint res;

  
  dialog = gtk_file_chooser_dialog_new ("Open File",
                                      NULL,
                                      action,
                                      "Cancel",
                                      GTK_RESPONSE_CANCEL,
                                      "Open",
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

  res = gtk_dialog_run(GTK_DIALOG(dialog));
  if(res == GTK_RESPONSE_ACCEPT)
  {
    char* filename;
    GtkFileChooser* chooser = GTK_FILE_CHOOSER (dialog);
    filename = gtk_file_chooser_get_filename(chooser);
    if(filename){
    filePath  = filename;
    g_free(filename);
    }
  }
  gtk_widget_destroy(dialog);

  return filePath;
}

/* replacement incase gtk does not work
std::string FileDialog::openFile(){
  char filename[1024] = "";
  FILE* f = popen("zenity --file-selection --title=\"Select a file\"", "r");

  if(f){
    if(fgets(filename, sizeof(filename), f)){
      std::string file_path(filename);
      file_path.erase(file_path.find_last_not_of("\n\r\t") + 1);
      pclose(f);
      return file_path;
    }
    pclose(f);
  }
  return "";
} */

#endif
