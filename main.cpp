#include <iostream>
#include <memory>
#include "MatchLocations.h"
#include "FileManipulation.h"
#include "SequenceMatching.h"

int main(int argc, const char *argv[]) {
    if (argc != 4){
        std::cout << "Incorrect Arguments \n" <<std::endl;
        return EXIT_FAILURE;
    }

    char * seq1Filename = (char *) argv[1];
    char * seq2Filename = (char *) argv[2];
    size_t minimumMatchSize = (std::stoi(argv[3]));
    //Todo Check for conversion errors
    size_t seq1Size;
    size_t seq2Size;


    std::shared_ptr<std::string> seq1String = Load_Sequence(seq1Filename, seq1Size);
    if (seq1String == nullptr){
        return EXIT_FAILURE;
    }

    std::shared_ptr<std::string> seq2String  = Load_Sequence(seq2Filename, seq2Size);
    if (seq2String == nullptr){
        return EXIT_FAILURE;
    }

    if (*seq1String != *seq2String){
        std::cout << "Determining Matches..." << std::endl;
        std::shared_ptr<tbb::concurrent_unordered_map<std::string, MatchLocations>> matchesMapPtr =
                Determine_Matches(seq1String, seq1Size, seq2String, seq2Size, minimumMatchSize);

//        std::cout << "Writing Matches..." << std::endl;
//        if (!Write_Matches(*matchesMapPtr, "Matches.txt")){
//            return EXIT_FAILURE;
//        }

        std::cout << "Determining Metrics..." << std::endl;
        float seq1Metric,seq2Metric, combinedMetric;
        Determine_Similarity(*matchesMapPtr, minimumMatchSize,seq1Size, seq2Size, seq1Metric, seq2Metric, combinedMetric);
        std::cout << "Similarity Metrics: " << std::endl;
        std::cout << "Combined:" << combinedMetric << std::endl;
        std::cout << "Seq 1:" << seq1Metric << std::endl;
        std::cout << "Seq 2:" << seq2Metric << std::endl;


        std::cout << "Determining Submatches..." << std::endl;
        Determine_Submatching(*matchesMapPtr, minimumMatchSize);

        std::cout << "Writing Matches..." << std::endl;
        if (!Write_Matches(*matchesMapPtr, "All_Matches.txt")){
            return EXIT_FAILURE;
        }

    }
    else{
        std::cout << "Identical strings" << std::endl;
    }


    return EXIT_SUCCESS;
}
