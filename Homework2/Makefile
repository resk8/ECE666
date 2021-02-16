all: sort sort-debug matmult matmult-debug

sort: mpi_quicksort.c 
	mpicc -O1 mpi_quicksort.c -o quicksort

sort-debug: mpi_quicksort.c 
	mpicc -O1 -DMYDEBUG mpi_quicksort.c -o quicksort_d

run-sort:
	mpiexec -n 8 -f machinefile quicksort 4096

run-sort-debug:
	mpiexec -n 8 -f machinefile quicksort_d 4096

run-sort-linux:
	mpiexec -np 8 -hostfile machinefile quicksort_d 4096

run-sort-osx:
	mpiexec -np 8 quicksort_d 4096

matmult: mpi_matrixmul.c 
	mpicc -O1 mpi_matrixmul.c -o matmult

matmult-debug: mpi_matrixmul.c 
	mpicc -O1 -DMYDEBUG mpi_matrixmul.c -o matmult_d

run-matmult:
	mpiexec -n 8 -f machinefile matmult 4096

run-matmult-debug:
	mpiexec -n 8 -f machinefile matmult_d 4096

run-matmult-linux:
	mpiexec -np 8 -hostfile machinefile matmult_d 4096

run-matmult-osx:
	mpiexec -np 8 matmult_d 4096

clean: 
	rm matmult quicksort matmult_d quicksort_d C.* output.*
