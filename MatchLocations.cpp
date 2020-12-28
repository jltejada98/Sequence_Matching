//
// Created by Jose Luis Tejada on 14/12/20.
//

#include "MatchLocations.h"

MatchLocations::MatchLocations(){

}

void MatchLocations::addMatchLocation(size_t &start1, size_t &start2) {
    index1Set.insert(start1);
    index2Set.insert(start2);
}

void MatchLocations::addMatchLocationThreadSafe(size_t &start1, size_t &start2) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    index1Set.insert(start1);
    index2Set.insert(start2);
}

void MatchLocations::addSubMatchIndex1(unsigned long start1) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    index1Set.insert(start1);
}

void MatchLocations::addSubMatchIndex2(unsigned long start2) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    index2Set.insert(start2);

}

size_t MatchLocations::getNumberMatches() const {
    return index1Set.size()*index2Set.size();
}

std::shared_ptr<std::unordered_set<size_t>> MatchLocations::getIndex1Set() {
    return std::make_shared<std::unordered_set<size_t>>(index1Set);
}

std::shared_ptr<std::unordered_set<size_t>> MatchLocations::getIndex2Set() {
    return std::make_shared<std::unordered_set<size_t>>(index2Set);
}




