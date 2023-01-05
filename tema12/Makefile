all: prim_serial prim_omp prim_pthreads prim_mpi prim_omp_mpi

prim_serial: prim_serial.cpp
	g++ prim_serial.cpp -o prim_serial

prim_mpi: prim_mpi.cpp
	mpic++ prim_mpi.cpp -o prim_mpi

prim_omp: prim_omp.cpp
	g++ prim_omp.cpp -o prim_omp -fopenmp

prim_omp_mpi: prim_omp_mpi.cpp
	mpic++ -fopenmp prim_omp_mpi.cpp -o prim_omp_mpi

prim_pthreads: prim_pthreads.cpp
	g++ prim_pthreads.cpp -o prim_pthreads -lpthread

clean:
	rm -f prim_mpi prim_omp prim_serial prim_pthreads prim_omp_mpi
