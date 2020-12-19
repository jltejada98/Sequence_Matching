//
// Created by Jose Luis Tejada on 14/12/20.
//

#ifndef SEQUENCE_MATCHING_MATCHLOCATIONS_H
#define SEQUENCE_MATCHING_MATCHLOCATIONS_H

#include <vector>
#include <mutex>
#include <unordered_map>
#include "MatchNode.h"

class MatchLocations { //Todo Chage all instances of list/vetor into map.
private:
    size_t  numOccurences;
    std::mutex mutex;
    std::unordered_map<std::string, std::shared_ptr<MatchNode>> matchMap;
public:
    MatchLocations();
    void addMatchLocation(size_t start1, size_t start2);
    void addSubMatchLocation(size_t start1, size_t start2);
    size_t getOccurences() const;
    bool matchExists(size_t start1, size_t start2);
    std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<MatchNode>>> getLocationsMap();
};


#endif //SEQUENCE_MATCHING_MATCHLOCATIONS_H
