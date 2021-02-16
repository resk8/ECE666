all: quicksort

quicksort: mpi_quicksort.c 
	mpicc -O2 mpi_quicksort.c -o quicksort

quicksort_debug: mpi_quicksort.c 
	mpicc -O2 -DMYDEBUG mpi_quicksort.c -o quicksort_d

run:
	mpiexec -n 8 -f machinefile quicksort 4096

run-linux:
	mpiexec -np 8 -hostfile machinefile quicksort_d 4096

run-osx:
	mpiexec -np 8 quicksort_d 4096

clean: 
	rm quicksort output.*

