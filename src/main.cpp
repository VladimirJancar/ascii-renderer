#include <iostream>

#include "ImageRenderer.hpp"
#include "ArgumentParser.hpp"

int main(int argc, char* argv[]) {
    ArgumentParser argParser(argc, argv);
    if (!argParser.parseArguments()) {
        return 1;
    }

    const std::unordered_map<std::string, std::string>& options = argParser.getOptions();

    ImageRenderer renderer(options);
    if (!renderer.loadImage(options.at("-i"))) {
        return 1;
    }

    renderer.convertToAscii();
    renderer.render();

    return 0;
}
