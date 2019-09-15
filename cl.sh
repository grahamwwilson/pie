#!/bin/sh
#
# Unix shell script based method
#
g++ -O1 -fopenmp -o pie pie.cpp

g++ -O1 -o pie_Sequential pie.cpp

exit
