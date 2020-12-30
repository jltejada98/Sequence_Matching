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

void Determine_Matches(const std::shared_ptr<std::string> &seq1String, const size_t &seq1Size,
                       const std::shared_ptr<std::string> &seq2String, const size_t &seq2Size,
                       const size_t &minLength);


void Determine_Matches_Thread(tbb::concurrent_unordered_map<std::string, MatchLocations> &matchesMap,
                              const std::shared_ptr<std::string> &seq1String, const size_t &seq1Size,
                              const std::shared_ptr<std::string> &seq2String, const size_t &seq2Size, const size_t &minLength,
                              const size_t &threadStart, const size_t &threadEnd);

#endif //SEQUENCE_MATCHING_SEQUENCEMATCHING_H
