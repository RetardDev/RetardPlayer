#include "VideoRenderer.hpp"
#include <mutex>

VideoRenderer::VideoRenderer(SDL_Renderer *renderer)
    : renderer(renderer), texture(nullptr), swsContext(nullptr) {
  videoRect = {0, 0, 1280, 670};
}

VideoRenderer::~VideoRenderer() {
  if (texture) {
    SDL_DestroyTexture(texture);
  }
  if (swsContext) {
    sws_freeContext(swsContext);
  }
}

void VideoRenderer::prepareScene() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
}

void VideoRenderer::presentScene() { SDL_RenderPresent(renderer); }

void VideoRenderer::createTexture(int width, int height) {
  if (texture) {
    SDL_DestroyTexture(texture);
  }

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                              SDL_TEXTUREACCESS_STREAMING, width, height);
  if (!texture) {
    std::cerr << "SDL Create Texture Error : " << SDL_GetError() << std::endl;
  }
}

void VideoRenderer::renderFrame(AVFrame *pFrame,  SDL_Rect *targetRect) {
  std::lock_guard<std::mutex> lock(frameMutex);

  if (!pFrame) {
    std::cerr << "Error: Frame is null" << std::endl;
    return;
  }

  if (pFrame->width <= 0 || pFrame->height <= 0) {
    std::cerr << "Invalid frame dimensions: " << pFrame->width << "x"
              << pFrame->height << std::endl;
    return;
  }

  if (!texture || pFrame->width != videoRect.w ||
      pFrame->height != videoRect.h) {
    createTexture(pFrame->width, pFrame->height);
    //videoRect.w = pFrame->width;
   // videoRect.h = pFrame->height;
  }

  if (!swsContext) {
    swsContext =
        sws_getContext(pFrame->width, pFrame->height, AV_PIX_FMT_YUV420P,
                       pFrame->width, pFrame->height, AV_PIX_FMT_RGB24,
                       SWS_BILINEAR, nullptr, nullptr, nullptr);
  }

  // Create a temporary AVFrame for RGB conversion
  AVFrame *pFrameRGB = av_frame_alloc();
  int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pFrame->width,
                                          pFrame->height, 1);
  uint8_t *buffer =
      static_cast<uint8_t *>(av_malloc(numBytes * sizeof(uint8_t)));
  av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer,
                       AV_PIX_FMT_RGB24, pFrame->width, pFrame->height, 1);

  // Perform YUV to RGB conversion
  sws_scale(swsContext, pFrame->data, pFrame->linesize, 0, pFrame->height,
            pFrameRGB->data, pFrameRGB->linesize);

  void *pixels;
  int pitch;
  if (SDL_LockTexture(texture, nullptr, &pixels, &pitch) < 0) {
    std::cerr << "Failed to lock SDL texture: " << SDL_GetError() << std::endl;
    av_free(buffer);
    av_frame_free(&pFrameRGB);
    return;
  }

  for (int y = 0; y < pFrame->height; y++) {
    memcpy(static_cast<uint8_t *>(pixels) + y * pitch,
           pFrameRGB->data[0] + y * pFrameRGB->linesize[0], pFrame->width * 3);
  }

  SDL_UnlockTexture(texture);
  SDL_RenderCopy(renderer, texture, nullptr, targetRect);

  // Cleanup temporary RGB frame and buffer
  av_free(buffer);
  av_frame_free(&pFrameRGB);
}
