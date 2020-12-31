#include <iostream>
#include <memory>
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
    size_t smallestSequenceLength = std::numeric_limits<size_t>::max();
    size_t smallestSequenceIndex;
    std::shared_ptr<std::string> seqStringArray[numSequences];
    for (index = 0; index < numSequences; ++index) {
        seqStringArray[index] = Load_Sequence((char *) argv[index + 1], seqSizeArray[index]);
        if (seqStringArray[index] == nullptr){
            return EXIT_FAILURE;
        }
        if (seqSizeArray[index] < smallestSequenceLength){
            smallestSequenceLength = seqSizeArray[index];
            smallestSequenceIndex = index;
        }
    }

    size_t numSeqPairs = numSequences / 2;
    size_t remainderSeqPairs = numSequences % 2;
    size_t matchesMapArraySize = numSeqPairs + remainderSeqPairs;
    //Array holding all matches of all pairs of sequences
    std::shared_ptr<tbb::concurrent_unordered_map<std::string, MatchLocations>> matchesMapArray [matchesMapArraySize];

    //Todo Concurrently determine matches for all sequence pairs. Create a thread to handle each pair.

    for (index = 0; index <= numSeqPairs; index+=2) {
        if (*seqStringArray[index] != *seqStringArray[index + 1]) { //If strings are not identical.
           //Determine longest sequence, pass it as the first sequence
           std::cout << "Determining Matches between " << *seqStringArray[index] << " and " << *seqStringArray[index+1] << " ..." << std::endl;
           if(seqSizeArray[index] > seqSizeArray[index + 1]){
               matchesMapArray[index] = Determine_Matches(seqStringArray[index], seqSizeArray[index], seqStringArray[index + 1], seqSizeArray[index + 1], minimumMatchSize);
           }
           else{
               matchesMapArray[index] = Determine_Matches(seqStringArray[index + 1], seqSizeArray[index + 1], seqStringArray[index], seqSizeArray[index], minimumMatchSize);
           }
           //Determine similarity metrics for each pair, for more than two sequences is not meaningful, it will only be
           // determined on the combined map of all sequences
            Determine_Submatching(*matchesMapArray[index], minimumMatchSize);
        }
        else{
            std::cout << "Files : " << (char *) argv[index + 1] << " ," << (char *) argv[index + 2] << "contain identical strings. (Incorrect)" << std::endl;
            return EXIT_FAILURE;
        }
    }

    //Determine matches for remaining pair
    if (remainderSeqPairs == 1){
        if (*seqStringArray[index-1] != *seqStringArray[index]) { //If strings are not identical.
            //Determine longest sequence, pass it as the first sequence
            std::cout << "Determining Matches between " << *seqStringArray[index-1] << "and" << *seqStringArray[index] << "..." << std::endl;
            if(seqSizeArray[index-1] > seqSizeArray[index]){
                matchesMapArray[index-1] = Determine_Matches(seqStringArray[index-1], seqSizeArray[index-1], seqStringArray[index], seqSizeArray[index], minimumMatchSize);
            }
            else{
                matchesMapArray[index-1] = Determine_Matches(seqStringArray[index], seqSizeArray[index], seqStringArray[index-1], seqSizeArray[index-1], minimumMatchSize);
            }
            //Determine similarity metrics for each pair, for more than two sequences is not meaningful, it will only be
            // determined on the combined map of all sequences
            Determine_Submatching(*matchesMapArray[index-1], minimumMatchSize);
        }
        else{
            std::cout << "Files : " << (char *) argv[index] << " ," << (char *) argv[index + 1] << "contain identical strings. (Incorrect)" << std::endl;
            return EXIT_FAILURE;
        }
    }

    //Compare all maps and determine which keys exist in all of them, iterate on shortest map.
    //Todo Create unordered map holding std:string, array of unordered sets-> one for each of the sequences.

    if (numSequences > 2){
        for (auto &key : *matchesMapArray[smallestSequenceIndex]){
            //Check if key exists in all other maps.
            for (index = 0; index < matchesMapArraySize; ++index) {
                if ((index != smallestSequenceIndex) && (!matchesMapArray[index]->count(key.first))){ //count returns 1 if exists, 0 otherwise
                    break;
                }
            }
            if (index >= matchesMapArraySize){ //Checked all maps, didn't break

            }
        }
    }











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
