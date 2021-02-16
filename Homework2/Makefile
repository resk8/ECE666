all: sort sort-debug sort-debug-2 matmult matmult-debug matmult-debug-2

sort: mpi_quicksort.c 
	mpicc -O1 mpi_quicksort.c -o quicksort
sort-debug: mpi_quicksort.c 
	mpicc -O1 -DMYDEBUG mpi_quicksort.c -o quicksort_d
sort-debug-2: mpi_quicksort.c 
	mpicc -O1 -DMYDEBUG -DMYDEBUG2 mpi_quicksort.c -o quicksort_d2

matmult: mpi_matrixmul.c 
	mpicc -O1 mpi_matrixmul.c -o matmult
matmult-debug: mpi_matrixmul.c 
	mpicc -O1 -DMYDEBUG mpi_matrixmul.c -o matmult_d
matmult-debug-2: mpi_matrixmul.c 
	mpicc -O1 -DMYDEBUG -DMYDEBUG2 mpi_matrixmul.c -o matmult_d2
	
run-sort:
	mpiexec -n 8 -f machinefile quicksort 4096
run-sort-debug-1:
	mpiexec -n 1 -f machinefile quicksort_d 4096
run-sort-debug-2:
	mpiexec -n 2 -f machinefile quicksort_d 4096
run-sort-debug-4:
	mpiexec -n 4 -f machinefile quicksort_d 4096
run-sort-debug-8:
	mpiexec -n 8 -f machinefile quicksort_d 4096
run-sort-linux:
	mpiexec -n $(np) -f machinefile quicksort_d2 $(dim)
run-sort-osx:
	mpiexec -np $(np) quicksort_d2 $(dim)

run-matmult:
	mpiexec -n 8 -f machinefile matmult 4096
run-matmult-debug-1:
	mpiexec -n 1 -f machinefile matmult_d 4096
run-matmult-debug-2:
	mpiexec -n 2 -f machinefile matmult_d 4096
run-matmult-debug-4:
	mpiexec -n 4 -f machinefile matmult_d 4096
run-matmult-debug-8:
	mpiexec -n 8 -f machinefile matmult_d 4096
run-matmult-linux:
	mpiexec -n $(np) -f machinefile matmult_d2 $(dim)
run-matmult-osx:
	mpiexec -np $(np) matmult_d2 $(dim)

clean: 
	rm matmult quicksort matmult_d quicksort_d matmult_d2 quicksort_d2 C.* output.*
