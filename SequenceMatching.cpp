//
// Created by Jose Luis Tejada on 13/12/20.
//

#include "SequenceMatching.h"

//Todo use emplace when inserting objects into map.

std::shared_ptr<tbb::concurrent_unordered_map<std::string, MatchLocations>>
Determine_Matches(const std::shared_ptr<std::string> &seq1String, const size_t &seq1Size,
                  const std::shared_ptr<std::string> &seq2String, const size_t &seq2Size,
                  const size_t &minLength){
    size_t numSplits = std::thread::hardware_concurrency();
    size_t splitSize = seq1Size / numSplits;
    size_t splitRemainder = seq1Size % numSplits; //Determine remainder, and add to first task.
    size_t startIndex = 0;
    size_t endIndex = startIndex+splitSize + splitRemainder;
    std::thread threadArray[numSplits];
    tbb::concurrent_unordered_map<std::string, MatchLocations> matchesMap;
    auto matchesMapRef = std::ref(matchesMap);

    // Create First thread
    threadArray[0] = std::thread(Determine_Matches_Thread,matchesMapRef, seq1String, seq1Size, seq2String, seq2Size, minLength, startIndex, endIndex);
        // Determine_Matches_Thread(std::ref(matchesMap), seq1String, seq1Size, seq2String, seq2Size, minLength, startIndex, endIndex); //Single Thread Version

    //Create subsequent threads
    for (size_t i = 1; i < numSplits; ++i) {
        startIndex = endIndex;
        endIndex = startIndex+splitSize;
        threadArray[i] = std::thread(Determine_Matches_Thread,matchesMapRef, seq1String, seq1Size, seq2String, seq2Size, minLength, startIndex, endIndex);
        // Determine_Matches_Thread(std::ref(matchesMap), seq1String, seq1Size, seq2String, seq2Size, minLength, startIndex, endIndex); //Single Thread Version
    }

    //Wait for all threads to complete
    //Todo Consider adding std::this_thread::yield(); Either in for loop or outside
    for (size_t i=0; i<numSplits; i++){
        threadArray[i].join();
    }

    return std::make_shared<tbb::concurrent_unordered_map<std::string, MatchLocations>>(matchesMap);
}


void Determine_Matches_Thread(tbb::concurrent_unordered_map<std::string, MatchLocations> &matchesMap,
                              const std::shared_ptr<std::string> &seq1String, const size_t &seq1Size,
                              const std::shared_ptr<std::string> &seq2String, const size_t &seq2Size, const size_t &minLength,
                              const size_t &threadStart, const size_t &threadEnd){
    size_t curr1;
    size_t curr2;
    size_t index1 = threadStart;
    size_t index2;
    size_t matchLength;
    std::shared_ptr<std::string> matchString;
    std::shared_ptr<MatchLocations> newMatchLocation;
    std::pair<std::string, MatchLocations> newMatchPair;

    while (index1 < threadEnd){ //Iterate first sequence until end of split
        index2 = 0;
        while (index2 < seq2Size){ //Iterate entirety of second sequence
            curr1 = index1;
            curr2 = index2;
            //If a match starts in split and extends beyond it (So long as not end of sequence) Still add match in for
            // this split (Since it started in it)
            while ((curr2 < seq2Size) && (curr1 < seq1Size) && (seq1String->at(curr1) == seq2String->at(curr2))){
                ++curr1;
                ++curr2;
            }
            matchLength = curr2 - index2;
            if (matchLength >= minLength){
                matchString = std::make_shared<std::string>(std::string(seq1String->substr(index1,matchLength)));
                if (matchesMap.count(*matchString)){ //Returns 1 if found, 0 if not found.
                    matchesMap.at(*matchString).addMatchLocation(index1, index2);
                }
                else{ //Match not found, add key and indices
                    newMatchLocation = std::make_shared<MatchLocations>();
                    newMatchLocation->addMatchLocation(index1, index2);
                    newMatchPair = std::make_pair(*matchString, *newMatchLocation);
                    matchesMap.insert(newMatchPair);
                }
            }
            ++index2;
        }
        ++index1;
    }
}


void Determine_Submatching(tbb::concurrent_unordered_map<std::string, MatchLocations> &matchesMap,
                           const size_t &minLength){

    tbb::task_group submatchesTaskGroup;

    for (auto &x: matchesMap){
        if (x.first.length() > minLength){ //Sequence is partitionable/submatches may be contained in sequence
            // Thread pool to manage execution of threads. Such that each thread in pool is assigned a set of strings to check.
            submatchesTaskGroup.run([&]{Submatches_Thread(matchesMap, x.first, minLength);});
//            Submatches_Thread(matchesMap, x.first, minLength); //Single Thread Version.
        }
    }
    submatchesTaskGroup.wait(); //Wait for all tasks to complete.

}


void Submatches_Thread(tbb::concurrent_unordered_map<std::string, MatchLocations> &matchesMap,
                       const std::string &keyToCheck, const size_t &minLength) {
    size_t keyLength = keyToCheck.length();
    size_t numPartitions = ((keyLength-minLength)*(keyLength - minLength + 3))/2; //Closed form for number of partitions

    if (numPartitions <= matchesMap.size()){ //Check all partitions against hashtable to determine if partitition exists.
        std::vector<std::shared_ptr<size_t>> partitionShiftList;
        std::shared_ptr<std::vector<std::shared_ptr<std::string>>> partitions;
        std::shared_ptr<std::vector<std::shared_ptr<size_t>>> partitionsShiftList = std::make_shared<std::vector<std::shared_ptr<size_t>>>(partitionShiftList);
        partitions = Determine_Partitions(keyToCheck, keyLength, minLength, partitionsShiftList);

        for (size_t i = 0; i < partitions->size(); ++i) { //Iterate all partitions.
            if (matchesMap.count(*partitions->at(i))){ //Partition exists in map (Returns 1 if found, 0 if not found.)
                // Add non existing match locations of keyToCheck into found, with apropiate partitionShiftList
                size_t partitionShift = *partitionsShiftList->at(i);
                auto index1Set = matchesMap.at(keyToCheck).getIndex1Set();
                auto index2Set = matchesMap.at(keyToCheck).getIndex2Set();

                //No need to check if element exists since insertion guarantees unique elements.
                for(const auto& elem1: index1Set){
                    matchesMap.at(*partitions->at(i)).addSubMatchIndex1(elem1 + partitionShift);
                }
                for(const auto& elem2: index2Set){
                    matchesMap.at(*partitions->at(i)).addSubMatchIndex2(elem2 + partitionShift);
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

        for(auto &key: matchesMap){ //Iterate all keys
            if ((key.first != keyToCheck) && (keyToCheck.length() > key.first.length()) && (keyToCheck.find(key.first) != std::string::npos)){ //Determines if key is contained in keyToCheck
                regexKey = std::regex(std::string(key.first));
                foundIterator = std::sregex_iterator(keyToCheck.begin(), keyToCheck.end(), regexKey);

                for(std::sregex_iterator i = foundIterator; i != end; i++){ //For each instance of substring found.
                    std::smatch subtringMatch = *i;
                    foundAtIndex = i->position();

                    auto index1Set = matchesMap.at(keyToCheck).getIndex1Set();
                    auto index2Set = matchesMap.at(keyToCheck).getIndex2Set();

                    //No need to check if element exists since insertion guarantees unique elements.
                    for(const auto& elem1: index1Set){
                        key.second.addSubMatchIndex1(elem1 + foundAtIndex);
                    }
                    for(const auto& elem2: index2Set){
                        key.second.addSubMatchIndex2(elem2 + foundAtIndex);
                    }
                }
            }
        }
    }

}

std::shared_ptr<std::vector<std::shared_ptr<std::string>>>
Determine_Partitions(const std::string &key, const size_t &keyLen, const size_t &minLength,
                     std::shared_ptr<std::vector<std::shared_ptr<size_t>>> &partitionsShiftList){

    std::vector<std::shared_ptr<std::string>> partitionsStringList;

    for(size_t p = keyLen-1; p >= minLength; --p){
        for (size_t i = 0; (i+p) <= keyLen ; ++i){
            partitionsShiftList->push_back(std::make_shared<size_t>(i));
            partitionsStringList.push_back(std::make_shared<std::string>(std::string(key.substr(i, p))));
        }
    }

    return std::make_shared<std::vector<std::shared_ptr<std::string>>>(partitionsStringList);
}
