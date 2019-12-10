#!/bin/bash
srun -p sc -N4 -n10 --exclusive ./thunderstormMPI /home/brc_user/Democase_analysis/data/1/ ~/brc/data/output/test_1209/1/

srun -p sc -N4 -n10 --exclusive ./thunderstormMPI /home/brc_user/Democase_analysis/data/2/ ~/brc/data/output/test_1209/2/

srun -p sc -N4 -n10 --exclusive ./thunderstormMPI /home/brc_user/Democase_analysis/data/3/ ~/brc/data/output/test_1209/3/

srun -p sc -N4 -n10 --exclusive ./thunderstormMPI /home/brc_user/Democase_analysis/data/4/ ~/brc/data/output/test_1209/4/
