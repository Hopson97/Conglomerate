
#include <filesystem>

namespace fs = std::filesystem;

#include <unordered_set>
#include <unordered_map>

class Header
{
    public:
        Header(const fs::path& path);

        const fs::path& getPath()       const { return m_fullPath; }
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

#include <vector>

void sortHeaders(std::vector<Header>& headerFiles);

#include <chrono>
#include <optional>

template<typename F>
auto timeFunction(F f)
{
    auto begin = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - begin;
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

bool lineContainsInclude    (const std::string& line);
bool lineContainsPragmaOnce (const std::string& line);
std::optional<std::string> tryExtractHeaderNameFromInclude(const std::string& line);

#include <fstream>
#include <iostream>


uint16_t Header::id = 0;

Header::Header(const fs::path & path)
    :   m_fullPath  (path)
    ,   m_fileName  (path.filename())
    ,   m_id        (id++)
{ }

void Header::createDependaciesList(const std::unordered_map<std::string, uint16_t>& idLookup)
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
                if (hasUnobservedDependacies) 
                    return false;
            }
            index++;
        }
        return true;
    }
}

void sortHeaders(std::vector<Header>& headers)
{
    /*
    std::vector<Header> sortedHeaders;

    for (auto& header : headers) {
        bool hasDependancy = false;
        std::cout << "ITR\n";
        for (int i = 0; i < sortedHeaders.size(); i++) {
            auto& d = header.getDependancies();
            if (d.find(sortedHeaders[i].getID()) != d.end()) {
                sortedHeaders.insert(sortedHeaders.begin(), header);
                hasDependancy = true;
                break;
            }
        }
        if (!hasDependancy) {
            sortedHeaders.push_back(header);
        }
    }

    headers = sortedHeaders;
    */
   while (!isSorted(headers));
}
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>


/*
    Gets all the source and header files in the root directory, and any child directories using .c and .cpp wildcard
*/
std::pair< std::vector<fs::path>, std::vector<fs::path>> findFiles()
{
    std::vector<fs::path> sourcePaths;
    std::vector<fs::path> headerPaths;
    for (auto& entry : fs::recursive_directory_iterator(fs::current_path())) {
        const auto& path = entry.path();
        if (path.extension() == ".cpp" || path.extension() == ".c") {
            sourcePaths.emplace_back(path);
        }
        else if (path.extension() == ".h" || path.extension() == ".hpp") {
            headerPaths.emplace_back(path);
        }
    }
    return { sourcePaths, headerPaths };
}

std::ofstream getOutputFile(int argc, char** argv)
{
    std::cout << "Parsing  command line arguments....\n";
    std::string outputFileName = "out.cpp";
    if (argc > 1) {
        std::cout << "Arguments found...\n";
        for (int i = 1; i < argc; i++) {
            if (std::string(argv[i]) == "-o") {
                if (argc >= i + 1) {
                    outputFileName = argv[i + 1];
                }
            }
        }
    }
    std::cout << "Finished parsing command line arguments.\n";
    const auto folder = fs::current_path() / "glom_output";
    const auto outputFile = folder / outputFileName;

    if (!fs::exists(folder))
        fs::create_directory(folder);

    if (fs::exists(outputFile)) {
        fs::remove(outputFile);
    }

    std::ofstream outFile(outputFile);
    return outFile;
}

std::vector<Header> getSortedHeaderList(const std::vector<fs::path>& headerPaths)
{
    std::cout << "Sorting headers by their dependancies...\n";
    std::vector<Header> headerFiles;
    std::unordered_map<std::string, uint16_t> headerIDs;
    for (auto& headerPath : headerPaths) {
        auto& header = headerFiles.emplace_back(headerPath);
        headerIDs.emplace(header.getFileName().string(), header.getID());
    }

    for (auto& header : headerFiles) {
        header.createDependaciesList(headerIDs);
    }

    sortHeaders(headerFiles);
    return headerFiles;
}



int main(int argc, char** argv)
{
    std::ofstream outFile = getOutputFile(argc, argv);
    auto t = timeFunction([&]() {
        //Get all the source and header files
        std::cout << "Searching for C++ files...\n";
        auto [sourcePaths, headerPaths] = findFiles();
        std::cout << "Number of C++ source files found: " << sourcePaths.size() << '\n';
        std::cout << "Number of C++ header files found: " << headerPaths.size() << '\n';

        //Sort the header files 
        std::vector<Header> headerFiles = getSortedHeaderList(headerPaths);

        //Output the sorted header files to the final file
        for (auto& header : headerFiles) {
            outFile << header.getFileContent();
        }

        //Output the source files to the final file
        for (auto& sourceFile : sourcePaths) {
            std::ifstream inFile(sourceFile);
            std::string line;
            while (std::getline(inFile, line)) {
                if (!lineContainsInclude(line)) {
                    outFile << line << '\n';
                }
            }
        }
    });

    std::cout << "Done. Time taken: " << t / 1000.0 << "ms" << std::endl;
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

std::optional<std::string> tryExtractHeaderNameFromInclude(const std::string & line)
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
    return {};
}
