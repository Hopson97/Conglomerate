#include <iostream>
#include <vector>
#include <unordered_set>
#include "Filesystem.h"
#include "SourceFile.h"
#include "Util.h"

/*
    Gets all the source and header files in the root directory, and any child directories using .c and .cpp wildcard
*/
void findFiles(const fs::path& directory, 
    std::vector<fs::path>& sourcePaths, 
    std::vector<fs::path>& headerPaths)
{
    for (auto& entry : fs::recursive_directory_iterator(directory)) {
        const auto& path = entry.path();
        if (path.extension() == ".cpp" || path.extension() == ".c") {
            sourcePaths.emplace_back(path);
        }
        else if (path.extension() == ".h" || path.extension() == ".hpp") {
            headerPaths.emplace_back(path);
        }
    }
}

int main(int argc, char** argv)
{
    std::vector<fs::path> sourcePaths;
    std::vector<fs::path> headerPaths;

    std::cout << "Searching for C++ files...\n";
    findFiles(fs::current_path(), sourcePaths, headerPaths);
    std::cout << "Number of C++ source files found: " << sourcePaths.size() << "\n";
    std::cout << "Number of C++ header files found: " << headerPaths.size() << "\n";


    for (auto& header : headerPaths) {
        
    }

    /*
    for (auto& path : filePaths) {
        sourceFiles.emplace_back(path);
    }

    std::unordered_set<std::string> headerFileNames;
    for (auto& sourceFile : sourceFiles) {
        auto headers = sourceFile.getHeaderFiles();
        for (auto& header : headers) {
            headerFileNames.emplace(header.string());
            std::cout << fs::exists(header) << " " << header << std::endl;
        }
    }

    std::cout << "Found " << headerFileNames.size() << " header files\n";
    std::cout << "Loaded source files\n";
    */
}