#!/bin/bash
mpicxx -cxx=g++ -pthread -O3 -std=c++17 -o thunderstormMPI_dynamic thunderstormMPI_dynamic.cpp
