#define STB_IMAGE_IMPLEMENTATION  // Only define in ONE .cpp file!
#include "stb_image.h"
#include <iostream>

int main() {
    int width, height, channels;

    // Load an image (force 3 channels = RGB)
    unsigned char* img = stbi_load("assets/img.jpg", &width, &height, &channels, 3);

    if (!img) {
        std::cerr << "Failed to load image!\n";
        return 1;
    }

    std::cout << "Image Loaded: " << width << "x" << height << " Channels: " << channels << "\n";

    // Read a pixel (example: pixel at x=10, y=10)
    int x = 10, y = 10;
    int index = (y * width + x) * 3;  // Multiply by 3 for RGB
    unsigned char r = img[index + 0];  // Red
    unsigned char g = img[index + 1];  // Green
    unsigned char b = img[index + 2];  // Blue

    std::cout << "Pixel at (10,10): R=" << (int)r << " G=" << (int)g << " B=" << (int)b << "\n";

    // Free the image memory
    stbi_image_free(img);

    return 0;
}
