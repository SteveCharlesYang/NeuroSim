.SECONDEXPANSION:

MAINS := main.cpp
ALLSRC := $(wildcard *.cpp NeuroSim/*.cpp)
SRC := $(filter-out $(MAINS),$(ALLSRC))
ALLOBJ := $(ALLSRC:.cpp=.o)
OBJ := $(SRC:.cpp=.o)

CXX := g++
CXXFLAGS := -fopenmp -O3 -std=c++0x -w

.PHONY: all clean
all: $(MAINS:.cpp=)
$(MAINS:.cpp=): $(OBJ) $$@.o
	$(CXX) $(CXXFLAGS) $^ -o $@
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

depend: .depend
.depend: $(ALLSRC)
	@$(RM) .depend
	@g++ -MM $(CXXFLAGS) $^ > .depend;
include .depend

clean:
	$(RM) $(MAINS:.cpp=)
	$(RM) $(ALLOBJ)

# Run simulation
NOW := $(shell date +"%Y%m%d_%H%M%S")
run:
	stdbuf -o 0 ./$(MAINS:.cpp=) | tee log_$(NOW).txt

