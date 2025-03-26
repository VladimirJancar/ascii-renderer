#include <iostream>

#include "ImageRenderer.hpp"
#include "ArgumentParser.hpp"

int main(int argc, char* argv[]) {
    ArgumentParser argParser(argc, argv);
    argParser.parseArguments();

    //std::string imagePath = "assets/img2.png";
    std::string imagePath = argv[argc - 1];
    ImageRenderer renderer(argParser.m_info);
    if (!renderer.loadImage(imagePath)) {
        return 1;
    }

    renderer.convertToAscii();
    renderer.render();

    return 0;
}
