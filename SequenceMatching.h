//
// Created by Jose Luis Tejada on 13/12/20.
//

#ifndef SEQUENCE_MATCHING_SEQUENCEMATCHING_H
#define SEQUENCE_MATCHING_SEQUENCEMATCHING_H
#include <unordered_map>
#include <vector>
#include <regex>
#include <thread>
#include <tbb/task_group.h>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_unordered_set.h>
#include "MatchLocations.h"

std::shared_ptr<tbb::concurrent_unordered_map<std::string, MatchLocations>>
Determine_Matches(const std::shared_ptr<std::string> &seq1String, const size_t &seq1Size,
                  const std::shared_ptr<std::string> &seq2String, const size_t &seq2Size,
                  const size_t &minLength);


void Determine_Matches_Thread(tbb::concurrent_unordered_map<std::string, MatchLocations> &matchesMap,
                              const std::shared_ptr<std::string> &seq1String, const size_t &seq1Size,
                              const std::shared_ptr<std::string> &seq2String, const size_t &seq2Size, const size_t &minLength,
                              const size_t &threadStart, const size_t &threadEnd);

void Determine_Similarity(tbb::concurrent_unordered_map<std::string, MatchLocations> &matchesMap,
                          const size_t &minLength,const size_t &seq1Size, const size_t &seq2Size,float &seq1Metric,
                          float &seq2Metric, float &combinedMetric);


void Similarity_Thread(const size_t &matchKeyLength, MatchLocations &matchLocationsSet,
                       tbb::concurrent_unordered_set<size_t> &seq1Set,
                       tbb::concurrent_unordered_set<size_t> &seq2Set);

void Determine_Submatching(tbb::concurrent_unordered_map<std::string, MatchLocations> &matchesMap,
                           const size_t &minLength);

void Submatches_Thread(tbb::concurrent_unordered_map<std::string, MatchLocations> &matchesMap,
                       const std::string &keyToCheck, const size_t &minLength);

std::shared_ptr<std::vector<std::shared_ptr<std::string>>>
Determine_Partitions(const std::string &key, const size_t &keyLen, const size_t &minLength,
                     std::shared_ptr<std::vector<std::shared_ptr<size_t>>> &partitionsShiftList);

#endif //SEQUENCE_MATCHING_SEQUENCEMATCHING_H
