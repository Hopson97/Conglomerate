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
        auto includeStr = tryExtractHeaderNameFromInclude(line);
        if (includeStr) {
            auto name = fs::path(*includeStr).filename().string();
            m_dependancies.emplace(idLookup.at(name));
        }
        else {
            if (!lineContainsPragmaOnce(line)) {
                m_fileContents.append(line + '\n');
            }
        }
    }
}
