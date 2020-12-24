cmake_minimum_required(VERSION 3.17)
project(Sequence_Matching)

set(CMAKE_CXX_STANDARD 11) #Changed from 14 to 11

include_directories(/usr/local/Cellar/tbb/2020_U3_1/include/)

set(CMAKE_CXX_FLAGS "-I/usr/local/Cellar/tbb/2020_U3_1/include/")
set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/Cellar/tbb/2020_U3_1/lib -ltbb")

add_executable(Sequence_Matching
        main.cpp
        FileManipulation.cpp
        FileManipulation.h
        SequenceMatching.cpp
        SequenceMatching.h
        MatchLocations.cpp
        MatchLocations.h)