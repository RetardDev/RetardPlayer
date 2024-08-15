#include "VideoProcessor.hpp"

VideoProcessor::VideoProcessor() : pFormatContext(nullptr), pCodecContext(nullptr), pFrame(nullptr), pPacket(nullptr), videoStreamIndex(-1), buffer(nullptr),img_convet_ctx(nullptr){}

VideoProcessor::~VideoProcessor(){
      if (pPacket) {
        av_packet_free(&pPacket);
    }
    if (pFrame) {
        av_frame_free(&pFrame);
    }
    if (pFrameYUV) {
        av_frame_free(&pFrameYUV);
    }
    if (pCodecContext) {
        avcodec_free_context(&pCodecContext);
    }
    if (pFormatContext) {
        avformat_close_input(&pFormatContext);
    }
    if (buffer) {
        av_free(buffer);
    }
    if (img_convet_ctx) {
        sws_freeContext(img_convet_ctx);
    }
}


void VideoProcessor::setFrameDuration(const std::chrono::milliseconds& duration){frameDuration = duration;}

std::chrono::steady_clock::time_point VideoProcessor::getNextFrameTime(){
  return lastFrameTime + frameDuration;
}

void VideoProcessor::updateLastFrameTime(){
  lastFrameTime = std::chrono::steady_clock::now();
}

void VideoProcessor::cleanUp(){
    if (pPacket) {
        av_packet_free(&pPacket);
        pPacket = nullptr;
    }
    if (pFrame) {
        av_frame_free(&pFrame);
        pFrame = nullptr;
    }
    if (pFrameYUV) {
        pFrameYUV = nullptr;
      //  av_frame_free(&pFrameYUV);

    }
    if (pCodecContext) {
        avcodec_free_context(&pCodecContext);
        pCodecContext = nullptr;
    }
    if (pFormatContext) {
        avformat_close_input(&pFormatContext);
        pFormatContext = nullptr;
    }
    if (buffer) {
        av_free(buffer);
        buffer = nullptr;
    }
    if (img_convet_ctx) {
        sws_freeContext(img_convet_ctx);
        img_convet_ctx = nullptr;
    }
}



bool VideoProcessor::openVideo(const char* filename){
 std::cout << "VideoProcessor::openVideo(const char*) called" << std::endl;
    pFormatContext = avformat_alloc_context();

    if (!pFormatContext) {
        std::cerr << "Could not allocate format context;\n";
        return 0;
    }

    if (avformat_open_input(&pFormatContext, filename, NULL, NULL) != 0) {
        std::cerr << "Could not open input stream\n"; return 0;
    }

    if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
        std::cerr << "Could not find stream info\n"; return 0;
    }

    videoStreamIndex = -1;
    for (unsigned i = 0; i < pFormatContext->nb_streams; i++) {
        if (pFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }
    if (videoStreamIndex == -1) {
        std::cerr << "Could not find a video stream\n";
        return 0;
    }

    AVCodecParameters* pCodecParameters = pFormatContext->streams[videoStreamIndex]->codecpar;
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (!pCodec) { std::cerr << "Codec not found\n"; return 0; }

    pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext) { std::cerr << "Could not allocate a video codec context\n"; return 0; }

    if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0) {
        std::cerr << "Failed to copy codec parameters to codec context\n"; return 0;
    }

    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0) { std::cerr << "Could not open codec\n"; return 0; }

    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    if (!pFrame || !pFrameYUV) { std::cerr << "Could not allocate frame\n"; return 0; }

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecContext->width, pCodecContext->height, 32);
    if (numBytes < 0) { std::cerr << "Invalid buffer size\n"; return 0; }

    buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    if (!buffer) { std::cerr << "Could not allocate buffer\n"; return 0; }

    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, buffer, AV_PIX_FMT_RGB24, pCodecContext->width, pCodecContext->height, 32);

    img_convet_ctx = sws_getContext(pCodecContext->width, pCodecContext->height, pCodecContext->pix_fmt,
                                    pCodecContext->width, pCodecContext->height, AV_PIX_FMT_RGB24, SWS_LANCZOS, NULL, NULL, NULL);
    if (!img_convet_ctx) { std::cerr << "Could not initialize sws context\n"; return 0; }

    pPacket = av_packet_alloc();
    if (!pPacket) { std::cerr << "Could not allocate packet\n"; return 0; }

    std::cout << "--------------- File Information ----------------\n";
    av_dump_format(pFormatContext, 0, filename, 0);
    std::cout << "-------------------------------------------------\n";



//will be removed on the next commit

 /*   AVStream* videoStream = pFormatContext->streams[videoStreamIndex];

    AVRational fps = videoStream->avg_frame_rate;
    if (fps.num == 0 || fps.den == 0) {
        std::cerr << "\nInvalid frame rate:\n" << "fps.num: " << fps.num << ", den: " << fps.den << std::endl;
        return 0;
    }

    double frameRate = av_q2d(fps);
    if (frameRate == 0) { std::cerr << "Frame rate cannot be zero\n"; return 0; }

    std::chrono::milliseconds frameDuration(static_cast<int>(1000.0 / frameRate));
    setFrameDuration(frameDuration);
*/
    std::cout << "videoProcessor openvideo function run smoothly\n";

    return true;
}

double VideoProcessor::getFrameRate() const{
  if(videoStreamIndex < 0 || !pFormatContext || pFormatContext->nb_streams <= videoStreamIndex){std::cerr << "Invalid video stream index"; return 0.0;} 
  AVStream* videoStream = pFormatContext->streams[videoStreamIndex];
  if(!videoStream){std::cerr << "Video stream not found\n"; return 0.0;}

  AVRational fps = videoStream->avg_frame_rate;
  if(fps.num == 0 || fps.den == 0){std::cerr << "Invalid frame rate: num = " << fps.num << ", den = " << fps.den << std::endl; return 0.0;}
  
  return av_q2d(fps);
}


bool VideoProcessor::readNextPacket(){
    av_packet_unref(pPacket);
  

  int ret = av_read_frame(pFormatContext, pPacket);
  if(ret < 0){
    if(ret == AVERROR_EOF){std::cerr << "End of video reached\n";
    }else{
      std::cerr << "Error reading frame: ";
    }
    return 0;  
  }
  
  if(pPacket->stream_index != videoStreamIndex){
    av_packet_unref(pPacket);
    return 0;
  }
  return true;
}

AVFrame* VideoProcessor::decodeFrame(){
    if(!pPacket || pPacket->size <= 0){std::cerr << "No packet to decode\n"; return nullptr;}

    int ret;

    ret = avcodec_send_packet(pCodecContext, pPacket);
    if(ret < 0){
        std::cerr << "Error sending packet to decoder\n";
        return nullptr;
    }

    ret = avcodec_receive_frame(pCodecContext, pFrame);
    if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
        return nullptr;
    }else if (ret < 0){
        std::cerr << "Error receiving frame from decoder\n";
        return nullptr;
    }

    if(pFrame->width == 0 || pFrame->height == 0){
        std::cerr << "Invalid source frame dimensions: " << pFrame->width << "x" << pFrame->height << std::endl;
        return nullptr;
    }

    if(!img_convet_ctx || pFrame->width != pCodecContext->width || pFrame->height != pCodecContext->height){
        if(img_convet_ctx){sws_freeContext(img_convet_ctx);}
        img_convet_ctx = sws_getContext(
            pFrame->width, pFrame->height, pCodecContext->pix_fmt,
            pFrame->width, pFrame->height, AV_PIX_FMT_RGB24,
            SWS_LANCZOS, nullptr, nullptr, nullptr
        );
        if(!img_convet_ctx){
            std::cerr << "Could not initialize the conversion context\n";
            return nullptr;
        }
    }

    pFrameYUV->width = pFrame->width;
    pFrameYUV->height = pFrame->height;
    pFrameYUV->format = AV_PIX_FMT_RGB24;

    sws_scale(img_convet_ctx, pFrame->data, pFrame->linesize, 0, pCodecContext->height, pFrameYUV->data, pFrameYUV->linesize);

    if (pFrameYUV->width == 0 || pFrameYUV->height == 0) {
        std::cerr << "Invalid frame dimensions after conversion: " << pFrameYUV->width << "x" << pFrameYUV->height << std::endl;
        return nullptr;
    }

    return pFrameYUV;
}
