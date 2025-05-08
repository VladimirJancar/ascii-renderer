#define STB_IMAGE_IMPLEMENTATION  // Only define in ONE .cpp file!
#include "ImageRenderer.hpp"


ImageRenderer::ImageRenderer(const std::unordered_map<std::string, std::string>& options) :
    m_imageData(nullptr), m_width(0), m_height(0), m_channels(0), m_options(options)
{
    m_asciiChars = (options.at("-r") == "true") ? "@&#*+=-:. " : " .:-=+*#&@";

    //const std::string ASCII_CHARS = "@&#*+=-:. ";
    //const std::string ASCII_CHARS = " .:~i=CX&";
    //const std::string ASCII_BLOCKS = "██▓▓▒▒░░  ";
}

ImageRenderer::~ImageRenderer() {
    freeImage();
}

bool ImageRenderer::loadImage(const std::string& filePath) {
    m_imageData = stbi_load(filePath.c_str(), &m_width, &m_height, &m_channels, 0);
    if (!m_imageData) {
        std::cerr << "Error: Failed to load image " << filePath << std::endl;
        return false;
    }
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


    // Calculate the new width (100 in this case) and adjust the height to keep the correct aspect ratio. The height is halved to account for the fact that terminal characters are usually taller than they are wide.
    int newWidth = 80;
    int newHeight = (m_height * newWidth) / m_width / 2;  // Aspect ratio correction


    m_asciiArt.clear();
    for (int h = 0; h < newHeight; h++) {
        std::string line = "";
        for (int w = 0; w < newWidth; w++) {
            // Rescale x, y coordinates to the original image size
            int origX = (w * m_width) / newWidth;
            int origY = (h * m_height) / newHeight;

            int pixelIndex = (origY * m_width + origX) * m_channels;

            // Bounds check to prevent out-of-bounds access
            if (pixelIndex < 0 || pixelIndex + 2 >= m_width * m_height * m_channels) {
                std::cerr << "Out of bounds access at pixelIndex=" << pixelIndex << std::endl;
                line += ' '; // Add space instead of crashing
                continue;
            }

            // Extract RGB values
            unsigned char r = m_imageData[pixelIndex + 0]; // Red
            unsigned char g = m_imageData[pixelIndex + 1]; // Green
            unsigned char b = m_imageData[pixelIndex + 2]; // Blue

            // Convert to grayscale
            unsigned char grayPixel = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
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
    for (const std::string& line : m_asciiArt) {
        std::cout << line << '\n';
    }
}

void ImageRenderer::freeImage() {
    if (m_imageData) {
        stbi_image_free(m_imageData);
        m_imageData = nullptr;
    }
}
