//
// Created by Jose Luis Tejada on 13/12/20.
//


#include "FileManipulation.h"

std::shared_ptr<std::string> Load_Sequence(const char *filename, size_t &seq_size){
    std::ifstream File;
    std::string sequenceString;
    File.exceptions(::std::ios_base::failbit | ::std::ios_base::badbit);
    try{
        File.open(filename);
        File.seekg(0, std::ios::end);
        sequenceString.reserve(File.tellg());
        File.seekg(0, std::ios::beg);
        sequenceString.assign((std::istreambuf_iterator<char>(File)), std::istreambuf_iterator<char>());
        seq_size = sequenceString.length();
        File.close();
    }
    catch (const std::ifstream::failure &e){
        std::cerr << "Exception" << e.what() << e.code() <<"opening/reading/closing file" <<std::endl;
        return nullptr;
    }
    catch (const std::exception &e) {
        std::cerr << "Exception" << e.what() <<std::endl;
        return nullptr;
    }

    return std::make_shared<std::string>(sequenceString);
}

bool Write_Matches(tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>>> &matchesMap,
                   tbb::concurrent_vector<float> &similarityMetricVector,size_t &numSequences, const char *argv[],
                   const std::string& outFilename){
    std::ofstream File;
    File.exceptions(~::std::ios_base::goodbit);
    try{
        File.open(outFilename); //Open for writing

        //Write Similarity Metrics
        File << "Similarity Metrics:" << std::endl;
        for (size_t i = 0; i < numSequences; ++i) {
            File << (char *) argv[i+1] << " : " << similarityMetricVector[i] << std::endl;
        }
        File << "Overall : " << similarityMetricVector[numSequences] << std::endl;

        //Write Matches
        size_t seqIndex = 0;
        for (auto &key : matchesMap){
            File << key.first << std::endl;
            seqIndex = 0;
            for (auto &set : key.second){
                File << (char *) argv[seqIndex+1] << ":";
                for(auto &index : set){
                    File << index << ",";
                }
                File << std::endl;
                ++seqIndex;
            }
            File << std::endl;
        }

        File.close();
    }
    catch (const std::ifstream::failure &e){
        std::cerr << "Exception" << e.what() << e.code() <<" opening/reading/closing file" <<std::endl;
        return false;
    }
    catch (const std::exception &e) {
        std::cerr << "Exception" << e.what() <<std::endl;
        return false;
    }

    return true;
}


