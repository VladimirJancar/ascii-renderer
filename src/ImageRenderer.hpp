#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}

class ImageRenderer {
public:
    ImageRenderer(const std::unordered_map<std::string, std::string>& options);
    ~ImageRenderer();

    bool loadImage(const std::string& filename);
    void convertToAscii();
    void render();
    void writeToFile();

private:
    const std::unordered_map<std::string, std::string>& m_options;
    std::string m_asciiChars;
    int m_width, m_height, m_channels;
    int m_lineSize;
    uint8_t* m_imageData;
    std::vector<std::string> m_asciiArt;

    AVPacket* m_packet;
    AVFrame* m_frame;
    AVCodecContext* m_codecCtx;
    AVFormatContext* m_fmtCtx;

    char getAsciiChar(unsigned char grayscaleValue);
    void freeImage(AVPacket* packet, AVFrame* frame, AVCodecContext* codec_ctx, AVFormatContext* fmt_ctx);
};