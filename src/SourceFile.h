#pragma once

#include <vector>
#include "Filesystem.h"

/**
    Represents a single .c or .cpp file
*/
class SourceFile
{
    public:
        SourceFile(const fs::path& fileLocation);


    private:
        std::vector<std::string> m_lines;
};