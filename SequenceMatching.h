//
// Created by Jose Luis Tejada on 13/12/20.
//

#ifndef SEQUENCE_MATCHING_SEQUENCEMATCHING_H
#define SEQUENCE_MATCHING_SEQUENCEMATCHING_H
#include <unordered_map>
#include <vector>
#include <thread>
#include <regex>
#include "MatchNode.h"
#include "MatchList.h"
#include "ctpl.h"

std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchList>>>
Determine_Matches(const std::shared_ptr<std::string> &seq1String, const size_t &seq1Size,
                  const std::shared_ptr<std::string> &seq2String, const size_t &seq2Size, const size_t &minLength);

std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchList>>>
Determine_Submatching(const std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchList>>>& matchesMap,
                      const size_t &minLength);

void Submatches_Thread(int threadID, const std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchList>>>& matchesMap,
                       const std::string& key, const size_t &minLength);

std::shared_ptr<std::vector<std::shared_ptr<std::string>>>
Determine_Partitions(const std::string &key, const size_t &keyLen, const size_t &minLength,
                     std::shared_ptr<std::vector<std::shared_ptr<size_t>>> &partitionsShiftList);



#endif //SEQUENCE_MATCHING_SEQUENCEMATCHING_H
