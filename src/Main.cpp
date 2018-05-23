#include <unordered_set>
#include <iostream>
#include "Filesystem.h"

void iterateFiles(const fs::path& directory)
{
    for (auto& entry : fs::directory_iterator(directory)) {
        const auto& path = entry.path();
    }
}

int main()
{
    std::unordered_set<std::string> sourceFiles;
    std::unordered_set<std::string> headerFiles;
    fs::directory_iterator itr(fs::current_path());

    iterateFiles(fs::current_path());






    std::cin.ignore();
}