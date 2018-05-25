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

        void getDependancies(const std::unordered_map<std::string, unsigned>& idLookup);

    private:
        std::vector<unsigned> m_dependancies;
        fs::path m_fullPath;
        fs::path m_fileName;
        unsigned m_id;

        static unsigned id;
};