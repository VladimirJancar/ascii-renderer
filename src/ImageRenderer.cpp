#include "ImageRenderer.hpp"

ImageRenderer::ImageRenderer(const std::unordered_map<std::string, std::string>& options) :
    m_imageData(nullptr), m_width(0), m_height(0), m_channels(0), m_lineSize(0), m_options(options)
{
    m_asciiChars = (options.at("-r") == "true") ? "@&#*+=-:. " : " .:-=+*#&@";
}

ImageRenderer::~ImageRenderer() {
    freeImage(m_packet, m_frame, m_codecCtx, m_fmtCtx);
}

bool ImageRenderer::loadImage(const std::string& filename) {
    // Initialize FFmpeg
    avformat_network_init();

    // Open the image file
    m_fmtCtx = nullptr;
    if (avformat_open_input(&m_fmtCtx, filename.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Could not open image file: " << filename << "\n";
        return false;
    }

    // Find stream information
    if (avformat_find_stream_info(m_fmtCtx, nullptr) < 0) {
        std::cerr << "Could not find stream information\n";
        avformat_close_input(&m_fmtCtx);
        return false;
    }

    // Find the first video stream
    int video_stream_idx = -1;
    const AVCodec* codec = nullptr;
    for (unsigned int i = 0; i < m_fmtCtx->nb_streams; i++) {
        if (m_fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            codec = avcodec_find_decoder(m_fmtCtx->streams[i]->codecpar->codec_id);
            break;
        }
    }
    if (video_stream_idx == -1 || !codec) {
        std::cerr << "Could not find video stream or codec\n";
        avformat_close_input(&m_fmtCtx);
        return false;
    }

    // Allocate codec context
    m_codecCtx = avcodec_alloc_context3(codec);
    if (!m_codecCtx || avcodec_parameters_to_context(m_codecCtx, m_fmtCtx->streams[video_stream_idx]->codecpar) < 0) {
        std::cerr << "Could not allocate or initialize codec context\n";
        avformat_close_input(&m_fmtCtx);
        return false;
    }

    // Open codec
    if (avcodec_open2(m_codecCtx, codec, nullptr) < 0) {
        std::cerr << "Could not open codec\n";
        avcodec_free_context(&m_codecCtx);
        avformat_close_input(&m_fmtCtx);
        return false;
    }

    // Allocate packet and frame
    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();
    if (!m_packet || !m_frame) {
        std::cerr << "Could not allocate packet or frame\n";
        avcodec_free_context(&m_codecCtx);
        avformat_close_input(&m_fmtCtx);
        return false;
    }

    // Read and decode the image
    bool decoded = false;
    if (av_read_frame(m_fmtCtx, m_packet) >= 0 && m_packet->stream_index == video_stream_idx) {
        if (avcodec_send_packet(m_codecCtx, m_packet) >= 0) {
            if (avcodec_receive_frame(m_codecCtx, m_frame) >= 0) {
                decoded = true;
            }
        }
    }
    if (!decoded) {
        std::cerr << "Failed to decode image\n";
        freeImage(m_packet, m_frame, m_codecCtx, m_fmtCtx);
        return false;
    }

    // Set image properties
    m_width = m_frame->width;
    m_height = m_frame->height;
    m_channels = 3; // We'll convert to RGB24

    // Convert to RGB24 using swscale
    AVFrame* rgb_frame = av_frame_alloc();
    if (!rgb_frame) {
        std::cerr << "Could not allocate RGB frame\n";
        freeImage(m_packet, m_frame, m_codecCtx, m_fmtCtx);
        return false;
    }

    // Allocate buffer for RGB data
    int buffer_size = av_image_alloc(rgb_frame->data, rgb_frame->linesize, m_width, m_height, AV_PIX_FMT_RGB24, 1);
    if (buffer_size < 0) {
        std::cerr << "Could not allocate RGB buffer\n";
        av_frame_free(&rgb_frame);
        freeImage(m_packet, m_frame, m_codecCtx, m_fmtCtx);
        return false;
    }

    // Set up swscale context
    struct SwsContext* sws_ctx = sws_getContext(
        m_frame->width, m_frame->height, static_cast<AVPixelFormat>(m_frame->format),
        m_width, m_height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!sws_ctx) {
        std::cerr << "Could not initialize swscale context\n";
        av_freep(&rgb_frame->data[0]);
        av_frame_free(&rgb_frame);
        freeImage(m_packet, m_frame, m_codecCtx, m_fmtCtx);
        return false;
    }

    // Convert to RGB24
    sws_scale(sws_ctx, m_frame->data, m_frame->linesize, 0, m_height, rgb_frame->data, rgb_frame->linesize);

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

void ImageRenderer::freeImage(AVPacket* packet, AVFrame* frame, AVCodecContext* codec_ctx, AVFormatContext* fmt_ctx) {
    if (m_imageData) {
        delete[] m_imageData; // Free dynamically allocated pixel data
        m_imageData = nullptr;
    }
    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);
    avformat_network_deinit();
}