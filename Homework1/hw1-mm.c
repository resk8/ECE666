// ECE666
// Homework 1 - Parallel Matrix Multiplication
// Renato Oliveira PUID: 033167709

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int g_numThreads;
pthread_t * g_worker_threads;

typedef struct MatMultData {
    int _tid;
    int _N;
    int *_a;
    int *_b;
    int *_c;    
} matMultData_t;

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
    int i;
    g_numThreads = nThreads;
    g_worker_threads = malloc(g_numThreads * sizeof(pthread_t));    

    /////////////////////////////////////////////////
    //WARNING: Super ugly code below. forgive me :(//
    /////////////////////////////////////////////////

    //had to create copies of this matrix data structure
    //only for the purpose of avoiding undercount/overcount
    //with _tid
    matMultData_t data[g_numThreads];
    data[0]._N = N;
    data[0]._a = a;
    data[0]._b = b;
    data[0]._c = c;    
    data[0]._tid = 0;
    
    for(i=1; i<g_numThreads; i++)
    {
        data[i]._N = N;
        data[i]._a = a;
        data[i]._b = b;
        data[i]._c = c;    
        data[i]._tid = i;
        
        //[i-1] thread 1 to 7
        if(pthread_create(&g_worker_threads[i-1], NULL, worker, &data[i])) {
            printf("Error creating thread %d\n", i);
        }
    }
    worker(&data[0]);

    for(i=0; i<g_numThreads-1; i++)
    {
        pthread_join(g_worker_threads[i],NULL);
    }

    free(g_worker_threads);
}

