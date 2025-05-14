#include "ImageRenderer.hpp"

ImageRenderer::ImageRenderer(const std::unordered_map<std::string, std::string>& options) :
    m_imageData(nullptr), m_width(0), m_height(0), m_channels(0), m_lineSize(0), m_options(options)
{
    m_asciiChars = (options.at("-r") == "true") ? "@&#*+=-:. " : " .:-=+*#&@";
}

ImageRenderer::~ImageRenderer() {
    if (m_imageData) {
        delete[] m_imageData;
        m_imageData = nullptr;
    }
}

bool ImageRenderer::loadImage(const std::string& filename) {
    // Init
    avformat_network_init();

    // Open the image file
    AVFormatContext* fmtCtx = nullptr;
    if (avformat_open_input(&fmtCtx, filename.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Could not open image file: " << filename << "\n";
        return false;
    }

    // Find stream information
    if (avformat_find_stream_info(fmtCtx, nullptr) < 0) {
        std::cerr << "Could not find stream information\n";
        avformat_close_input(&fmtCtx);
        return false;
    }

    // Find the first video stream
    int video_stream_idx = -1;
    const AVCodec* codec = nullptr;
    for (unsigned int i = 0; i < fmtCtx->nb_streams; i++) {
        if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            codec = avcodec_find_decoder(fmtCtx->streams[i]->codecpar->codec_id);
            break;
        }
    }
    if (video_stream_idx == -1 || !codec) {
        std::cerr << "Could not find video stream or codec\n";
        avformat_close_input(&fmtCtx);
        return false;
    }

    // Allocate codec context
    AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx || avcodec_parameters_to_context(codecCtx, fmtCtx->streams[video_stream_idx]->codecpar) < 0) {
        std::cerr << "Could not allocate or initialize codec context\n";
        avformat_close_input(&fmtCtx);
        return false;
    }

    // Open codec
    if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
        std::cerr << "Could not open codec\n";
        avcodec_free_context(&codecCtx);
        avformat_close_input(&fmtCtx);
        return false;
    }

    // Allocate packet and frame
    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    if (!packet || !frame) {
        std::cerr << "Could not allocate packet or frame\n";
        avcodec_free_context(&codecCtx);
        avformat_close_input(&fmtCtx);
        return false;
    }

    // Read and decode the image
    bool decoded = false;
    if (av_read_frame(fmtCtx, packet) >= 0 && packet->stream_index == video_stream_idx) {
        if (avcodec_send_packet(codecCtx, packet) >= 0) {
            if (avcodec_receive_frame(codecCtx, frame) >= 0) {
                decoded = true;
            }
        }
    }
    if (!decoded) {
        std::cerr << "Failed to decode image\n";
        return false;
    }

    m_width = frame->width;
    m_height = frame->height;
    m_channels = 3;

    // Convert to RGB24 using swscale
    AVFrame* rgb_frame = av_frame_alloc();
    if (!rgb_frame) {
        std::cerr << "Could not allocate RGB frame\n";
        return false;
    }

    // Allocate buffer for RGB data
    int buffer_size = av_image_alloc(rgb_frame->data, rgb_frame->linesize, m_width, m_height, AV_PIX_FMT_RGB24, 1);
    if (buffer_size < 0) {
        std::cerr << "Could not allocate RGB buffer\n";
        av_frame_free(&rgb_frame);
        return false;
    }

    // Set up swscale context
    struct SwsContext* sws_ctx = sws_getContext(
        frame->width, frame->height, static_cast<AVPixelFormat>(frame->format),
        m_width, m_height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!sws_ctx) {
        std::cerr << "Could not initialize swscale context\n";
        av_freep(&rgb_frame->data[0]);
        av_frame_free(&rgb_frame);
        return false;
    }

    // Convert to RGB24
    sws_scale(sws_ctx, frame->data, frame->linesize, 0, m_height, rgb_frame->data, rgb_frame->linesize);

    // Copy pixel data to m_imageData
    m_imageData = new uint8_t[buffer_size];
    memcpy(m_imageData, rgb_frame->data[0], buffer_size);
    m_lineSize = rgb_frame->linesize[0];

    // Clean up conversion resources
    sws_freeContext(sws_ctx);
    av_freep(&rgb_frame->data[0]);
    av_frame_free(&rgb_frame);

    std::cout << "Image loaded: " << m_width << "x" << m_height
        << ", format: RGB24, channels: " << m_channels << "\n";

    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&codecCtx);
    avformat_close_input(&fmtCtx);
    avformat_network_deinit();

    return true;
}

void ImageRenderer::convertToAscii() {
    if (!m_imageData) {
        std::cerr << "Error: No image loaded!\n";
        return;
    }

    if (m_options.at("-v") == "true") {
        std::cout << "Image Loaded: " << m_width << "x" << m_height << " Channels: " << m_channels << "\n";
    }

    // Calculate the new width and height for ASCII art
    int newWidth = 80;
    int newHeight = (m_height * newWidth) / m_width / 2; // Aspect ratio correction

    m_asciiArt.clear();
    for (int h = 0; h < newHeight; h++) {
        std::string line = "";
        for (int w = 0; w < newWidth; w++) {
            // Rescale x, y coordinates to the original image size
            int origX = (w * m_width) / newWidth;
            int origY = (h * m_height) / newHeight;

            // Use m_lineSize for row stride
            int pixelIndex = origY * m_lineSize + origX * m_channels;

            // Bounds check
            if (pixelIndex < 0 || pixelIndex + m_channels > m_width * m_height * m_channels) {
                std::cerr << "Out of bounds access at pixelIndex=" << pixelIndex << "\n";
                line += ' ';
                continue;
            }

            // Extract RGB values
            unsigned char r = m_imageData[pixelIndex + 0]; // Red
            unsigned char g = m_imageData[pixelIndex + 1]; // Green
            unsigned char b = m_imageData[pixelIndex + 2]; // Blue

            // Convert to grayscale
            unsigned char grayPixel = static_cast<unsigned char>(0.299 * r + 0.587 * g + 0.114 * b);
            line += getAsciiChar(grayPixel);
        }
        m_asciiArt.push_back(line);
    }
}

char ImageRenderer::getAsciiChar(unsigned char grayscaleValue) {
    int index = (grayscaleValue * (m_asciiChars.length() - 1)) / 255;
    return m_asciiChars[index];
}

void ImageRenderer::render() {
    if (!m_options.at("-o").empty()) {
        writeToFile();
    }
    else {
        for (const std::string& line : m_asciiArt) {
            std::cout << line << '\n';
        }
    }
}

void ImageRenderer::writeToFile() {
    std::ofstream outFile(m_options.at("-o"));
    if (!outFile) {
        std::cerr << "Error: Could not open file '" << m_options.at("-o") << "' for writing.\n";
        return;
    }

    for (const std::string& line : m_asciiArt) {
        outFile << line << '\n';
    }

    outFile.close();
    std::cout << "Image successfully converted and written into '" << m_options.at("-o") << "'.\n";
}
