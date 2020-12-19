cmake_minimum_required(VERSION 3.17)
project(Sequence_Matching)

set(CMAKE_CXX_STANDARD 14)

add_executable(Sequence_Matching
        main.cpp
        MatchNode.cpp
        MatchNode.h
        FileManipulation.cpp
        FileManipulation.h
        SequenceMatching.cpp
        SequenceMatching.h
        MatchLocations.cpp
        MatchLocations.h
        ctpl.h)