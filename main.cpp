#include <memory>
#include <iostream>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_unordered_set.h>
#include <unordered_set>
#include "MatchLocations.h"
#include "FileManipulation.h"
#include "SequenceMatching.h"

int main(int argc, const char *argv[]) {
    if (argc < 4){
        std::cout << "Incorrect Arguments \n" <<std::endl;
        std::cout << "./Sequence_Matching <seq1.txt> <seq2.txt> ... <seqN.txt> <min match length>" << std::endl;
        return EXIT_FAILURE;
    }

    //Read all sequences provided.
    size_t index;
    size_t minimumMatchSize = (std::stoi(argv[argc-1]));
    size_t numSequences = argc-2;
    size_t seqSizeArray [numSequences];
    std::shared_ptr<std::string> seqStringArray[numSequences];
    for (index = 0; index < numSequences; ++index) {
        seqStringArray[index] = Load_Sequence((char *) argv[index + 1], seqSizeArray[index]);
        if (seqStringArray[index] == nullptr){
            return EXIT_FAILURE;
        }
    }

    size_t numSeqPairs = numSequences / 2;
    size_t remainderSeqPairs = numSequences % 2;
    size_t matchesMapArraySize = numSeqPairs + remainderSeqPairs;
    //Array holding all matches of all pairs of sequences
    std::shared_ptr<tbb::concurrent_unordered_map<std::string, MatchLocations>> matchesMapArray [matchesMapArraySize];
    size_t smallestMapSize= std::numeric_limits<size_t>::max();
    size_t smallestMapIndex;
    //Todo Concurrently determine matches for all sequence pairs. Create a thread to handle each pair.
    for (index = 0; index <= numSeqPairs; index+=2) {
        if (*seqStringArray[index] != *seqStringArray[index + 1]) { //If strings are not identical.
           //Determine longest sequence, pass it as the first sequence
           std::cout << "Determining Matches between " << (char *) argv[index + 1] << " and " << (char *) argv[index + 2] << "..." << std::endl;
           if(seqSizeArray[index] > seqSizeArray[index + 1]){
               matchesMapArray[index/2] = Determine_Matches(seqStringArray[index], seqSizeArray[index], seqStringArray[index + 1], seqSizeArray[index + 1], minimumMatchSize);
           }
           else{
               matchesMapArray[index/2] = Determine_Matches(seqStringArray[index + 1], seqSizeArray[index + 1], seqStringArray[index], seqSizeArray[index], minimumMatchSize);
           }
           //Determine similarity metrics for each pair, for more than two sequences is not meaningful, it will only be
           // determined on the combined map of all sequences
           Determine_Submatching(*matchesMapArray[index/2], minimumMatchSize);
           //Determine if this is the smallest map
            if (matchesMapArray[index/2]->size() < smallestMapSize){
                smallestMapSize = matchesMapArray[index/2]->size();
                smallestMapIndex = index/2;
            }
        }
        else{
            std::cout << "Files : " << (char *) argv[index + 1] << " ," << (char *) argv[index + 2] << "contain identical strings. (Not valid)" << std::endl;
            return EXIT_FAILURE;
        }
    }

    //Determine matches for remaining pair
    if (remainderSeqPairs == 1){
        if (*seqStringArray[numSequences-2] != *seqStringArray[numSequences-1]) { //If strings are not identical.
            //Determine longest sequence, pass it as the first sequence
            std::cout << "Determining Matches between " << (char *) argv[numSequences-1] << " and " << (char *) argv[numSequences] << " ..." << std::endl;
            if(seqSizeArray[numSequences-2] > seqSizeArray[numSequences-1]){
                matchesMapArray[matchesMapArraySize-1] = Determine_Matches(seqStringArray[numSequences-2], seqSizeArray[numSequences-2], seqStringArray[numSequences-1], seqSizeArray[numSequences-1], minimumMatchSize);
            }
            else{
                matchesMapArray[matchesMapArraySize-1] = Determine_Matches(seqStringArray[numSequences-1], seqSizeArray[numSequences-1], seqStringArray[numSequences-2], seqSizeArray[numSequences-2], minimumMatchSize);
            }
            //Determine similarity metrics for each pair, for more than two sequences is not meaningful, it will only be
            // determined on the combined map of all sequences
            Determine_Submatching(*matchesMapArray[matchesMapArraySize-1], minimumMatchSize);
            //Determine if this is the smallest map
            if (matchesMapArray[matchesMapArraySize-1]->size() < smallestMapSize){
                smallestMapIndex = matchesMapArraySize-1;
            }
        }
        else{
            std::cout << "Files : " << (char *) argv[numSequences-1] << " ," << (char *) argv[numSequences] << "contain identical strings. (Not valid)" << std::endl;
            return EXIT_FAILURE;
        }
    }

    //Compare all maps and determine which keys exist in all of them, iterate on shortest map.
    //Map holding vector of all indices combined from the matches found previously.
    tbb::concurrent_unordered_map<std::string, std::vector<tbb::concurrent_unordered_set<size_t>>> combinedMatchesMap;
    std::shared_ptr<std::vector<tbb::concurrent_unordered_set<size_t>>> combinedMatchIndices;
    std::shared_ptr<std::string> matchString;
    std::pair<std::string, std::vector<tbb::concurrent_unordered_set<size_t>>> newPair;

    if (numSequences > 2){ // Todo check cases of number of sequences
        for (auto &key : *matchesMapArray[smallestMapIndex]){
            //Check if key exists in all other maps.
            for (index = 0; index < matchesMapArraySize; ++index) {
                if ((index != smallestMapIndex) && (!matchesMapArray[index]->count(key.first))){ //count returns 1 if exists, 0 otherwise
                    break;
                }
            }
            if (index >= matchesMapArraySize){ //Checked all maps key exists in all, add to combined map.
                matchString = std::make_shared<std::string>(key.first);
                combinedMatchIndices = std::make_shared<std::vector<tbb::concurrent_unordered_set<size_t>>>();
                for (index = 0; index < numSeqPairs; ++index) { //Add sets to vector of sequence
                    combinedMatchIndices->push_back(matchesMapArray[index / 2]->at(key.first).getIndex1Set());
                    combinedMatchIndices->push_back(matchesMapArray[index / 2]->at(key.first).getIndex2Set());
                }
                if (remainderSeqPairs == 1){ //Add remaining pair
                    combinedMatchIndices->push_back(matchesMapArray[matchesMapArraySize-1]->at(key.first).getIndex2Set());
                }
                newPair = std::make_pair(*matchString, *combinedMatchIndices);
                combinedMatchesMap.insert(newPair);
            }
        }
    }
    else{ //Run similarity on single map from only pair.

    }


    for (auto &k: combinedMatchesMap){
        std::cout << k.first << std::endl;
        for (auto &y: k.second){
            for (auto &u :y){
                std::cout << u << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    //Compute similarity metrics on all sequences.









//    if (*seq1String != *seq2String){
//        std::cout << "Determining Matches..." << std::endl;
//
//        std::shared_ptr<tbb::concurrent_unordered_map<std::string, MatchLocations>> matchesMapPtr;
//
//        if (seq1Size >= seq2Size){
//             matchesMapPtr=Determine_Matches(seq1String, seq1Size, seq2String, seq2Size, minimumMatchSize);
//        }
//        else{
//            matchesMapPtr=Determine_Matches(seq2String, seq2Size, seq1String, seq1Size, minimumMatchSize);
//        }
//
//        // Todo Consider doing matching between a set of sequences, do matches map for non-overlapping pairs
//        // Then iterate on shortest map and determine if keys exist on all other ones, adding to new map if
//        // they exist on all other ones.
//
//        std::cout << "Determining Metrics..." << std::endl;
//        float seq1Metric,seq2Metric, combinedMetric;
//        Determine_Similarity(*matchesMapPtr, minimumMatchSize,seq1Size, seq2Size, seq1Metric, seq2Metric, combinedMetric);
//        std::cout << "Similarity Metrics: " << std::endl;
//        std::cout << "Combined:" << combinedMetric << std::endl;
//        std::cout << "Seq 1:" << seq1Metric << std::endl;
//        std::cout << "Seq 2:" << seq2Metric << std::endl;
//
//
//        std::cout << "Determining Submatches..." << std::endl;
//        Determine_Submatching(*matchesMapPtr, minimumMatchSize);
//
//        std::cout << "Writing Complete Matches..." << std::endl;
//        if (!Write_Matches(*matchesMapPtr, "All_Matches.txt")){
//            return EXIT_FAILURE;
//        }
//
//    }
//    else{
//        std::cout << "Identical strings" << std::endl;
//    }


    return EXIT_SUCCESS;
}
