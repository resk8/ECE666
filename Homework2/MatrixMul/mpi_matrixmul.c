#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <mpi.h>

#define MASTER 0               /* taskid of first task */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */

#define NUM_PROCESSES 6
#define DIM 9

int g_numThreads;

void init_mat(int N, int *a, int *b, int *c) {
    int i, j, k;

    for(j=0; j<N; j++) {
        for(i=0; i<N; i++) {
            *((a+i*N) + j) = 1;
            *((b+i*N) + j) = 1;
            *((c+i*N) + j) = 0;
        }
    }    
}

void some_matmult(int N, int *a, int *b, int *c) {
  int i, j, k;

  /* Multiply matrices. */
  for (i = 0; i < N; i++)	
    for (j = 0; j < N; j++)
      for (k = 0; k < N; k++)
	    *((c+i*N) + j) += *((a+i*N) + k) * *((b+k*N) + j);
}

void matmult(int N) {
    int i, j, k, myid, start, end, chunk_size, worker, d[N][N], tmp;
    MPI_Status work_status;
    int a[N][N];
    int b[N][N];
    int c[N][N];
    double tstart1, tend1, tstart2, tend2, exectime1, exectime2;
    g_numThreads = NUM_PROCESSES + 1; // 9 processors -> 8 workers, 1 main

    MPI_Init(NULL, NULL);                 // Initialize the MPI environments
    MPI_Comm_rank(MPI_COMM_WORLD, &myid); // Get the rank of the process
    if (myid == 0) {
        
        printf("\nStarting Matrix Multiplication Test...\n");

        init_mat(N,&a[0][0],&b[0][0],&c[0][0]);
        init_mat(N,&a[0][0],&b[0][0],&d[0][0]);

        // for (i=0;i<N;i++)
        //     for (j=0; j<N; j++) 
        //         printf("a[%d][%d] = %d, b[%d][%d] = %d\n", i,j, a[i][j],i,j,b[i][j]);

        tstart1 = MPI_Wtime();
        //send work to workers
        chunk_size = (N/NUM_PROCESSES);     // calculate the size of data a worker will work on
        for (worker = 1; worker < g_numThreads; worker++) {
            start = (worker-1) * chunk_size;
            MPI_Send(&start, 1, MPI_INT, worker, 1, MPI_COMM_WORLD);
            MPI_Send(&chunk_size, 1, MPI_INT, worker, 1, MPI_COMM_WORLD);
            MPI_Send(&a[start][0], chunk_size*N, MPI_INT, worker, 1, MPI_COMM_WORLD);
            MPI_Send(&b, N*N, MPI_INT, worker, 1, MPI_COMM_WORLD);
        }

        //receive results from workers
        for (worker = 1; worker < g_numThreads; worker++) {
            MPI_Recv(&start, 1, MPI_INT, worker, 2, MPI_COMM_WORLD, &work_status);
            MPI_Recv(&chunk_size, 1, MPI_INT, worker, 2, MPI_COMM_WORLD, &work_status);
            MPI_Recv(&c[start][0], chunk_size*N, MPI_INT, worker, 2, MPI_COMM_WORLD, &work_status);
        }
        tend1 = MPI_Wtime();

        //sequential
        tstart2 = MPI_Wtime();
        some_matmult(N,&a[0][0],&b[0][0],&d[0][0]);
        tend2 = MPI_Wtime();
        

        char result[5];
        sprintf(result, "%s", "PASS");
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++) {
                //printf("c[%d][%d] = %d, d[%d][%d] = %d\n", i,j, c[i][j],i,j,d[i][j]);
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
        MPI_Recv(&start, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &work_status);
        MPI_Recv(&chunk_size, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &work_status);
        MPI_Recv(&a, chunk_size*N, MPI_INT, 0, 1, MPI_COMM_WORLD, &work_status);
        MPI_Recv(&b, N*N, MPI_INT, 0, 1, MPI_COMM_WORLD, &work_status);

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
        MPI_Send(&start, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&chunk_size, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&c, chunk_size*N, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}

int main(void) {
    matmult(pow(2,DIM));
    return 0;
}