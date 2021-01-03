//
// Created by Jose Luis Tejada on 14/12/20.
//

#ifndef SEQUENCE_MATCHING_MATCHLOCATIONS_H
#define SEQUENCE_MATCHING_MATCHLOCATIONS_H

#include <tbb/concurrent_unordered_set.h>

class MatchLocations {
private:
    tbb::concurrent_unordered_set<size_t> index1Set;
    tbb::concurrent_unordered_set<size_t> index2Set;
public:
    MatchLocations();
    void addMatchLocation(size_t &start1, size_t &start2);
    void addSubMatchIndex1(unsigned long start1);
    void addSubMatchIndex2(unsigned long start2);
    tbb::concurrent_unordered_set<size_t> & getIndex1Set() const;
    tbb::concurrent_unordered_set<size_t> & getIndex2Set() const;
};


#endif //SEQUENCE_MATCHING_MATCHLOCATIONS_H
