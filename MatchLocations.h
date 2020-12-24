//
// Created by Jose Luis Tejada on 14/12/20.
//

#ifndef SEQUENCE_MATCHING_MATCHLOCATIONS_H
#define SEQUENCE_MATCHING_MATCHLOCATIONS_H

#include <vector>
#include <mutex>
#include <unordered_set>

class MatchLocations {
private:
    std::mutex mutex;
    std::unordered_set<size_t> index1Set;
    std::unordered_set<size_t> index2Set;
    //Todo Compare using ordered set to avoid vector to calculate intersection of sets?
public:
    MatchLocations();
    void addMatchLocation(size_t start1, size_t start2);
    void addSubMatchIndex1(size_t start1);
    void addSubMatchIndex2(size_t start2);
    size_t getNumberMatches() const;
    bool matchExists(size_t start1, size_t start2);
    std::shared_ptr<std::unordered_set<size_t>> getIndex1Set();
    std::shared_ptr<std::unordered_set<size_t>> getIndex2Set();
};


#endif //SEQUENCE_MATCHING_MATCHLOCATIONS_H
