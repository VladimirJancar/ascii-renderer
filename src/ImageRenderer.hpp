#pragma once

#include <string>
#include <vector>

class ImageRenderer {
public:
    ImageRenderer();
    ~ImageRenderer();

    bool loadImage(const std::string& filePath);
    void convertToAscii();
    void render();

private:
    int m_width, m_height, m_channels;
    unsigned char* m_imageData;
    std::vector<std::string> m_asciiArt;

    char getAsciiChar(unsigned char grayscaleValue);
    void freeImage();
};