#include "HeaderSorter.h"

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