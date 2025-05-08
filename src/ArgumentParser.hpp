#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

class ArgumentParser {
public:
    ArgumentParser(int m_argc, char* m_argv[]);
    ~ArgumentParser();

    void printUsage(const std::string& programName);
    int checkArgumentValidity();
    int parseArguments();
    const std::unordered_map<std::string, std::string>& getOptions() const {
        return m_options;
    }

private:
    std::unordered_map<std::string, std::string> m_options{
        {"-i", ""},    // input file
        {"-o", ""},    // output file
        {"-s", "1.0"}, // scale
        {"-v", ""},    // verbose mode
        {"-d", ""},    // resize image
        {"-r", ""}     // reverse grayscale
    };

    int m_argc;
    std::vector<std::string> m_argv;
};