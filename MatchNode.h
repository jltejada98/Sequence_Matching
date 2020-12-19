//
// Created by Jose Luis Tejada on 13/12/20.
//

#ifndef SEQUENCE_MATCHING_MATCHNODE_H
#define SEQUENCE_MATCHING_MATCHNODE_H
#include <iostream>

class MatchNode {
private:
    size_t startIndex_1;
    size_t startIndex_2;
public:
    MatchNode(size_t, size_t);
    unsigned int getStartIndex1() const;
    unsigned int getStartIndex2() const;
    friend std::ostream& operator<<(std::ostream &os,  MatchNode &arg);
};


#endif //SEQUENCE_MATCHING_MATCHNODE_H
