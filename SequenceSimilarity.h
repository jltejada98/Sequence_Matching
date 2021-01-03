//
// Created by Jose Luis Tejada on 2/1/21.
//

#ifndef SEQUENCE_MATCHING_SEQUENCESIMILARITY_H
#define SEQUENCE_MATCHING_SEQUENCESIMILARITY_H

#include <tbb/task_group.h>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_unordered_set.h>

std::shared_ptr<tbb::concurrent_vector<float>> Determine_Similarity(
        tbb::concurrent_unordered_map<std::string,tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>>> &combinedMatchesMap,
        const size_t *seqSizeArray, size_t &numSequences);

void Similarity_Thread(tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>> &matchLocations,
                       const size_t &matchKeyLength,
                       tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>> &seqVectorSet,
                       size_t &numSequences);

#endif //SEQUENCE_MATCHING_SEQUENCESIMILARITY_H
