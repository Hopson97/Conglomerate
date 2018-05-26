#pragma once

#include <unordered_set>
#include <unordered_map>
#include "Filesystem.h"

class Header
{
    public:
        Header(const fs::path& path);

        const fs::path& getFileName()   const { return m_fileName; }
        unsigned        getID()         const { return m_id;       }

        void createDependaciesList(const std::unordered_map<std::string, uint16_t>& idLookup);

        const std::unordered_set<uint16_t>& getDependancies() const { return m_dependancies; }

        const std::string& getFileContent() { return m_fileContents;  }

    private:
        std::string m_fileContents;
        std::unordered_set<uint16_t> m_dependancies;
        fs::path m_fullPath;
        fs::path m_fileName;
        uint16_t m_id;

        static uint16_t id;
};