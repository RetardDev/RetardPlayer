#pragma once

extern "C"{
#include <SDL2/SDL.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
};

#include <string>
#include <mutex>
#include <queue>

class AudioProcessor {
  public:
    AudioProcessor();
    ~AudioProcessor();

    bool openAudioStream(const std::string& filePath);
    void startDecoding();
    void stopDecoding();

  private:
    void decodeLoop();
    void playAudio();



    std::thread decodeThread;
    bool isDecoding;
    std::mutex queueMutex;
    std::queue<AVFrame *> audioQueue;
    SDL_AudioSpec audioSpec;
    SDL_AudioDeviceID audioDevice;
};
