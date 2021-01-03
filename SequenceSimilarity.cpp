//
// Created by Jose Luis Tejada on 2/1/21.
//

#include "SequenceSimilarity.h"

std::shared_ptr<tbb::concurrent_vector<float>> Determine_Similarity(
        tbb::concurrent_unordered_map<std::string,tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>>> &combinedMatchesMap,
        const size_t *seqSizeArray, size_t &numSequences){

    tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>> seqVectorSet;
    //Insert empty sets for each sequence
    for (size_t i = 0; i < numSequences; ++i) {
        seqVectorSet.push_back(tbb::concurrent_unordered_set<size_t>());
    }
    auto seqVectorSetRef = std::ref(seqVectorSet);
    auto combinedMatchesMapRef = std::ref(combinedMatchesMap);

    tbb::task_group similarityTaskGroup;

    for (auto &x: combinedMatchesMap){
        //For each key add corresponding indices to sets for each sequence, determining overall coverage.
//        Similarity_Thread(x.second, x.first.length(), seqVectorSet, numSequences); //Single Threaded Version
        similarityTaskGroup.run([&]{Similarity_Thread(x.second, x.first.length(), seqVectorSetRef, numSequences);});
    }

    similarityTaskGroup.wait();

    //Compute metric for each set
    tbb::concurrent_vector<float> similarityMetricVector;
    size_t overallMatchesSize = 0;
    size_t overallSequenceSize = 0;
    for (size_t i = 0; i < numSequences; ++i) {
        overallMatchesSize += seqVectorSet[i].size();
        overallSequenceSize += seqSizeArray[i];
        similarityMetricVector.push_back(((float)seqVectorSet[i].size())/((float) seqSizeArray[i]));
    }
    //Add overall metric
    similarityMetricVector.push_back(((float)overallMatchesSize)/((float)overallSequenceSize));

    return std::make_shared<tbb::concurrent_vector<float>>(similarityMetricVector);
}


void Similarity_Thread(tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>> &matchLocations,
                       const size_t &matchKeyLength,
                       tbb::concurrent_vector<tbb::concurrent_unordered_set<size_t>> &seqVectorSet,
                       size_t &numSequences) {
    size_t setIndex;
    size_t index;
    for (setIndex = 0; setIndex < numSequences; ++setIndex){//Iterate through all sequence sets.
        //For each set, iterate through all indices of the match, add to respective set in seqVectorSet
        for (auto &matchIndex: matchLocations[setIndex]){
            for (index = matchIndex; index < (matchIndex + matchKeyLength) ; ++index) {
                seqVectorSet[setIndex].insert(index);
            }
        }
    }
}
