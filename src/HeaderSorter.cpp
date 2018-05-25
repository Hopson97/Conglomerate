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

    for (auto& header : headers) {
        std::cout << std::setw(3) << header.getID() << " " << header.getDependancies().size() << " depend on ";
        for (auto d : header.getDependancies()) {
            std::cout << std::setw(3) << d << " ";
        }
        std::cout << "\n";
    }
}