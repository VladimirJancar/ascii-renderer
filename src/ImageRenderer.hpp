#pragma once

#include "stb_image.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

class ImageRenderer {
public:
    ImageRenderer(const std::unordered_map<std::string, std::string>& options);
    ~ImageRenderer();

    bool loadImage(const std::string& filePath);
    void convertToAscii();
    void render();

private:
    const std::unordered_map<std::string, std::string>& m_options;
    int m_width, m_height, m_channels;
    unsigned char* m_imageData;
    std::vector<std::string> m_asciiArt;

    char getAsciiChar(unsigned char grayscaleValue);
    void freeImage();
};