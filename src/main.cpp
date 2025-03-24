#define STB_IMAGE_IMPLEMENTATION  // Only define in ONE .cpp file!
#include "stb_image.h"
#include <iostream>

const char* ASCII_CHARS = " .:-=+*#%@";

//todo add flax for non-fixed cmd size and font size

char grayscaleToASCII(unsigned char gray) {
    int asciiIndex = (gray * (strlen(ASCII_CHARS) - 1)) / 255;
    return ASCII_CHARS[asciiIndex];
}

int main() {
    int width, height, channels;

    // Load an image (force 3 channels = RGB)
    unsigned char* img = stbi_load("assets/img2.png", &width, &height, &channels, 3);

    if (!img) {
        std::cerr << "Failed to load image!\n";
        return 1;
    }

    // todo -i flag for extra info
    std::cout << "Image Loaded: " << width << "x" << height << " Channels: " << channels << "\n";

    // We calculate the new width (100 in this case) and adjust the height to keep the correct aspect ratio. The height is halved to account for the fact that terminal characters are usually taller than they are wide.
    int newWidth = 95;
    int newHeight = (height * newWidth) / width / 2;  // Aspect ratio correction


    // Read pixels
    for (int h = 0; h < newHeight; h++) {
        for (int w = 0; w < newWidth; w++) {
            // Rescale x, y coordinates to the original image size
            int origX = (w * width) / newWidth;
            int origY = (h * height) / newHeight;

            // Get the index of the pixel (RGB)
            int index = (origY * width + origX) * 3;

            // Extract RGB values
            unsigned char r = img[index + 0]; // Red
            unsigned char g = img[index + 1]; // Green
            unsigned char b = img[index + 2]; // Blue

            // Convert to grayscale
            unsigned char gray = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);

            std::cout << grayscaleToASCII(gray);
        }
    }
    

    // Free the image memory
    stbi_image_free(img);

    return 0;
}
