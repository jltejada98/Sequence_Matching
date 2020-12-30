//
// Created by Jose Luis Tejada on 13/12/20.
//

#ifndef SEQUENCE_MATCHING_FILEMANIPULATION_H
#define SEQUENCE_MATCHING_FILEMANIPULATION_H
#include <fstream>
#include <iostream>
#include <tbb/concurrent_unordered_map.h>
#include "MatchLocations.h"

std::shared_ptr<std::string> Load_Sequence(const char *filename, size_t &seq_size);

bool Write_Matches(tbb::concurrent_unordered_map<std::string, MatchLocations> &matchesMap,
                   const std::string& outFilename);

#endif //SEQUENCE_MATCHING_FILEMANIPULATION_H
