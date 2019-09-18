#
# Makefile for pie.cpp
# (this needs an actual <tab> in the command line to work)
#
# Specify the compiler: g++ for C++
CXX = g++

# C++ compiler flags
# -Wall     turns on most, but not all warnings
# -O1       Optimization level 1
# -fopenmp  Compile with OpenMP for shared memory parallel  
CXXFLAGS = -O1 -fopenmp

# the build target executable
TARGET = pie

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(TARGET).cpp

clean:
	$(RM) $(TARGET)
