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

bool Write_Matches(std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchList>>> matchesMap, std::string outFilename){
    std::ofstream File;
    File.exceptions(~::std::ios_base::goodbit);
    try{
        File.open(outFilename); //Open for writing
        for (auto &x: *matchesMap){
            File << x.first << " len=" << x.first.length() <<  " num=" << x.second->getOccurences() << std::endl;
            auto y = x.second->getMatchVector();
            for (auto &ye: *y){
                File << " " << ye->getStartIndex1() << "," << ye->getStartIndex2() << std::endl;
            }
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


