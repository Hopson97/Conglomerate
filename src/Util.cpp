#include "Util.h"

bool lineContainsInclude(const std::string & line)
{
    return 
        ((line.find("#include") != std::string::npos) &&
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
    if (line.find("#include") != std::string::npos) {
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
