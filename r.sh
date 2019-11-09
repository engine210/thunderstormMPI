#!/bin/bash
#execute program with srun
srun -p sc -N4 -n10 --exclusive ./thunderstormMPI ~/brc/data/input/ ~/brc/data/output/1_trans/
