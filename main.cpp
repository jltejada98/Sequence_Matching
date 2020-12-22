#include <iostream>
#include <unordered_map>
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
        std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchLocations>>> matchesMap =
                Determine_Matches(seq1String, seq1Size, seq2String, seq2Size, minimumMatchSize);

        //Todo Determine Similarity Metric

        std::cout << "Writing Matches..." << std::endl;
        if (!Write_Matches(matchesMap, "Matches_Only.txt")){
            return EXIT_FAILURE;
        }

        std::cout << "Determining Submatches..." << std::endl;
        std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchLocations>>> matches =
                Determine_Submatching(matchesMap, minimumMatchSize);

        std::cout << "Writing Submatches..." << std::endl;
        if (!Write_Matches(matchesMap, "Submatches.txt")){
            return EXIT_FAILURE;
        }

    }
    else{
        std::cout << "Identical strings" << std::endl;
    }


    return EXIT_SUCCESS;
}
