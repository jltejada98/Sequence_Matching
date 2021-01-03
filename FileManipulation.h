//
// Created by Jose Luis Tejada on 13/12/20.
//

#ifndef SEQUENCE_MATCHING_FILEMANIPULATION_H
#define SEQUENCE_MATCHING_FILEMANIPULATION_H
#include <fstream>
#include <iostream>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_vector.h>
#include "MatchLocations.h"

std::shared_ptr<std::string> Load_Sequence(const char *filename, size_t &seq_size);

bool Write_Matches(tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>>> &matchesMap,
                   tbb::concurrent_vector<float> &similarityMetricVector,size_t &numSequences, const char *argv[],
                   const std::string& outFilename);

#endif //SEQUENCE_MATCHING_FILEMANIPULATION_H
