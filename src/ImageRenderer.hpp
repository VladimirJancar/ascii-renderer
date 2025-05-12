#pragma once

#include "stb_image.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

class ImageRenderer {
public:
    ImageRenderer(const std::unordered_map<std::string, std::string>& options);
    ~ImageRenderer();

    bool loadImage(const std::string& filePath);
    void convertToAscii();
    void render();
    void writeToFile();

private:
    const std::unordered_map<std::string, std::string>& m_options;
    std::string m_asciiChars;
    int m_width, m_height, m_channels;
    unsigned char* m_imageData;
    std::vector<std::string> m_asciiArt;

    char getAsciiChar(unsigned char grayscaleValue);
    void freeImage();
};