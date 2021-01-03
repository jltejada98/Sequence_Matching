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

void MatchLocations::addSubMatchIndex1(unsigned long start1) {
    index1Set.insert(start1);
}

void MatchLocations::addSubMatchIndex2(unsigned long start2) {
    index2Set.insert(start2);
}

tbb::concurrent_unordered_set<size_t> & MatchLocations::getIndex1Set() const {
    return (tbb::concurrent_unordered_set<size_t> &) index1Set;
}

tbb::concurrent_unordered_set<size_t> & MatchLocations::getIndex2Set() const {
    return (tbb::concurrent_unordered_set<size_t> &) index2Set;
}
