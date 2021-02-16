all: matmult

matmult: mpi_matrixmul.c 
	mpicc -O2 mpi_matrixmul.c -o matmult

matmult_debug: mpi_matrixmul.c 
	mpicc -O2 -DMYDEBUG mpi_matrixmul.c -o matmult_d

run:
	mpiexec -n 8 -f machinefile matrixmul 4096

run-linux:
	mpiexec -np 8 -hostfile machinefile matmult_d 4096

run-osx:
	mpiexec -np 8 matmult_d 4096

clean: 
	rm matrixmul C.*
