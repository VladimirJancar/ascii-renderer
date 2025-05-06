#pragma once

#include <iostream>
#include <string>
#include <vector>

class ArgumentParser {
public:
    ArgumentParser(int m_argc, char* m_argv[]);
    ~ArgumentParser();

    void printUsage(const std::string& programName);
    int checkArgumentValidity();
    int parseArguments();

    bool m_info;
    std::string m_outputFile;

private:
    int m_argc;
    std::vector<std::string> m_argv;
};