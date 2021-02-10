#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <mpi.h>

#define DIM 8
int g_numThreads;

void init_mat(int N, double *a, double *b, double *c, double *d) {
    int i, j, k;

    srand48(time(NULL)); //Give drand48() a seed value
    
    for(j=0; j<N; j++) {
        for(i=0; i<N; i++) {
            *((a+i*N) + j) = drand48(); // 1.0;
            *((b+i*N) + j) = drand48(); // 1.0;
            *((c+i*N) + j) = 0;
            *((d+i*N) + j) = 0;
        }
    }    
}

void sequential_matmult(int N, double *a, double *b, double *c) {
  int i, j, k;

  /* Multiply matrices. */
  for (i = 0; i < N; i++)	
    for (j = 0; j < N; j++)
      for (k = 0; k < N; k++)
	    *((c+i*N) + j) += *((a+i*N) + k) * *((b+k*N) + j);
}

int main(int argc, char* argv[]) {
    int i, j, k, myid, start, end, chunk_size, worker, N;
    double tstart1, tend1, tstart2, tend2, exectime1, exectime2;
    MPI_Status work_status;

    N = pow(2,DIM);    
    double a[N][N];
    double b[N][N];
    double c[N][N];
    double d[N][N];

    MPI_Init(&argc, &argv);                 // Initialize the MPI environments
    MPI_Comm_rank(MPI_COMM_WORLD, &myid); // Get the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &g_numThreads);
    chunk_size = N/(g_numThreads-1);     // calculate the size of data a worker will work on
    //printf("rank = %d, number of p = %d\n", myid, g_numThreads);
    if (myid == 0) {        
        printf("\nStarting Matrix Multiplication Test...\n");

        //init matrices
        init_mat(N,&a[0][0],&b[0][0],&c[0][0],&d[0][0]);

        tstart1 = MPI_Wtime();
        //send work to workers
        for (worker = 1; worker < g_numThreads; worker++) {
            start = (worker-1) * chunk_size;
            MPI_Send(&start, 1, MPI_DOUBLE, worker, 1, MPI_COMM_WORLD);
            MPI_Send(&a[start][0], chunk_size*N, MPI_DOUBLE, worker, 1, MPI_COMM_WORLD);
            MPI_Send(&b, N*N, MPI_DOUBLE, worker, 1, MPI_COMM_WORLD);
        }

        //receive results from workers
        for (worker = 1; worker < g_numThreads; worker++) {
            MPI_Recv(&start, 1, MPI_DOUBLE, worker, 2, MPI_COMM_WORLD, &work_status);
            MPI_Recv(&c[start][0], chunk_size*N, MPI_DOUBLE, worker, 2, MPI_COMM_WORLD, &work_status);
        }
        tend1 = MPI_Wtime();

        //sequential
        tstart2 = MPI_Wtime();
        sequential_matmult(N,&a[0][0],&b[0][0],&d[0][0]);
        tend2 = MPI_Wtime();
        

        char result[5];
        sprintf(result, "%s", "PASS");
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++) {
                //printf("c[%d][%d] = %6.2f, d[%d][%d] = %6.2f\n", i,j, c[i][j],i,j,d[i][j]);
                if(c[i][j] != d[i][j]) {
                    sprintf(result, "%s", "FAIL");
                }
            }
        }

        exectime1 = (tend1 - tstart1) * 1000.0; // sec to ms
        exectime1 += (tend1 - tstart1) / 1000.0; // us to ms
        exectime2 = (tend2 - tstart2) * 1000.0; // sec to ms
        exectime2 += (tend2 - tstart2) / 1000.0; // us to ms

        printf("MPI Matrix mul. Result is %s\n", result);
        printf("Parallel MPI execution time %.3lf ms\n", exectime1);
        printf("Sequential execution time %.3lf ms\n", exectime2);

    } else {        
        //receive data from main to start work
        MPI_Recv(&start, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &work_status);
        MPI_Recv(&a, chunk_size*N, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &work_status);
        MPI_Recv(&b, N*N, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &work_status);

        //the work
        for(i=0; i<chunk_size; i++) {
           for(j=0; j<N; j++) {
               c[i][j] = 0;
               for(k=0; k<N; k++) {
                   c[i][j] += a[i][k] * b[k][j];
               }
           }
        }

        //send result to main
        MPI_Send(&start, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&c, chunk_size*N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
    }

    // Finalize the MPI environment.
    MPI_Finalize();
    return 0;
}
