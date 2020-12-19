//
// Created by Jose Luis Tejada on 13/12/20.
//

#include "SequenceMatching.h"

std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchList>>>
        Determine_Matches(const std::shared_ptr<std::string> &seq1String, const size_t &seq1Size,
                          const std::shared_ptr<std::string> &seq2String, const size_t &seq2Size, const size_t &minLength){

    size_t curr1;
    size_t curr2;
    size_t index1 = 0;
    size_t index2;
    size_t matchLength;
    std::unordered_map<std::string,std::shared_ptr<MatchList>> matchesMap;
    std::unordered_map<std::string,std::shared_ptr<MatchList>>::const_iterator found;
    std::shared_ptr<std::string> matchString;
    std::shared_ptr<MatchList> newMatchList;


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
                    newMatchList = std::make_shared<MatchList>();
                    newMatchList->addMatch(index1, index2);
                    std::pair<std::string,std::shared_ptr<MatchList>> newMatchPair = std::make_pair(*matchString, newMatchList);
                    matchesMap.insert(newMatchPair);
                }
                else{ //Match already exists (Assume only one bucket for string?)
                    matchesMap.at(*matchString)->addMatch(index1, index2);
                }
            }
            matchLength > 0 ? index2 += matchLength: ++index2;
        }
        ++index1;
    }

    return std::make_shared<std::unordered_map<std::string,std::shared_ptr<MatchList>>>(matchesMap);
}


std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchList>>>
        Determine_Submatching(const std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchList>>>&matchesMap,
                              const size_t &minLength){

    // ctpl::thrcead_pool threadPool((int)std::thread::hardware_concurrency());

    for (auto &x: *matchesMap){
        if (x.first.length() > minLength){ //Sequence is partitionable/submatches may be contained in sequence.
            // Thread pool to manage execution of threads. Such that each thread in pool is assigned a string to check.
            // threadPool.push(Submatches_Thread,matchesMap,x.first,minLength);
           Submatches_Thread(0,matchesMap,x.first,minLength); //Single Thread Version.

        }
    }
    //Wait for all threads in pool to complete tasks
    // threadPool.stop(true);

    return matchesMap;
}

void Submatches_Thread(int threadID, const std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<MatchList>>>& matchesMap,
                       const std::string& key, const size_t &minLength){
    size_t keyLength = key.length();
    size_t numPartitions = ((keyLength-minLength)*(keyLength - minLength + 3))/2; //Closed form for number of partitions

    if (numPartitions <= matchesMap->size()){ //Check all partitions against hashtable to determine if partitition exists.
        std::vector<std::shared_ptr<size_t>> partitionShiftList;
        std::shared_ptr<std::vector<std::shared_ptr<std::string>>> partitions;
        std::unordered_map<std::string,std::shared_ptr<MatchList>>::const_iterator found;
        std::shared_ptr<std::vector<std::shared_ptr<size_t>>> partitionsShiftList = std::make_shared<std::vector<std::shared_ptr<size_t>>>(partitionShiftList);

        partitions = Determine_Partitions(key,keyLength, minLength,partitionsShiftList);

        for (size_t i = 0; i < partitions->size(); ++i) {
            found = matchesMap->find(*partitions->at(i));
            if (found != matchesMap->end()){ //Partition exists in map, add matches of key into found, with appropiate partitionShiftList
                size_t partitionShift = *partitionsShiftList->at(i);
                auto keyMatchesVector = matchesMap->at(key)->getMatchVector();
                for (auto &keyMatches: *keyMatchesVector){ //For all matches of keys, add to found
                    found->second->addSubMatch(keyMatches->getStartIndex1()+partitionShift, keyMatches->getStartIndex2()+partitionShift);
                }

            }
        }
    }
    else{ //Check all elements in hashtable against the key. (Consider that it is slower due tot regex)
        //Todo Determine if regex absolutely necessary -> I think so?
        auto regexKey = std::regex(key);
        auto end = std::sregex_iterator();
        size_t foundAtIndex;
        for(auto &match: *matchesMap){
            //Make function to determine if there are more submatches
            std::sregex_iterator foundIterator (match.first.begin(), match.first.end(), regexKey); //Determine if works with character without string.
            for(std::sregex_iterator i = foundIterator; i != end; i++){ //For each instance of substring found.
                std::smatch subtringMatch = *i;
                foundAtIndex = i->position();
                auto keyMatchesVector = matchesMap->at(key)->getMatchVector();
                for (auto &keyMatches: *keyMatchesVector){ //Add matches of key into found, with appropaite shift
                    match.second->addSubMatch(keyMatches->getStartIndex1() + foundAtIndex, keyMatches->getStartIndex2() + foundAtIndex);
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
