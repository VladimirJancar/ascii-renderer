#define STB_IMAGE_IMPLEMENTATION  // Only define in ONE .cpp file!
#include "stb_image.h"
#include <iostream>

#include <cstdlib>
#include <windows.h>


/*
* TODO add to file

void saveASCIIArtToFile(const std::string& asciiArt) {
    std::ofstream outFile("ascii_art.txt"); // Open text file for writing
    if (outFile.is_open()) {
        outFile << asciiArt;  // Write the ASCII art to the file
        outFile.close();      // Close the file after writing
        std::cout << "ASCII art saved to ascii_art.txt\n";
    } else {
        std::cerr << "Error opening file.\n";
    }
}

*/
// Function to set the CMD window size
//void setConsoleSize(int width, int height) {
//    // Create a new console screen buffer info object
//    SMALL_RECT sr = { 0, 0, width - 1, height - 1 };  // Set window size (width-1, height-1)
//    COORD coord = { width, height };                   // Set the screen buffer size (width, height)
//
//    // Set the size of the console screen buffer
//    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coord);
//
//    // Set the size of the console window
//    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &sr);
//}

const char* ASCII_CHARS = " .:-=+*#%@";

//todo add flax for non-fixed cmd size and font size

char grayscaleToASCII(unsigned char gray) {
    int asciiIndex = (gray * (strlen(ASCII_CHARS) - 1)) / 255;
    return ASCII_CHARS[asciiIndex];
}

int main() {
    //// Set console size (width x height)
    //setConsoleSize(250, 50);  // Set desired dimensions (example: 200x400)

    //// Set the console font size (optional)
    //CONSOLE_FONT_INFOEX cfi;
    //cfi.cbSize = sizeof(cfi);
    //cfi.dwFontSize.X = 8;  // Width of each character in pixels (smaller for higher resolution)
    //cfi.dwFontSize.Y = 16; // Height of each character in pixels (larger for better clarity)
    //cfi.FontFamily = FF_DONTCARE;
    //cfi.FontWeight = FW_NORMAL;
    //wcscpy_s(cfi.FaceName, L"Consolas"); // Set to a fixed font like "Consolas"
    //SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

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
    int newWidth = 120;
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
        std::cout << '\n';
    }
    

    // Free the image memory
    stbi_image_free(img);

    return 0;
}
