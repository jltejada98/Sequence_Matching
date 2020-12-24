//
// Created by Jose Luis Tejada on 13/12/20.
//

#include "SequenceMatching.h"

std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchLocations>>>
        Determine_Matches(const std::shared_ptr<std::string> &seq1String, const size_t &seq1Size,
                          const std::shared_ptr<std::string> &seq2String, const size_t &seq2Size, const size_t &minLength){

    size_t curr1;
    size_t curr2;
    size_t index1 = 0;
    size_t index2;
    size_t matchLength;
    std::unordered_map<std::string,std::shared_ptr<MatchLocations>> matchesMap; //Stores Sequence,MatchLocations
    std::unordered_map<std::string,std::shared_ptr<MatchLocations>>::const_iterator found;
    std::shared_ptr<std::string> matchString;
    std::shared_ptr<MatchLocations> newMatchLocation; //Stores location of Matches
    std::pair<std::string,std::shared_ptr<MatchLocations>> newMatchPair;


    while (index1 < seq1Size){
        index2 = 0;
        while (index2 < seq2Size){
            curr1 = index1;
            curr2 = index2;
            while ((curr2 < seq2Size) && (curr1 < seq1Size) && (seq1String->at(curr1) == seq2String->at(curr2))){
                ++curr1;
                ++curr2;
            }
            matchLength = curr2 - index2;
            if (matchLength >= minLength){
                matchString = std::make_shared<std::string>(std::string(seq1String->substr(index1,matchLength)));
                found = matchesMap.find(*matchString);
                if (found == matchesMap.end()){ //Match doesn't exist
                    newMatchLocation = std::make_shared<MatchLocations>();
                    newMatchLocation->addMatchLocation(index1, index2);
                    newMatchPair = std::make_pair(*matchString, newMatchLocation);
                    matchesMap.insert(newMatchPair);
                }
                else{ //Match already exists (Assume only one bucket for string?)
                    matchesMap.at(*matchString)->addMatchLocation(index1, index2);
                }
            }
            ++index2;
        }
        ++index1;
    }

    return std::make_shared<std::unordered_map<std::string,std::shared_ptr<MatchLocations>>>(matchesMap);
}


std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchLocations>>>
        Determine_Submatching(const std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchLocations>>>&matchesMap,
                              const size_t &minLength){

    tbb::task_group submatchesTaskGroup;

    for (auto &x: *matchesMap){
        if (x.first.length() > minLength){ //Sequence is partitionable/submatches may be contained in sequence
            // Thread pool to manage execution of threads. Such that each thread in pool is assigned a string to check.
            submatchesTaskGroup.run([&]{Submatches_Thread(matchesMap, x.first, minLength);});
//            Submatches_Thread(matchesMap, x.first, minLength); //Single Thread Version.
        }
    }
    submatchesTaskGroup.wait(); //Wait for all tasks to complete.

    return matchesMap;
}

void Submatches_Thread(const std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<MatchLocations>>> &matchesMap,
                  const std::string &keyToCheck, const size_t &minLength) {
    size_t keyLength = keyToCheck.length();
    size_t numPartitions = ((keyLength-minLength)*(keyLength - minLength + 3))/2; //Closed form for number of partitions

    if (numPartitions <= matchesMap->size()){ //Check all partitions against hashtable to determine if partitition exists.
        std::vector<std::shared_ptr<size_t>> partitionShiftList;
        std::shared_ptr<std::vector<std::shared_ptr<std::string>>> partitions;
        std::unordered_map<std::string,std::shared_ptr<MatchLocations>>::const_iterator found;
        std::shared_ptr<std::vector<std::shared_ptr<size_t>>> partitionsShiftList = std::make_shared<std::vector<std::shared_ptr<size_t>>>(partitionShiftList);

        partitions = Determine_Partitions(keyToCheck, keyLength, minLength, partitionsShiftList);

        for (size_t i = 0; i < partitions->size(); ++i) { //Iterate all partitions.
            found = matchesMap->find(*partitions->at(i)); //Find partition string in matchesMap.

            if (found != matchesMap->end()){ //Partition exists in map
                // Add non existing match locations of keyToCheck into found, with apropiate partitionShiftList
                size_t partitionShift = *partitionsShiftList->at(i);
                auto index1Set = matchesMap->at(keyToCheck)->getIndex1Set();
                auto index2Set = matchesMap->at(keyToCheck)->getIndex2Set();

                //No need to check if element exists since insertion guarantees unique elements.
                for(const auto& elem1: *index1Set){
                    found->second->addSubMatchIndex1(elem1 + partitionShift);
                }
                for(const auto& elem2: *index2Set){
                    found->second->addSubMatchIndex2(elem2 + partitionShift);
                }
            }
        }
    }
    else{ //Check all elements in hashtable against the keyToCheck. (Consider that it is slower due to regex)
        //Todo Determine if regex absolutely necessary -> I think so?
        size_t foundAtIndex;
        std::string matchString;
        std::regex regexKey;
        std::sregex_iterator foundIterator;
        auto end = std::sregex_iterator();

        for(auto &match: *matchesMap){ //Iterate all keys
            if (match.first != keyToCheck){
                regexKey = std::regex(std::string(match.first));
                foundIterator = std::sregex_iterator(keyToCheck.begin(), keyToCheck.end(), regexKey); //Determine if works with character without string.

                for(std::sregex_iterator i = foundIterator; i != end; i++){ //For each instance of substring found.
                    std::smatch subtringMatch = *i;
                    foundAtIndex = i->position();

                    auto index1Set = matchesMap->at(keyToCheck)->getIndex1Set();
                    auto index2Set = matchesMap->at(keyToCheck)->getIndex2Set();

                    //No need to check if element exists since insertion guarantees unique elements.
                    for(const auto& elem1: *index1Set){
                        match.second->addSubMatchIndex1(elem1 + foundAtIndex);
                    }
                    for(const auto& elem2: *index2Set){
                        match.second->addSubMatchIndex2(elem2 + foundAtIndex);
                    }
                }
            }
        }
    }

}

std::shared_ptr<std::vector<std::shared_ptr<std::string>>>
Determine_Partitions(const std::string &key, const size_t &keyLen, const size_t &minLength,
                     std::shared_ptr<std::vector<std::shared_ptr<size_t>>> &partitionsShiftList) {

    std::vector<std::shared_ptr<std::string>> partitionsStringList;

    for(size_t p = keyLen-1; p >= minLength; --p){
        for (size_t i = 0; (i+p) <= keyLen ; ++i){
            partitionsShiftList->push_back(std::make_shared<size_t>(i));
            partitionsStringList.push_back(std::make_shared<std::string>(std::string(key.substr(i, p))));
        }
    }

    return std::make_shared<std::vector<std::shared_ptr<std::string>>>(partitionsStringList);
}

