#include "Util.h"

std::string tryExtractHeaderNameFromInclude(const std::string & line)
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
    return "-1";
}
