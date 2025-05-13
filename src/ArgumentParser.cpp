#include "ArgumentParser.hpp"

ArgumentParser::ArgumentParser(int argc, char* argv[]) : m_argc(argc)
{
	for (int i = 0; i < argc; ++i) {
		m_argv.push_back(std::string(argv[i]));
	}
}


ArgumentParser::~ArgumentParser() {}

void ArgumentParser::printUsage(const std::string& programName) {
	std::cout << "Usage: ./" << std::filesystem::path(programName).filename().string() << " -i <file_path> [options]\n"
		<< "Options:\n"
		<< "  -h, --help         Show help message\n"
		<< "  -i <input.jpg/...> Input file to convert\n"
		<< "  -o <output.txt>    Save ASCII output to a file\n"
		<< "  -s <scale>         Adjust ASCII scaling (default: 1.0)\n"
		<< "  -d                 Resize image before processing\n"
		<< "  -r                 Reverse grayscale of output \n"
		<< "  -v                 Extra information about parsed file \n"
		<< std::endl;
}

void ArgumentParser::printBanner() {
	std::ifstream file("assets/banner.txt");
	if (!file) return;
	std::string line{};
	while (file) {
		std::getline(file, line);
		std::cout << line << '\n';
	}
	file.close();
}

int ArgumentParser::checkArgumentValidity()
{
	if (m_argc < 2) {
		printBanner();
		return 0;
	}

	const std::vector<std::string> possibleOptions{
		"-h", "--help", "-o", "-s", "-d", "-r", "-v", "-i"
	};

	for (int arg = 1; arg < m_argc - 1; arg++) {
		if (m_argv[arg][0] == '-') {
			const auto option{ std::find(possibleOptions.begin(), possibleOptions.end(), m_argv[arg]) };

			if (option == possibleOptions.end()) {
				//std::cerr << "Error: invalid argument(s).\n";
				std::cerr << "Error: Invalid option(s)!\n";
				return 0;
			}
		}
	}

	return 1;
}

int ArgumentParser::parseArguments()
{
	if (!checkArgumentValidity()) {
		printUsage(m_argv[0]);
		return 0;
	}

	// todo if only ./ascii.exe filename OR ./ascii.exe filename [options] => auto -i

	for (int arg = 1; arg < m_argc; arg++) {
		if (m_argv[arg] == "-h" || m_argv[arg] == "--help") {
			std::cout << "This program is a simple, lightweight way to convert images of various formats into ascii art." << '\n';
			printUsage(m_argv[0]);
		}
		else if (m_argv[arg] == "-i") {
			if (arg + 1 >= m_argv.size() || m_argv[arg + 1][0] == '-') {
				std::cerr << "Error: -i requires a filename.\n";
				return 0;;
			}
			m_options["-i"] = m_argv[++arg];
		}
		else if (m_argv[arg] == "-o") {
			if (arg + 1 >= m_argv.size() || m_argv[arg + 1][0] == '-') {
				std::cerr << "Error: -o requires a filename.\n";
				return 0;;
			}
			m_options["-o"] = m_argv[++arg];
		}
		else if (m_argv[arg] == "-s") {
			if (arg + 1 >= m_argv.size() || m_argv[arg + 1][0] == '-') {
				std::cerr << "Error: -s requires a number for scale (default: 1.0).\n";
				return 0;
			}
			m_options["-s"] = m_argv[++arg];
		}
		else if (m_argv[arg] == "-v") {
			m_options["-v"] = "true";
		}
		else if (m_argv[arg] == "-d") {
			m_options["-d"] = "true";
		}
		else if (m_argv[arg] == "-r") {
			m_options["-r"] = "true";
		}
	}

	return 1;
}
