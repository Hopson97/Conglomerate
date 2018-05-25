#include "Header.h"

#include <fstream>
#include <iostream>

#include "Util.h"

unsigned Header::id = 0;

Header::Header(const fs::path & path)
    :   m_fullPath  (path)
    ,   m_fileName  (path.filename())
    ,   m_id        (id++)
{ }

void Header::createDependaciesList(const std::unordered_map<std::string, unsigned>& idLookup)
{
    m_fileContents.reserve((unsigned)fs::file_size(m_fullPath));
    std::ifstream inFile(m_fullPath);
    std::string line;
    while (std::getline(inFile, line)) {
        auto s = tryExtractHeaderNameFromInclude(line);
        if (s != "-1") {
            auto name = fs::path(s).filename().string();
            m_dependancies.push_back(idLookup.at(name));
        }
        else {
            m_fileContents.append(line);
        }
    }
}