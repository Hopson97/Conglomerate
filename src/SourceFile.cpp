#include "SourceFile.h"

#include <iostream>
#include <fstream>

namespace 
{
    bool startsWith(const std::string& string, const std::string& prefix)
    {
        return !std::strncmp(string.c_str(), prefix.c_str(), prefix.length());
    }
}

SourceFile::SourceFile(const fs::path & fileLocation)
{
    std::ifstream inFile(fileLocation);
    std::string line;
    /**@TODO Possible optimization: Count number of lines and reserve space in vector*/
    while (std::getline(inFile, line)) {
        m_lines.push_back(line);
        /*
        if (startsWith(line, "#include")) {
            line.erase(0, 9); //remove the #include 
            line.erase(line.length() - 1, line.length() - 1);
            std::cout << line << "\n";
        }
        */
    }
}
