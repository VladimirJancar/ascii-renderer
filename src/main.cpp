#include <iostream>

#include "ImageRenderer.hpp"



int main(int argc, char* argv[]) {
    /*if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return 1;
    }*/

    //std::string imagePath = argv[1];
    std::string imagePath = "../assets/ship2_nb.png";
    ImageRenderer renderer;

    if (!renderer.loadImage(imagePath)) {
        return 1;
    }

    renderer.convertToAscii();
    renderer.render();

    return 0;
}

//todo add flag for non-fixed cmd size and font size
