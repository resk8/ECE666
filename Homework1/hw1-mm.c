// ECE666
// Homework 1 
// Renato Oliveira PUID: 033167709

#include <stdio.h>
#include <string.h>

#define DIM 6

void init_mat(int N, int *a, int *b, int *c) {
    int i, j, k;

    for(j=0; j<N; j++) {
        for(i=0; i<N; i++) {
            a[i*N + j] = i;
            b[i*N + j] = j;
            c[i*N + j] = 0;
        }
    }    
}

void matmult(int N, int *a, int *b, int *c) {
    int i, j, k;

    for(j=0; j<N; j++) {
        for(i=0; i<N; i++) {
            c[i*N + j] = 0;
        }
    }
    for(k=0; k<N; k++) {
        for(j=0; j<N; j++) {
            for(i=0; i<N; i++) {
                c[i*N +j] += a[i*N + k] * b[k*N +j];
            }
        }
    }
}

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

int main(void) {
    int i;
    int dim = DIM * DIM; //2dmatrix 6X6
    int A[dim];
    int B[dim];
    int C[dim];

    int D[dim];
    int E[dim];
    int F[dim];

    memset(C, 0, dim*sizeof(*C));
    memset(F, 0, dim*sizeof(*F));

    init_mat(DIM,A,B,C);
    init_mat(DIM,D,E,F);

    for(i=0;i<dim;i++)
    {
        printf("c[%d] = %d, f[%d] = %d\n",i,C[i],i,F[i]);
    }

    matmult(DIM,A,B,C);
    some_matmult(DIM,D,E,F);

    char result[5];
    sprintf(result, "%s", "PASS");

    printf("\n\n");
    for(i=0;i<dim;i++)
    {
        printf("c[%d] = %d, f[%d] = %d\n",i,C[i],i,F[i]);
        if(C[i] != F[i]) {
            sprintf(result, "%s", "FAIL");
        }
    }
    printf("hello world matrix mul. result is %s\n", result);

    return;
}
