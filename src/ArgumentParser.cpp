#include "ArgumentParser.hpp"

ArgumentParser::ArgumentParser(int argc, char* argv[]) : m_argc(argc) {
	for (int i = 0; i < argc; ++i) {
		m_argv.push_back(std::string(argv[i]));
	}
}


ArgumentParser::~ArgumentParser() {}

void ArgumentParser::printUsage(const std::string& programName) {
	std::cout << "Usage: " << programName << " [options] <file_path>\n"
		<< "Options:\n"
		<< "  -h, --help        Show help message\n"
		<< "  -o <output.txt>   Save ASCII output to a file\n"
		<< "  -s <scale>        Adjust ASCII scaling (default: 1.0)\n"
		<< "  -d                Resize image before processing\n"
		<< "  -r                Reverse grayscale of output \n"
		<< "  -v                Extra information about parsed file \n"
		<< std::endl;
}

int ArgumentParser::checkArgumentValidity()
{
	if (m_argc < 2) {
		return 0;
	}

	const std::vector<std::string> possibleOptions{
		"-h", "--help", "-o", "-s", "-d", "-r", "-v"
	};

	for (int arg = 1; arg < m_argc - 1; arg++) {
		if (m_argv[arg][0] == '-') {
			const auto option{ std::find(possibleOptions.begin(), possibleOptions.end(), m_argv[arg]) };

			if (option == possibleOptions.end()) {
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

	for (int arg = 1; arg < m_argc - 1; arg++) {
		if (m_argv[arg] == "-h" || m_argv[arg] == "--help") {
			std::cout << "This program is a simple, lightweight way to convert images of various formats into ascii art." << '\n';
			printUsage(m_argv[0]);
		}
		else if (m_argv[arg] == "-o") {
			if (arg + 1 < m_argv.size()) {
				m_outputFile = m_argv[++arg];

				if (m_outputFile[0] == '-') {
					std::cerr << "Error: Invalid output file name.\n";
					exit(1);
				}
			}
			else {
				std::cerr << "Error: -o requires a filename.\n";
				exit(1);
			}
		}
	}

	return 1;
}