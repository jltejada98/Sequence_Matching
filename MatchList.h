//
// Created by Jose Luis Tejada on 14/12/20.
//

#ifndef SEQUENCE_MATCHING_MATCHLIST_H
#define SEQUENCE_MATCHING_MATCHLIST_H

#include <vector>
#include <mutex>
#include <unordered_map>
#include "MatchNode.h"

class MatchList { //Todo Chage all instances of list/vetor into map.
private:
    size_t  numOccurences;
    std::mutex mutex;
    std::unordered_map<std::string, std::shared_ptr<MatchNode>> matchMap;
public:
    MatchList();
    void addMatch(size_t start1, size_t start2);
    void addSubMatch(size_t start1, size_t start2);
    size_t getOccurences() const;
    bool matchExists(size_t start1, size_t start2);
    std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<MatchNode>>> getMatchMap();
};


#endif //SEQUENCE_MATCHING_MATCHLIST_H
