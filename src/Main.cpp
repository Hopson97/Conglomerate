#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "Filesystem.h"
#include "Header.h"
#include "Util.h"
#include "HeaderSorter.h"


/*
    Gets all the source and header files in the root directory, and any child directories using .c and .cpp wildcard
*/
std::pair< std::vector<fs::path>, std::vector<fs::path>> findFiles()
{
    std::vector<fs::path> sourcePaths;
    std::vector<fs::path> headerPaths;
    for (auto& entry : fs::recursive_directory_iterator(fs::current_path())) {
        const auto& path = entry.path();
        if (path.extension() == ".cpp" || path.extension() == ".c") {
            sourcePaths.emplace_back(path);
        }
        else if (path.extension() == ".h" || path.extension() == ".hpp") {
            headerPaths.emplace_back(path);
        }
    }
    return { sourcePaths, headerPaths };
}

std::ofstream getOutputFile()
{
    const auto folder = fs::current_path() / "glom_output";
    const auto outputFile = folder / "out.cpp";

    if (!fs::exists(folder))
        fs::create_directory(folder);

    if (fs::exists(outputFile)) {
        fs::remove(outputFile);
    }

    std::ofstream outFile(outputFile);
    return outFile;
}

int main(int argc, char** argv)
{
    std::ofstream outFile = getOutputFile();
    auto t = timeFunction([&]() {
        std::cout << "Searching for C++ files...\n";
        auto [sourcePaths, headerPaths] = findFiles();
        std::cout << "Number of C++ source files found: " << sourcePaths.size() << '\n';
        std::cout << "Number of C++ header files found: " << headerPaths.size() << '\n';

        std::cout << "Sorting headers by their dependancies...\n";
        std::vector<Header> headerFiles;
        std::unordered_map<std::string, unsigned> headerIDs;
        for (auto& headerPath : headerPaths) {
            auto& header = headerFiles.emplace_back(headerPath);
            headerIDs.emplace(header.getFileName().string(), header.getID());
        }

        for (auto& header : headerFiles) {
            header.createDependaciesList(headerIDs);
        }

        sortHeaders(headerFiles);
        std::cout << "Outputting code...\n";

        for (auto& header : headerFiles) {
            outFile << header.getFileContent();
        }

        for (auto& sourceFile : sourcePaths) {
            std::ifstream inFile(sourceFile);
            std::string line;
            while (std::getline(inFile, line)) {
                if (!lineContainsInclude(line)) {
                    outFile << line << '\n';
                }
            }
        }
    });

    std::cout << "Done. Time taken: " << t << "ms" << std::endl;
}