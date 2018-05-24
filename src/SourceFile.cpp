#include "SourceFile.h"

#include <iostream>
#include <fstream>
#include <iterator>

SourceFile::SourceFile(const fs::path & fileLocation)  
    : m_rootDirectory   (fileLocation.parent_path())
{
    std::ifstream inFile(fileLocation);
    std::string line;
    while (std::getline(inFile, line)) {
        m_lines.push_back(line);
    }
}

std::vector<fs::path> SourceFile::getHeaderFiles()
{
    std::vector<fs::path> headerFiles;

    for (auto& line : m_lines) {
        if (line.find("#include") != std::string::npos) {
            auto loc = line.find("\"");
            if (loc != std::string::npos) {
                char c = line[++loc];
                std::string name;
                while (c != '\"') {
                    name.push_back(c);
                    c = line[++loc];
                }
                headerFiles.push_back(m_rootDirectory / name);
            }
        }
    }
    return headerFiles;
}
