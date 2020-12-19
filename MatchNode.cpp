//
// Created by Jose Luis Tejada on 13/12/20.
//

#include "MatchNode.h"
#include <iostream>

MatchNode::MatchNode(size_t start1, size_t start2): startIndex_1(start1), startIndex_2(start2) {

}

std::ostream &operator<<(std::ostream &os, MatchNode &arg) {
    os << "Indecies:" << arg.startIndex_1 << "," << arg.startIndex_2 << std::endl;
    return os;
}

unsigned int MatchNode::getStartIndex1() const {
    return startIndex_1;
}

unsigned int MatchNode::getStartIndex2() const {
    return startIndex_2;
}



