#!/bin/bash
#execute program with srun
srun -p sc -N4 -n10 --exclusive ./thunderstormMPI /home/brc_user/Democase_analysis/data/1/ ~/brc/data/output/1/
