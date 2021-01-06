OBJS = FileManipulation.o MatchLocations.o SequenceMatching.o SequenceSimilarity.o main.o
CC = g++
INCLUDE = -I/usr/local/Cellar/tbb/2020_U3_1/include/
LIBS = -L/usr/local/Cellar/tbb/2020_U3_1/lib -ltbb
CXXFLAGS =-std=c++11 -Wall -O3
EXECUTABLE = Sequence_Matching
COMMANDLINE = Q9I7U4.txt Q8WZ42.txt A2ASS6.txt G4SLH0.txt 3
VALGRIND = valgrind --leak-check=full --verbose --log-file

.PHONY : run
run : $(EXECUTABLE)
	./$(EXECUTABLE) $(COMMANDLINE)

cleanmake : clean $(EXECUTABLE)

.PHONY : $(EXECUTABLE)
$(EXECUTABLE) :  $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) $(CXXFLAGS) $(INCLUDE) $(LIBS)

%.o : %.c
	$(CC) -c $@ $(INCLUDE) $(LIBS)

testmemory:
	$(CC) -o $(EXECUTABLE) $(OBJS) $(CXXFLAGS) -g $(INCLUDE) $(LIBS) #Added -g for debugger flag
	$(VALGRIND)=./valgrind1.log ./$(EXECUTABLE)

clean :
	rm -rf $(EXECUTABLE) ${OBJS} valgrind?.log Results.txt
