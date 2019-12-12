# thunderstormMPI
Running ImageJ plugin ThunderSTORM on cluster using MPI as scheduler.

## Getting start
* Install imageJ
    ```https://github.com/fiji/fiji.git```
* Install thunderStorm
    ```cd Fiji.app/plugins```
    ```wget https://github.com/zitmen/thunderstorm/releases/download/v1.3/Thunder_STORM.jar```
* Compile Requirement
    * C++ 17
* Install thunderstormMPI
    ```git clone https://github.com/engine210/thunderstormMPI.git```
    ```make```

## Ussage
#### Dynamic version
* ```./thunderstormMPI /input/data/directory/ /ouptpu/data/directory/```
* With slurm
    ```srun -N4 -n4 --exclusive ./thunderstormMPI /input/data/directory/ /ouptpu/data/directory/```

## Program Detail


