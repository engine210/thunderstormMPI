#!/bin/bash
#compile program with mpicxx
mpicxx -cxx=clang++ -O3 -std=c++17 -o thunderstormMPI thunderstormMPI.cpp
