//
// Created by Jose Luis Tejada on 14/12/20.
//

#include "MatchList.h"

MatchList::MatchList(): numOccurences(0) {

}

void MatchList::addMatch(size_t start1, size_t start2) {//Guaranteed that a match isn't duplicated
    ++numOccurences;
    std::shared_ptr<std::string> newKey = std::make_shared<std::string>(std::to_string(start1) + std::to_string(start2));
    std::shared_ptr<MatchNode> newMatch = std::make_shared<MatchNode>(MatchNode(start1, start2));
    std::pair<std::string,std::shared_ptr<MatchNode>> newMatchPair = std::make_pair(*newKey, newMatch);
    matchMap.insert(newMatchPair);
}

void MatchList::addSubMatch(size_t start1, size_t start2) { //Due to submatching, must check if match already exists.
    std::lock_guard<std::mutex> lockGuard(mutex);
    ++numOccurences;
    std::shared_ptr<std::string> newKey = std::make_shared<std::string>(std::to_string(start1) + std::to_string(start2));
    std::shared_ptr<MatchNode> newMatch = std::make_shared<MatchNode>(MatchNode(start1, start2));
    std::pair<std::string,std::shared_ptr<MatchNode>> newMatchPair = std::make_pair(*newKey, newMatch);
    matchMap.insert(newMatchPair);
}

size_t MatchList::getOccurences() const {
    return numOccurences;
}


bool MatchList::matchExists(size_t start1, size_t start2) {
    return matchMap.find(std::to_string(start1) + std::to_string(start2)) != matchMap.end();
}

std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<MatchNode>>> MatchList::getMatchMap() {
    return std::make_shared<std::unordered_map<std::string, std::shared_ptr<MatchNode>>>(matchMap);
}
