#pragma once

#include <unordered_map>
#include <vector>
#include "Filesystem.h"

class Header
{
    public:
        Header(const fs::path& path);

        const fs::path& getPath()       const { return m_fullPath; }
        const fs::path& getFileName()   const { return m_fileName; }
        unsigned        getID()         const { return m_id;       }

        void createDependaciesList(const std::unordered_map<std::string, unsigned>& idLookup);

        const std::vector<unsigned>& getDependancies() const { return m_dependancies; }

    private:
        std::string m_fileContents;
        std::vector<unsigned> m_dependancies;
        fs::path m_fullPath;
        fs::path m_fileName;
        unsigned m_id;

        static unsigned id;
};