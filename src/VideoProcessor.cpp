#include "VideoProcessor.hpp"

VideoProcessor::VideoProcessor(std::shared_ptr<MediaProcessor> mediaProcessor)
    : mediaProcessor(mediaProcessor), img_convet_ctx(nullptr, sws_freeContext) {
}

VideoProcessor::~VideoProcessor() {}

void VideoProcessor::cleanUp() {
  // Implement any necessary cleanup logic, if needed.
}

bool VideoProcessor::openVideo() {
  auto codecContext = mediaProcessor->getCodecContext();

  if (!codecContext) {
    std::cerr << "Codec context is not initialised\n";
    return false;
  }

  std::cout << "VideoProcessor OpenVideo Executed\n";
  return true;
}

bool VideoProcessor::readNextPacket() {
  std::unique_lock<std::mutex> lock(mediaProcessor->getFrameMutex());
  int ret = av_read_frame(mediaProcessor->getFormatContext().get(),
                          mediaProcessor->getPacket().get());

  if (ret < 0) {
    if (ret == AVERROR_EOF) {
      std::cerr << "End of video reached\n";
    } else {
      std::cerr << "Error reading frame\n";
    }
    return false; // Mutex will be unlocked automatically
  }

  if (mediaProcessor->getPacket()->stream_index !=
      mediaProcessor->getVideoStreamIndex()) {
    av_packet_unref(mediaProcessor->getPacket().get());
    return false; // Mutex will be unlocked automatically
  }

  return true; // Mutex will be unlocked automatically
}

AVFrame *VideoProcessor::decodeFrame() {
  std::unique_lock<std::mutex> lock(mediaProcessor->getFrameMutex());
  auto packet = mediaProcessor->getPacket();
  auto codecContext = mediaProcessor->getCodecContext();
  auto frame = mediaProcessor->getFrame();

  int ret = avcodec_send_packet(codecContext.get(), packet.get());
  if (ret < 0) {
    std::cerr << "Error sending packet to decoder\n";
    return nullptr; // Mutex will be released automatically
  }

  ret = avcodec_receive_frame(codecContext.get(), frame.get());
  if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
    return nullptr; // No more frames or need more packets, mutex released
                    // automatically
  } else if (ret < 0) {
    std::cerr << "Error receiving frame from decoder\n";
    return nullptr; // Mutex will be released automatically
  }

  if (frame->width == 0 || frame->height == 0) {
    std::cerr << "Invalid source frame dimensions: " << frame->width << "x"
              << frame->height << std::endl;
    return nullptr; // Mutex will be released automatically
  }

  av_packet_unref(
      packet.get()); // Unreference packet after successful processing

  // Return the decoded YUV420P frame
  return frame.get();
}
