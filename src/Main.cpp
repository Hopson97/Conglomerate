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
    std::string finalFile;
    auto t = timeFunction([&]() {
        std::vector<fs::path> sourcePaths;
        std::vector<fs::path> headerPaths;

        std::cout << "Searching for C++ files...\n";
        findFiles(fs::current_path(), sourcePaths, headerPaths);
        std::cout << "Number of C++ source files found: " << sourcePaths.size() << "\n";
        std::cout << "Number of C++ header files found: " << headerPaths.size() << "\n";

        std::cout << "Sorting headers by their dependancies...\n";
        std::vector<Header> headerFiles;
        std::unordered_map<std::string, unsigned> headerIDs;
        for (auto& headerPath : headerPaths) {
            headerFiles.emplace_back(headerPath);
            auto& header = headerFiles.back();
            headerIDs.emplace(std::make_pair(header.getFileName().string(), header.getID()));
        }

        for (auto& header : headerFiles) {
            header.createDependaciesList(headerIDs);
        }

        sortHeaders(headerFiles);

        for (auto& header : headerFiles) {
            finalFile.append(header.getFileContent());
        }

        for (auto& sourceFile : sourcePaths) {
            std::ifstream inFile(sourceFile);
            std::string line;
            while (std::getline(inFile, line)) {
                if (!lineContainsInclude(line)) {
                    finalFile.append(line + '\n');
                }
            }
        }

        const auto folder       = fs::current_path() / "glom_output";
        const auto outputFile   = folder / "out.cpp";
   
        if (!fs::exists(folder))
            fs::create_directory(folder);

        if (fs::exists(outputFile)) {
            fs::remove(outputFile);
        }

        std::ofstream outFile(fs::current_path() / "glom_output" / "out.cpp");
        outFile << finalFile;
    });

    std::cout << "Done. Time taken: " << t << "ms" << std::endl;
}