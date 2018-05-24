#include <iostream>
#include <vector>
#include <unordered_set>
#include "Filesystem.h"
#include "SourceFile.h"
#include "Util.h"

/*
    Locates all the source files in the root directory, and any child directories using .c and .cpp wildcard
*/
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

int main(int argc, char** argv)
{
    std::vector<fs::path> sourcePaths;
    std::vector<SourceFile> sourceFiles;

    std::cout << "Searching for source files...\n";
    findSourceFiles(fs::current_path(), sourcePaths);
    std::cout << "Number of source files found: " << sourcePaths.size() << "\n";


    for (auto& path : sourcePaths) {
        sourceFiles.emplace_back(path);
    }

    std::unordered_set<std::string> headerFileNames;
    for (auto& sourceFile : sourceFiles) {
        auto headers = sourceFile.getHeaderFiles();
        for (auto& header : headers) {
            headerFileNames.emplace(header);
        }
    }

    std::cout << "Found " << headerFileNames.size() << " header files\n";

    std::cout << "Loaded source files\n";
}