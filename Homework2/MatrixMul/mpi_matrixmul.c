#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <mpi.h>

#define NUM_THREADS 8
#define DIM 9

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

void matmult(int N, int *a, int *b, int *c, int nThreads) {
    // int i;
    // g_numThreads = nThreads;
    // g_worker_threads = malloc(g_numThreads * sizeof(pthread_t));    

    // /////////////////////////////////////////////////
    // //WARNING: Super ugly code below. forgive me :(//
    // /////////////////////////////////////////////////

    // //had to create copies of this matrix data structure
    // //only for the purpose of avoiding undercount/overcount
    // //with _tid
    // matMultData_t data[g_numThreads];
    // data[0]._N = N;
    // data[0]._a = a;
    // data[0]._b = b;
    // data[0]._c = c;    
    // data[0]._tid = 0;
    
    // for(i=1; i<g_numThreads; i++) {
    //     data[i]._N = N;
    //     data[i]._a = a;
    //     data[i]._b = b;
    //     data[i]._c = c;    
    //     data[i]._tid = i;
        
    //     //[i-1] thread 1 to 7
    //     if(pthread_create(&g_worker_threads[i-1], NULL, worker, &data[i])) {
    //         printf("Error creating thread %d\n", i);
    //     }
    // }
    // worker(&data[0]);

    // for(i=0; i<g_numThreads-1; i++) {
    //    pthread_join(g_worker_threads[i],NULL);
    // }

    // free(g_worker_threads);

    // Initialize the MPI environments
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("Hello world from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);

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
    matmult(dim,A,B,C,NUM_THREADS);
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