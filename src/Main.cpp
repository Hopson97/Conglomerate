#include <iostream>
#include <vector>

#include "Filesystem.h"
#include "SourceFile.h"

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
}