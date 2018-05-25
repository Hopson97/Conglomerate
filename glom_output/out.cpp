
#include <vector>

void sortHeaders(std::vector<Header>& headerFiles);

#include <unordered_set>
#include <unordered_map>

class Header
{
    public:
        Header(const fs::path& path);

        const fs::path& getPath()       const { return m_fullPath; }
        const fs::path& getFileName()   const { return m_fileName; }
        unsigned        getID()         const { return m_id;       }

        void createDependaciesList(const std::unordered_map<std::string, unsigned>& idLookup);

        const std::unordered_set<unsigned>& getDependancies() const { return m_dependancies; }

        const std::string& getFileContent() { return m_fileContents;  }

    private:
        std::string m_fileContents;
        std::unordered_set<unsigned> m_dependancies;
        fs::path m_fullPath;
        fs::path m_fileName;
        unsigned m_id;

        static unsigned id;
};

#include <chrono>

template<typename F>
auto timeFunction(F f)
{
    auto begin = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - begin;
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

bool lineContainsInclude    (const std::string& line);
bool lineContainsPragmaOnce (const std::string& line);
std::string tryExtractHeaderNameFromInclude(const std::string& line);

#include <filesystem>

namespace fs = std::filesystem;

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#include <unordered_map>
#include <vector>

class Header
{
    public:
        Header(const fs::path& path);

        const fs::path& getPath()       const { return m_fullPath; }
        const fs::path& getFileName()   const { return m_fileName; }
        unsigned        getID()         const { return m_id;       }

        void createDependaciesList(const std::unordered_map<std::string, unsigned>& idLookup);

        const std::vector<unsigned>& getDependancies() const { return m_dependancies; }

        std::string&& getFileContent() { return std::move(m_fileContents);  }

    private:
        std::string m_fileContents;
        std::vector<unsigned> m_dependancies;
        fs::path m_fullPath;
        fs::path m_fileName;
        unsigned m_id;

        static unsigned id;
};

#include <vector>

void sortHeaders(std::vector<Header>& headerFiles);

#include <chrono>

template<typename F>
auto timeFunction(F f)
{
    auto begin = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - begin;
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

bool lineContainsInclude    (const std::string& line);
bool lineContainsPragmaOnce (const std::string& line);
std::string tryExtractHeaderNameFromInclude(const std::string& line);

#include <fstream>
#include <iostream>


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
            if (!lineContainsPragmaOnce(line)) {
                m_fileContents.append(line + '\n');
            }
        }
    }
}

#include <iostream>
#include <iomanip>
#include <unordered_set>

namespace 
{
    unsigned getIndexOfId(unsigned id, std::vector<Header>& headers, unsigned startFrom) 
    {
        for (unsigned i = startFrom; i < headers.size(); i++) {
            if (headers[i].getID() == id) {
                return i;
            }
        }
        return -1;
    }

    bool isSorted(std::vector<Header>& headers)
    {
        unsigned index = 0;
        std::unordered_set<unsigned> observedIDs;
        for (auto& header : headers) {
            observedIDs.emplace(header.getID());
            //Test if the dependacies of this header have been found
            bool hasUnobservedDependacies = false;
            for (auto dependancy : header.getDependancies()) {
                //If not, then move the dependancy above
                if (observedIDs.find(dependancy) == observedIDs.end()) {
                    hasUnobservedDependacies = true;
                    unsigned dependancyIndex = getIndexOfId(dependancy, headers, index);
                    while (dependancyIndex > index) {
                        std::swap(headers[dependancyIndex], headers[dependancyIndex - 1]);
                        dependancyIndex--;
                    }
                }
                //Reset the sort
                if (hasUnobservedDependacies) return false;
            }
            index++;
        }

        return true;
    }
}

void sortHeaders(std::vector<Header>& headers)
{
    std::sort(headers.begin(), headers.end(),
        [](const Header& a, const Header& b) {
            return a.getDependancies().size() < b.getDependancies().size();
    });

    while (true) {
        if (isSorted(headers)) {
            break;
        }
    }
}
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>


/*
    Gets all the source and header files in the root directory, and any child directories using .c and .cpp wildcard
*/
void findFiles(const fs::path& directory, 
    std::vector<fs::path>& sourcePaths, 
    std::vector<fs::path>& headerPaths)
{
    for (auto& entry : fs::recursive_directory_iterator(directory)) {
        const auto& path = entry.path();
        if (path.extension() == ".cpp" || path.extension() == ".c") {
            sourcePaths.emplace_back(path);
        }
        else if (path.extension() == ".h" || path.extension() == ".hpp") {
            headerPaths.emplace_back(path);
        }
    }
}

int main(int argc, char** argv)
{
    std::string finalFile;
    auto t = timeFunction([&]() {
        std::vector<fs::path> sourcePaths;
        std::vector<fs::path> headerPaths;

        std::cout << "Searching for C++ files...\n";
        findFiles(fs::current_path(), sourcePaths, headerPaths);
        std::cout << "Number of C++ source files found: " << sourcePaths.size() << "\n";
        std::cout << "Number of C++ header files found: " << headerPaths.size() << "\n";

        std::cout << "Sorting headers by their dependancies...\n";
        std::vector<Header> headerFiles;
        std::unordered_map<std::string, unsigned> headerIDs;
        for (auto& headerPath : headerPaths) {
            headerFiles.emplace_back(headerPath);
            auto& header = headerFiles.back();
            headerIDs.emplace(std::make_pair(header.getFileName().string(), header.getID()));
        }

        for (auto& header : headerFiles) {
            header.createDependaciesList(headerIDs);
        }

        sortHeaders(headerFiles);

        for (auto& header : headerFiles) {
            finalFile.append(header.getFileContent());
        }

        for (auto& sourceFile : sourcePaths) {
            std::ifstream inFile(sourceFile);
            std::string line;
            while (std::getline(inFile, line)) {
                if (!lineContainsInclude(line)) {
                    finalFile.append(line + '\n');
                }
            }
        }

    });

    const auto folder       = fs::current_path() / "glom_output";
    const auto outputFile   = folder / "out.cpp";
   
    if (!fs::exists(folder))
        fs::create_directory(folder);

    if (fs::exists(outputFile)) {
        fs::remove(outputFile);
    }

    std::ofstream outFile(fs::current_path() / "glom_output" / "out.cpp");
    outFile << finalFile;
    std::cout << t << "ms" << std::endl;
}

bool lineContainsInclude(const std::string & line)
{
    return 
        (line.find("\"") != std::string::npos) &&
        (line.find("<") == std::string::npos));

}

bool lineContainsPragmaOnce(const std::string & line)
{
    return
        line.find("#pragma once") != std::string::npos;
}

std::string tryExtractHeaderNameFromInclude(const std::string & line)
{
        auto loc = line.find("\"");
        if (loc != std::string::npos) {
            char c = line[++loc];
            std::string name;
            while (c != '\"') {
                name.push_back(c);
                c = line[++loc];
            }
            return name;
        }
    }
    return "-1";
}

#include <fstream>
#include <iostream>


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
            m_dependancies.emplace(idLookup.at(name));
        }
        else {
            if (!lineContainsPragmaOnce(line)) {
                m_fileContents.append(line + '\n');
            }
        }
    }
}

#include <iostream>
#include <iomanip>
#include <unordered_set>

namespace 
{
    unsigned getIndexOfId(unsigned id, std::vector<Header>& headers, unsigned startFrom) 
    {
        for (unsigned i = startFrom; i < headers.size(); i++) {
            if (headers[i].getID() == id) {
                return i;
            }
        }
        return -1;
    }

    bool isSorted(std::vector<Header>& headers)
    {
        unsigned index = 0;
        std::unordered_set<unsigned> observedIDs;
        for (auto& header : headers) {
            observedIDs.emplace(header.getID());
            //Test if the dependacies of this header have been found
            bool hasUnobservedDependacies = false;
            for (auto dependancy : header.getDependancies()) {
                //If not, then move the dependancy above
                if (observedIDs.find(dependancy) == observedIDs.end()) {
                    hasUnobservedDependacies = true;
                    unsigned dependancyIndex = getIndexOfId(dependancy, headers, index);
                    while (dependancyIndex > index) {
                        std::swap(headers[dependancyIndex], headers[dependancyIndex - 1]);
                        dependancyIndex--;
                    }
                }
                //Reset the sort
                if (hasUnobservedDependacies) return false;
            }
            index++;
        }

        return true;
    }
}

void sortHeaders(std::vector<Header>& headers)
{
    std::sort(headers.begin(), headers.end(),
        [](const Header& a, const Header& b) {
            return a.getDependancies().size() < b.getDependancies().size();
    });


    std::vector<Header> sortedHeaders;
    for (Header& header : headers) {
        if (header.getDependancies().size() == 0) {
            sortedHeaders.push_back(header);
        }
        else {
            break;
        }
    }
    for (Header& header : headers) {
        for (int i = 0; i < sortedHeaders.size(); i++) {
            const auto& deps = header.getDependancies();
            if (deps.find(sortedHeaders[i].getID()) != deps.end()) {
                sortedHeaders.insert(sortedHeaders.begin() + i, header);
                break;
            }
        }
    }

    headers = sortedHeaders;




   // while (!isSorted(headers));
}
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>


/*
    Gets all the source and header files in the root directory, and any child directories using .c and .cpp wildcard
*/
void findFiles(const fs::path& directory, 
    std::vector<fs::path>& sourcePaths, 
    std::vector<fs::path>& headerPaths)
{
    for (auto& entry : fs::recursive_directory_iterator(directory)) {
        const auto& path = entry.path();
        if (path.extension() == ".cpp" || path.extension() == ".c") {
            sourcePaths.emplace_back(path);
        }
        else if (path.extension() == ".h" || path.extension() == ".hpp") {
            headerPaths.emplace_back(path);
        }
    }
}

int main(int argc, char** argv)
{
    std::string finalFile;
    auto t = timeFunction([&]() {
        std::vector<fs::path> sourcePaths;
        std::vector<fs::path> headerPaths;

        std::cout << "Searching for C++ files...\n";
        findFiles(fs::current_path(), sourcePaths, headerPaths);
        std::cout << "Number of C++ source files found: " << sourcePaths.size() << "\n";
        std::cout << "Number of C++ header files found: " << headerPaths.size() << "\n";

        std::cout << "Sorting headers by their dependancies...\n";
        std::vector<Header> headerFiles;
        std::unordered_map<std::string, unsigned> headerIDs;
        for (auto& headerPath : headerPaths) {
            auto& header = headerFiles.emplace_back(headerPath);
            headerIDs.emplace(header.getFileName().string(), header.getID());
        }

        for (auto& header : headerFiles) {
            header.createDependaciesList(headerIDs);
        }

        sortHeaders(headerFiles);

        for (auto& header : headerFiles) {
            finalFile.append(header.getFileContent());
        }

        for (auto& sourceFile : sourcePaths) {
            std::ifstream inFile(sourceFile);
            std::string line;
            while (std::getline(inFile, line)) {
                if (!lineContainsInclude(line)) {
                    finalFile.append(line + '\n');
                }
            }
        }

        const auto folder       = fs::current_path() / "glom_output";
        const auto outputFile   = folder / "out.cpp";
   
        if (!fs::exists(folder))
            fs::create_directory(folder);

        if (fs::exists(outputFile)) {
            fs::remove(outputFile);
        }

        std::ofstream outFile(outputFile);
        outFile << finalFile;
    });

    std::cout << "Done. Time taken: " << t << "ms" << std::endl;
}

bool lineContainsInclude(const std::string & line)
{
    return 
        (line.find("\"") != std::string::npos) &&
        (line.find("<") == std::string::npos));

}

bool lineContainsPragmaOnce(const std::string & line)
{
    return
        line.find("#pragma once") != std::string::npos;
}

std::string tryExtractHeaderNameFromInclude(const std::string & line)
{
        auto loc = line.find("\"");
        if (loc != std::string::npos) {
            char c = line[++loc];
            std::string name;
            while (c != '\"') {
                name.push_back(c);
                c = line[++loc];
            }
            return name;
        }
    }
    return "-1";
}
