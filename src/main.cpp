#include <iostream>

#include "ImageRenderer.hpp"


void printUsage(const std::string& programName) {
	std::cout << "Usage: " << programName << " [options] <file_path>\n"
		<< "Options:\n"
		<< "  -h, --help        Show help message\n"
		<< "  -o <output.txt>   Save ASCII output to a file\n"
		<< "  -s <scale>        Adjust ASCII scaling (default: 1.0)\n"
		<< "  -d                Resize image before processing\n"
		<< "  -r                Reverse grayscale of output \n"
		<< "  -i                Extra information about parsed file \n"
		<< std::endl;
}

int checkArgumentValidity(int argc, char* argv[])
{
	if (argc < 2) {
		return 0;
	}

	const std::vector<std::string> possibleOptions{
		"-h", "--help", "-o", "-s", "-d", "-r", "-i"
	};

	for (int arg = 1; arg < argc-1; arg++) {
		const auto option{ std::find(possibleOptions.begin(), possibleOptions.end(), argv[1]) };

		if (option == possibleOptions.end()) {
			std::cerr << "Invalid operation argument!\n";
			return 0;
		}
	}

	return 1;
}

int main(int argc, char* argv[]) {
	if (!checkArgumentValidity(argc, argv)) {
		printUsage(argv[0]);
		return 1;
	}

    //std::string imagePath = argv[1];
    std::string imagePath = "assets/img2.png";

    ImageRenderer renderer;

    if (!renderer.loadImage(imagePath)) {
        return 1;
    }

    renderer.convertToAscii();
    renderer.render();

    return 0;
}
