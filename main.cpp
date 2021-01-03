#include <memory>
#include <iostream>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_unordered_set.h>
#include <tbb/concurrent_vector.h>
#include "MatchLocations.h"
#include "FileManipulation.h"
#include "SequenceMatching.h"
#include "SequenceSimilarity.h"

int main(int argc, const char *argv[]) {
    if (argc < 4){
        std::cout << "Incorrect Arguments \n" <<std::endl;
        std::cout << "./Sequence_Matching <seq1.txt> <seq2.txt> ... <seqN.txt> <min match length>" << std::endl;
        return EXIT_FAILURE;
    }

    //Todo Reserve spaces on all dynamic data structures

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
    size_t smallestMapSize = std::numeric_limits<size_t>::max();
    size_t smallestMapIndex;

    //Todo Consider creating threads to check each sequence pair (Being aware of threads spawned and task groups)

    for (index = 0; index <= numSeqPairs; index+=2) {
        if (*seqStringArray[index] != *seqStringArray[index + 1]) { //If strings are not identical.
           //Determine longest sequence, pass it as the first sequence
           std::cout << "Determining Matches between " << (char *) argv[index + 1] << " and " << (char *) argv[index + 2] << "..." << std::endl;
           matchesMapArray[index/2] = Determine_Matches(seqStringArray[index], seqSizeArray[index], seqStringArray[index + 1], seqSizeArray[index + 1], minimumMatchSize);
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
            matchesMapArray[matchesMapArraySize-1] = Determine_Matches(seqStringArray[numSequences-2], seqSizeArray[numSequences-2], seqStringArray[numSequences-1], seqSizeArray[numSequences-1], minimumMatchSize);
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
    tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>>> combinedMatchesMap;
    std::shared_ptr<tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>>> combinedMatchIndices;
    std::shared_ptr<std::string> matchString;
    std::pair<std::string, tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>>> newPair;

    //Todo check if it works on two sequences
    for (auto &key : *matchesMapArray[smallestMapIndex]){
        //Check if key exists in all other maps.
        for (index = 0; index < matchesMapArraySize; ++index) {
            if ((index != smallestMapIndex) && (!matchesMapArray[index]->count(key.first))){ //count returns 1 if exists, 0 otherwise
                break;
            }
        }
        if (index >= matchesMapArraySize){ //Checked all maps key exists in all, add to combined map.
            matchString = std::make_shared<std::string>(key.first);
            combinedMatchIndices = std::make_shared<tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>>>();
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

    //Compute similarity metrics on all sequences.
    std::shared_ptr<tbb::concurrent_vector<float>> similarityMetricVector = Determine_Similarity(combinedMatchesMap, seqSizeArray, numSequences);

    std::cout << "Writing Results..." << std::endl;
    if(!Write_Matches(combinedMatchesMap, *similarityMetricVector, numSequences, argv, "Results.txt")){
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
