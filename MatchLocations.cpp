//
// Created by Jose Luis Tejada on 14/12/20.
//

#include "MatchLocations.h"

MatchLocations::MatchLocations(): numMatches(0) {

}

//addMatchLocation: Guarateed that a match isn't duplicated since Determine_Matches is sequential->Never repeats index

void MatchLocations::addMatchLocation(size_t start1, size_t start2) {
    ++numMatches;
    std::shared_ptr<std::string> newKey = std::make_shared<std::string>(std::to_string(start1) + std::to_string(start2));
    std::shared_ptr<MatchNode> newMatch = std::make_shared<MatchNode>(MatchNode(start1, start2));
    std::pair<std::string,std::shared_ptr<MatchNode>> newMatchPair = std::make_pair(*newKey, newMatch);
    matchMap.insert(newMatchPair);
}

//Must check if match already exists before inserting, since submatching is not sequential.
void MatchLocations::addSubMatchLocation(size_t start1, size_t start2) { //Due to submatching, must check if match already exists.
    std::lock_guard<std::mutex> lockGuard(mutex);
    ++numMatches;
    std::shared_ptr<std::string> newKey = std::make_shared<std::string>(std::to_string(start1) + std::to_string(start2));
    std::shared_ptr<MatchNode> newMatch = std::make_shared<MatchNode>(MatchNode(start1, start2));
    std::pair<std::string,std::shared_ptr<MatchNode>> newMatchPair = std::make_pair(*newKey, newMatch);
    matchMap.insert(newMatchPair);
}

size_t MatchLocations::getNumberMatches() const {
    return numMatches;
}

bool MatchLocations::matchExists(size_t start1, size_t start2) {
    return matchMap.find(std::to_string(start1) + std::to_string(start2)) != matchMap.end();
}

std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<MatchNode>>> MatchLocations::getLocationsMap() {
    return std::make_shared<std::unordered_map<std::string, std::shared_ptr<MatchNode>>>(matchMap);
}