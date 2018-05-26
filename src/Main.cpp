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

std::ofstream getOutputFile(int argc, char** argv)
{
    std::cout << "Parsing  command line arguments....\n";
    std::string outputFileName = "out.cpp";
    if (argc > 1) {
        std::cout << "Arguments found...\n";
        for (int i = 1; i < argc; i++) {
            if (std::string(argv[i]) == "-o") {
                if (argc >= i + 1) {
                    outputFileName = argv[i + 1];
                }
            }
        }
    }
    std::cout << "Finished parsing command line arguments.\n";
    const auto folder = fs::current_path() / "glom_output";
    const auto outputFile = folder / outputFileName;

    if (!fs::exists(folder))
        fs::create_directory(folder);

    if (fs::exists(outputFile)) {
        fs::remove(outputFile);
    }

    std::ofstream outFile(outputFile);
    return outFile;
}

std::vector<Header> getSortedHeaderList(const std::vector<fs::path>& headerPaths)
{
    std::cout << "Sorting headers by their dependancies...\n";
    std::vector<Header> headerFiles;
    std::unordered_map<std::string, uint16_t> headerIDs;
    for (auto& headerPath : headerPaths) {
        auto& header = headerFiles.emplace_back(headerPath);
        headerIDs.emplace(header.getFileName().string(), header.getID());
    }

    for (auto& header : headerFiles) {
        header.createDependaciesList(headerIDs);
    }

    sortHeaders(headerFiles);
    return headerFiles;
}



int main(int argc, char** argv)
{
    std::ofstream outFile = getOutputFile(argc, argv);
    auto t = timeFunction([&]() {
        //Get all the source and header files
        std::cout << "Searching for C++ files...\n";
        auto [sourcePaths, headerPaths] = findFiles();
        std::cout << "Number of C++ source files found: " << sourcePaths.size() << '\n';
        std::cout << "Number of C++ header files found: " << headerPaths.size() << '\n';

        //Sort the header files 
        std::vector<Header> headerFiles = getSortedHeaderList(headerPaths);

        //Output the sorted header files to the final file
        for (auto& header : headerFiles) {
            outFile << header.getFileContent();
        }

        //Output the source files to the final file
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

    std::cout << "Done. Time taken: " << t / 1000.0 << "ms" << std::endl;
}