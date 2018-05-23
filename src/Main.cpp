#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <regex>
#include <fstream>
#include <cstdlib>
#include "Filesystem.h"

bool startsWith(const std::string& string, const std::string& prefix) 
{
    return !std::strncmp(string.c_str(), prefix.c_str(), prefix.length());
}

void findSourceFiles(const fs::path& directory, std::vector<fs::path>& sources)
{
    for (auto& entry : fs::directory_iterator(directory)) {
        const auto& path = entry.path();
        if (path.extension() == ".cpp" || path.extension() == ".c") {
            sources.push_back(path);
        }
        else if (fs::is_directory(path)) {
            findSourceFiles(path, sources);
        }
    }
}

void findHeaderFiles(fs::path& file, std::unordered_set<std::string>& headers)
{
    std::cout << "\nLooking for header files in " << file << "\n";
    std::ifstream inFile(file);
    std::string line;
    while (std::getline(inFile, line)) {
        if (startsWith(line, "#include")) {
            line.erase(0, 9); //remove the #include 
            line.erase(line.length() - 1, line.length() - 1);
            std::cout << line << "\n";
        }
    }
    
}

int main(int argc, char** argv)
{
    std::vector<fs::path> sources;
    std::unordered_set<std::string> headers;

    findSourceFiles(fs::current_path(), sources);

    std::cout << "Number of source files found: " << sources.size() << "\n";
    std::cout << "Finding header files...\n";
    for (auto& sourceFile : sources) {
        findHeaderFiles(sourceFile, headers);
    }
}