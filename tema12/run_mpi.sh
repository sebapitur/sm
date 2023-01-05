#!/bin/bash
module load mpi/openmpi-x86_64

if (($# != 1)); then
    echo 'usage: ./run_mpi.sh <PROCS>'
else
    echo "PROCS" $1
    time mpirun -np $1 --oversubscribe ./prim_mpi input14.txt
fi