#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define DIM 9

extern void matmult(int N, int *a, int *b, int *c);

void some_matmult(int N, int *a, int *b, int *c) {
  int i, j, k;

  /* Multiply matrices. */
  for (i = 0; i < N; i++)	
    for (j = 0; j < N; j++)
      for (k = 0; k < N; k++)
	    c[i*N + j] += a[i*N + k] * b[k*N + j];
}

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

int main(void) {
    int i;
    double exectime1, exectime2;
    struct timeval tstart1, tend1, tstart2, tend2;
    
    int dim = pow(2,DIM);
    int twoD_dim = dim*dim; //2dmatrix N X N
    int A[twoD_dim];
    int B[twoD_dim];
    int C[twoD_dim];
    int D[twoD_dim];

    init_mat(dim,A,B,C);
    init_mat(dim,A,B,D);

    gettimeofday( &tstart1, NULL );
    matmult(dim,A,B,C);
    gettimeofday( &tend1, NULL );


    gettimeofday( &tstart2, NULL );
    some_matmult(dim,A,B,D);
    gettimeofday( &tend2, NULL );

    char result[5];
    sprintf(result, "%s", "PASS");

    printf("\n\n");
    for(i=0;i<twoD_dim;i++)
    {
        //printf("c[%d] = %d, d[%d] = %d\n",i,C[i],i,D[i]);
        if(C[i] != D[i]) {
            sprintf(result, "%s", "FAIL");
        }
    }

    exectime1 = (tend1.tv_sec - tstart1.tv_sec) * 1000.0; // sec to ms
    exectime1 += (tend1.tv_usec - tstart1.tv_usec) / 1000.0; // us to ms

    exectime2 = (tend2.tv_sec - tstart2.tv_sec) * 1000.0; // sec to ms
    exectime2 += (tend2.tv_usec - tstart2.tv_usec) / 1000.0; // us to ms

    printf("hello world matrix mul. result is %s\n", result);
    printf("parallel execution time %.3lf ms\n", exectime1);
    printf("Sequential execution time %.3lf ms\n", exectime2);
    return;
}