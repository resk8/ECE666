#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <mpi.h>

#define NUM_PROCESSES 9
#define DIM 6

int g_numThreads;
//pthread_t * g_worker_threads;

typedef struct MatMultData {
    int _tid;
    int _N;
    int *_a;
    int *_b;
    int *_c;    
} matMultData_t;

void init_mat(int N, int *a, int *b, int *c) {
    int i, j, k;

    for(j=0; j<N; j++) {
        for(i=0; i<N; i++) {
            a[i*N + j] = 1;
            b[i*N + j] = 1;
            c[i*N + j] = 0;
        }
    }    
}

void some_matmult(int N, int *a, int *b, int *c) {
  int i, j, k;

  /* Multiply matrices. */
  for (i = 0; i < N; i++)	
    for (j = 0; j < N; j++)
      for (k = 0; k < N; k++)
	    c[i*N + j] += a[i*N + k] * b[k*N + j];
}

void * worker(void *args)
{   
    int chunk_size, start, end;
    int i, j, k, sum;
    matMultData_t *data = args;

    //printf("Matrix multplication thread thread %d\n", data->_tid);
    //split the matrix in chunks based on the amount of threads
    chunk_size = data->_N / g_numThreads;
    start = data->_tid  * chunk_size;
    end = start + chunk_size;

    for(i=start; i<end; i++) {
        for(j=0; j<data->_N; j++) {
            for(k=0; k<data->_N; k++) {
                data->_c[i*data->_N + j] += data->_a[i*data->_N + k] * data->_b[k*data->_N + j];
            }
        }
    }
    return NULL;
}

void matmult(int N, int *a, int *b, int *c) {
    int i, j, k, myid, start, end, chunk_size, worker;
    MPI_Status work_status;
    g_numThreads = NUM_PROCESSES + 1;     // 9 processors -> 8 workers, 1 main
    chunk_size = N/NUM_PROCESSES;     // calculate the size of data a worker will work on

    MPI_Init(NULL, NULL);                 // Initialize the MPI environments
    MPI_Comm_rank(MPI_COMM_WORLD, &myid); // Get the rank of the process
    if (myid == 0) {
        
        //send work to workers
        for (worker = 1; worker < g_numThreads; worker++) {
            start = (worker-1) * chunk_size; // worker 1 will start at address [0, chunk_size]
            end = start + chunk_size;

            MPI_Send(&start, 1, MPI_INT, worker, 1, MPI_COMM_WORLD);
            //MPI_Send(&end, 1, MPI_INT, worker, 1, MPI_COMM_WORLD);
            MPI_Send(&a[start], chunk_size, MPI_INT, worker, 1, MPI_COMM_WORLD);
            MPI_Send(&b[start], chunk_size, MPI_INT, worker, 1, MPI_COMM_WORLD);
        }

        //receive results from workers
        for (worker = 1; worker < g_numThreads; worker++) {
            MPI_Recv(&start, 1, MPI_INT, worker, 2, MPI_COMM_WORLD, &work_status);
            MPI_Recv(&c[start], chunk_size, MPI_DOUBLE, worker, 2, MPI_COMM_WORLD, &work_status);
        }

    } else {        
        //receive data from main to start work
        MPI_Recv(&start, 1, MPI_INT, worker, 1, MPI_COMM_WORLD, &work_status);
        //MPI_Recv(&end, 1, MPI_INT, worker, 1, MPI_COMM_WORLD, &work_status);
        MPI_Recv(&a, chunk_size, MPI_INT, 0, 1, MPI_COMM_WORLD, &work_status);
        MPI_Recv(&b, chunk_size, MPI_INT, 0, 1, MPI_COMM_WORLD, &work_status);

        //THE WORK
        for(i=0; i<chunk_size; i++) {
            for(j=0; j<N; j++) {
                for(k=0; k<N; k++) {
                    c[i*N + j] += a[i*N + k] * b[k*N + j];
                }
            }
        }

        //send result to main
        MPI_Send(&start, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&c, chunk_size, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}

int main(void) {
    int i;
    //double exectime1, exectime2;
    //struct timeval tstart1, tend1, tstart2, tend2;
    
    int dim = pow(2,DIM);
    int twoD_dim = dim*dim; //2dmatrix N X N
    int A[twoD_dim];
    int B[twoD_dim];
    int C[twoD_dim];
    int D[twoD_dim];

    printf("\nStarting Matrix Multiplication Test...\n");

    init_mat(dim,A,B,C);
    init_mat(dim,A,B,D);

    //gettimeofday( &tstart1, NULL );
    matmult(dim,A,B,C);
    //gettimeofday( &tend1, NULL );


    //gettimeofday( &tstart2, NULL );
    some_matmult(dim,A,B,D);
    //gettimeofday( &tend2, NULL );

    char result[5];
    sprintf(result, "%s", "PASS");

    //printf("\n\n");
    for(i=0;i<twoD_dim;i++)
    {
        //printf("c[%d] = %d, d[%d] = %d\n",i,C[i],i,D[i]);
        if(C[i] != D[i]) {
            sprintf(result, "%s", "FAIL");
        }
    }

    //exectime1 = (tend1.tv_sec - tstart1.tv_sec) * 1000.0; // sec to ms
    //exectime1 += (tend1.tv_usec - tstart1.tv_usec) / 1000.0; // us to ms

    //exectime2 = (tend2.tv_sec - tstart2.tv_sec) * 1000.0; // sec to ms
    //exectime2 += (tend2.tv_usec - tstart2.tv_usec) / 1000.0; // us to ms

    printf("Matrix mul. Result is %s\n", result);
    //printf("Parallel execution time %.3lf ms\n", exectime1);
    //printf("Sequential execution time %.3lf ms\n", exectime2);

    return 0;
}