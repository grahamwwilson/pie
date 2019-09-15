#
# Makefile for pie.cpp
# (this needs an actual <tab> in the command line to work)
#
# Specify the compiler: g++ for C++
CXX = g++

# compiler flags
# -O1       Optimization level 1  
CXXFLAGS = -O1

# the build target executable
TARGET = pie_Sequential
SRC = pie

all: $(TARGET)

$(TARGET): $(SRC).cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC).cpp

clean:
	$(RM) $(TARGET)
