//
// Created by Jose Luis Tejada on 14/12/20.
//

#include "MatchList.h"

MatchList::MatchList(): numOccurences(0) {
}

void MatchList::addMatch(size_t start1, size_t start2) {
    ++numOccurences;
    matchVector.push_back(std::make_shared<MatchNode>(MatchNode(start1, start2)));
}

void MatchList::addSubMatch(size_t start1, size_t start2) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    ++numOccurences;
    matchVector.push_back(std::make_shared<MatchNode>(MatchNode(start1, start2)));
}

size_t MatchList::getOccurences() const {
    return numOccurences;
}

std::shared_ptr<std::vector<std::shared_ptr<MatchNode>>> MatchList::getMatchVector() {
    return std::make_shared<std::vector<std::shared_ptr<MatchNode>>>(matchVector);
}
