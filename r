#!/bin/bash
#execute program with srun
srun -p sc -n$1 ./thunderstormMPI $2 $3
