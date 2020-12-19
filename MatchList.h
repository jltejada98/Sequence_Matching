//
// Created by Jose Luis Tejada on 14/12/20.
//

#ifndef SEQUENCE_MATCHING_MATCHLIST_H
#define SEQUENCE_MATCHING_MATCHLIST_H

#include <vector>
#include <mutex>
#include "MatchNode.h"

class MatchList {
private:
    size_t  numOccurences;
    std::mutex mutex;
    std::vector<std::shared_ptr<MatchNode>> matchVector;
public:
    MatchList();
    void addMatch(size_t start1, size_t start2);
    void addSubMatch(size_t start1, size_t start2);
    size_t getOccurences() const;
    std::shared_ptr<std::vector<std::shared_ptr<MatchNode>>> getMatchVector();
};


#endif //SEQUENCE_MATCHING_MATCHLIST_H
