//
// Created by Jose Luis Tejada on 13/12/20.
//

#ifndef SEQUENCE_MATCHING_SEQUENCEMATCHING_H
#define SEQUENCE_MATCHING_SEQUENCEMATCHING_H
#include <unordered_map>
#include <vector>
#include <regex>
#include "unordered_set"
#include <tbb/task_group.h>
#include <tbb/concurrent_unordered_set.h>
#include "MatchLocations.h"

std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchLocations>>>
Determine_Matches(const std::shared_ptr<std::string> &seq1String, const size_t &seq1Size,
                  const std::shared_ptr<std::string> &seq2String, const size_t &seq2Size, const size_t &minLength);

std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchLocations>>>
Determine_Submatching(const std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchLocations>>>& matchesMap,
                      const size_t &minLength);

void
Submatches_Thread(const std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<MatchLocations>>> &matchesMap,
                  const std::string &keyToCheck, const size_t &minLength);

std::shared_ptr<std::vector<std::shared_ptr<std::string>>>
Determine_Partitions(const std::string &key, const size_t &keyLen, const size_t &minLength,
                     std::shared_ptr<std::vector<std::shared_ptr<size_t>>> &partitionsShiftList);

void Determine_Similarity(const std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchLocations>>>&matchesMap,
                          const size_t &minLength,const size_t &seq1Size, const size_t &seq2Size,float &seq1Metric,
                          float &seq2Metric, float &combinedMetric);

void Similarity_Thread(const size_t &matchKeyLength, const std::shared_ptr<MatchLocations> &matchLocationsSet,
                       std::shared_ptr<tbb::concurrent_unordered_set<size_t>> &seq1Set,
                       std::shared_ptr<tbb::concurrent_unordered_set<size_t>> &seq2Set);

#endif //SEQUENCE_MATCHING_SEQUENCEMATCHING_H
