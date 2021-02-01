#include <stdio.h>
#include <pthread.h>
#include <math.h>

#define DIM 9

extern void matmult(int N, int *a, int *b, int *c);

void some_matmult(int N, int *a, int *b, int *c) {
  int i, j, k;

  /* Initialize the matrices. */
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      {
	    c[i*N + j] = 0;
      }

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
    
    int dim = pow(2,DIM);
    int twoD_dim = dim*dim; //2dmatrix N X N
    int A[twoD_dim];
    int B[twoD_dim];
    int C[twoD_dim];
    int D[twoD_dim];

    init_mat(dim,A,B,C);
    init_mat(dim,A,B,D);

    matmult(dim,A,B,C);
    some_matmult(dim,A,B,D);

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
    printf("hello world matrix mul. result is %s\n", result);

    return;
}