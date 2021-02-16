#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define NUM_THREADS 8
#define DIM 12

extern void matmult(int N, int *a, int *b, int *c, int nThreads);
extern void sort(int N, int *arr, int nThreads);

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

void seq_do_swap(int *x, int *y) { 
    int tmp = *x; 
    *x = *y; 
    *y = tmp; 
}

int seq_do_split(int *my_arr, int bot, int top) {
    int i;
    int index = bot - 1;
    int pivot = my_arr[top];

    for (i=bot; i <= top-1; i++) {
        if (my_arr[i] < pivot) {
            index++;
            seq_do_swap(&my_arr[index], &my_arr[i]);
        }
    }
    seq_do_swap(&my_arr[index+1], &my_arr[top]);
    return (index+1);
}

void seq_quickSort(int *arr, int bot, int top) {
    int split_index;

    if ( bot < top ) {
        split_index = seq_do_split(arr, bot, top);
        seq_quickSort(arr, bot, split_index-1);
        seq_quickSort(arr, split_index+1, top);
    }
}

void seq_sort(int N, int *arr) {
    seq_quickSort(arr, 0, N-1);
}

void do_matrix_test(void) {
    int i;
    double exectime1, exectime2;
    struct timeval tstart1, tend1, tstart2, tend2;
    
    int dim = pow(2,DIM);
    int twoD_dim = dim*dim; //2dmatrix N X N
    int* A = (int*)malloc(twoD_dim*sizeof(int));
    int* B = (int*)malloc(twoD_dim*sizeof(int)); 
    int* C = (int*)malloc(twoD_dim*sizeof(int)); 
    int* D = (int*)malloc(twoD_dim*sizeof(int)); 

    printf("\nStarting Matrix Multiplication Test...\n");

    init_mat(dim,A,B,C);
    init_mat(dim,A,B,D);

    gettimeofday( &tstart1, NULL );
    matmult(dim,A,B,C,NUM_THREADS);
    gettimeofday( &tend1, NULL );


    gettimeofday( &tstart2, NULL );
    some_matmult(dim,A,B,D);
    gettimeofday( &tend2, NULL );

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

    exectime1 = (tend1.tv_sec - tstart1.tv_sec) * 1000.0; // sec to ms
    exectime1 += (tend1.tv_usec - tstart1.tv_usec) / 1000.0; // us to ms

    exectime2 = (tend2.tv_sec - tstart2.tv_sec) * 1000.0; // sec to ms
    exectime2 += (tend2.tv_usec - tstart2.tv_usec) / 1000.0; // us to ms

    printf("Matrix mul. Result is %s\n", result);
    printf("Parallel execution time %.3lf ms\n", exectime1);
    printf("Sequential execution time %.3lf ms\n", exectime2);

    free(A);
    free(B);
    free(C);
    free(D);
}

void do_sort_test(void) {
    int i;
    int size = pow(2,DIM);    
    int * myarr1 = NULL;
    int * myarr2 = NULL;
    double exectime1, exectime2;
    struct timeval tstart1, tend1, tstart2, tend2;

    size = size* size;
    myarr1 = (int*)malloc(size*sizeof(int));
    myarr2 = (int*)malloc(size*sizeof(int));

    printf("\nStarting Sorting Test...\n");

    for(i=0; i<size; i++) {
        myarr1[i] = rand() % size + 1;
        myarr2[i] = myarr1[i];
    }

    // for(i=0; i < size; i++) {
    //     printf("myarr1[%d] = %d, myarr2[%d] = %d\n", i, myarr1[i], i, myarr2[i]);
    // }

    gettimeofday( &tstart1, NULL );
    sort(size, myarr1, NUM_THREADS);
    gettimeofday( &tend1, NULL );


    gettimeofday( &tstart2, NULL );
    seq_sort(size, myarr2);   
    gettimeofday( &tend2, NULL );    

    char result[20];
    sprintf(result, "%s", "PASS");
    for(i=0; i < size-1; i++) {
        if(myarr1[i] > myarr1[i+1]) {
            sprintf(result, "%s", "FAIL not sorted");
        }
    }

    for(i=0; i < size; i++) {
        // printf("myarr1[%d] = %d, myarr2[%d] = %d\n", i, myarr1[i], i, myarr2[i]);
        if(myarr1[i] != myarr2[i]) {
            sprintf(result, "%s", "FAIL arrays differ");
        }
    }

    exectime1 = (tend1.tv_sec - tstart1.tv_sec) * 1000.0; // sec to ms
    exectime1 += (tend1.tv_usec - tstart1.tv_usec) / 1000.0; // us to ms

    exectime2 = (tend2.tv_sec - tstart2.tv_sec) * 1000.0; // sec to ms
    exectime2 += (tend2.tv_usec - tstart2.tv_usec) / 1000.0; // us to ms

    printf("Parallel sort. Result is %s\n", result);
    printf("Parallel execution time %.3lf ms\n", exectime1);
    printf("Sequential execution time %.3lf ms\n", exectime2);

    free(myarr1);
    free(myarr2);
}

int main(void) {
    do_matrix_test();
    do_sort_test();
    return 0;
}