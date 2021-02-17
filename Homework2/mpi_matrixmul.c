// ECE666
// Homework 2 - Parallel MPI Matrix Multiplication
// Renato Oliveira PUID: 033167709

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>

int g_numThreads;

int write_to_file(char * filename, double *data, int num) {
    FILE * fp;
    int write_count;
    fp = fopen(filename, "w");
    if (fp ==NULL) {
	    printf("file not found: %s\n", filename);
	    return -1;
    }
    write_count = fwrite( (const void*) data, sizeof(double),num,fp);
    if (write_count == num) {
	    fclose(fp);
	    return 1;
    } else {
	    fclose(fp);
	    return -1;
    }
} 

int read_from_file(char * filename, double *data, int num) {
    FILE * fp;
    int read_count;
    fp = fopen(filename, "rb");
    if (fp ==NULL) {
	    printf("file not found: %s\n", filename);
	    return -1;
    }
    read_count = fread( (void*) data, sizeof(double),num,fp);
    if (read_count == num) {
	    fclose(fp);
	    return 1;
    } else {
	    printf("%d\n", read_count);
	    fclose(fp);
	    return -1;
    }
} 

void sequential_matmult(int N, double *a, double *b, double *c) {
  int i, j, k;

  /* Multiply matrices. */
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            *((c+i*N) + j) = 0;
            for (k = 0; k < N; k++) {
                *((c+i*N) + j) += *((a+i*N) + k) * *((b+k*N) + j);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    int i, j, k, myid, start, end, chunk_size, worker, offset, N, rval;
    double tstart1, tend1, tstart2, tend2, exectime1, exectime2;
    MPI_Status work_status;
    char file_a[15];
    char file_b[15];
    char file_c[15];
    double* a = NULL;
    double* b = NULL;
    double* c = NULL;
    double* d = NULL;

    if(argc < 2) {
        printf("Need to pass matrix dimension\n");
        return -1;
    } else {
        N = atoi(argv[1]);
#ifdef MYDEBUG
        sprintf(file_a,"bin/A.%d.bin",N);
        sprintf(file_b,"bin/B.%d.bin",N);
        sprintf(file_c,"bin/C.%d.bin",N);
#else
        sprintf(file_a,"A.%d.bin",N);
        sprintf(file_b,"B.%d.bin",N);
        sprintf(file_c,"C.%d.bin",N);
#endif
    }

    //Create Matrices
    a = (double*)malloc(N*N*sizeof(double));
    b = (double*)malloc(N*N*sizeof(double));
    c = (double*)malloc(N*N*sizeof(double));

    MPI_Init(&argc, &argv);                       //Initialize the MP
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);         //Get the processor id of the execution
    MPI_Comm_size(MPI_COMM_WORLD, &g_numThreads); //Get number of processor of the execution
    chunk_size = N/(g_numThreads);                //calculate the size of data a worker will work on
    //printf("rank = %d, number of p = %d\n", myid, g_numThreads);
    if (myid == 0) {
        /***MAIN PROCESSOR***/
        if ((read_from_file(file_a,a,N*N) != 1) || (read_from_file(file_b,b,N*N) != 1)) {
            printf("could not read input files %s and %s\n",file_a,file_b);
            // Finalize the MPI environment.
            free(a);
            free(b);
            free(c);
            MPI_Abort(MPI_COMM_WORLD,-1);
            exit(-1);
        }

#ifdef MYDEBUG
        printf("Starting Parallel Matrix Multiplication Test...\n");
        tstart1 = MPI_Wtime();
#endif
        if(g_numThreads > 1)
            for (worker = 1; worker < g_numThreads; worker++) {
                start = worker * chunk_size; //chunk of data starting address for worker
                offset = start * N;          //starting row of the chunk (convert from 1D to 2D)                
                //send work to workers
                MPI_Send((a+offset), chunk_size*N, MPI_DOUBLE, worker, 1, MPI_COMM_WORLD);
                MPI_Send(b, N*N, MPI_DOUBLE, worker, 1, MPI_COMM_WORLD);
            }

        // main trhead does work on first chunk        
        for(i=0; i<chunk_size; i++) {
            for(j=0; j<N; j++) {
            *((c+i*N) + j) = 0;
                for(k=0; k<N; k++) {
                *((c+i*N) + j) += *((a+i*N) + k) * *((b+k*N) + j);
                }
            }
        }

        if(g_numThreads > 1)
            for (worker = 1; worker < g_numThreads; worker++) {
                start = worker * chunk_size; //chunk of data starting address for worker
                offset = start * N;          //starting row of the chunk (convert from 1D to 2D)                
                //receive results from workers
                MPI_Recv((c+offset), chunk_size*N, MPI_DOUBLE, worker, 2, MPI_COMM_WORLD, &work_status);
            }

#ifdef MYDEBUG
        tend1 = MPI_Wtime();
    #ifdef MYDEBUG2
        //sequential
        printf("Starting Sequential Matrix Multiplication Test...\n");
        double* d = (double*)malloc(N*N*sizeof(double));
        tstart2 = MPI_Wtime();
        sequential_matmult(N,a,b,d);
        tend2 = MPI_Wtime();

        char result[5];
        sprintf(result, "%s", "PASS");
        for(i=0;i<N;i++)
            for(j=0;j<N;j++) {
                //printf("c[%d][%d] = %6.2f, d[%d][%d] = %6.2f\n", i,j, *((c+i*N) + j),i,j,*((d+i*N) + j));
                if(*((c+i*N) + j) != *((d+i*N) + j))
                    sprintf(result, "%s", "FAIL");              
            }        

        printf("MPI Matrix mul. Result is %s\n", result);
        exectime2 = (tend2 - tstart2) * 1000.0; // sec to ms
        exectime2 += (tend2 - tstart2) / 1000.0; // us to ms
        printf("Sequential execution time %.3lf ms\n", exectime2);                
    #endif
        exectime1 = (tend1 - tstart1) * 1000.0; // sec to ms
        exectime1 += (tend1 - tstart1) / 1000.0; // us to ms        
        printf("Parallel MPI execution time %.3lf ms\n", exectime1);        
#endif
        //output data to file
        rval = write_to_file(file_c,c,N*N);
#ifdef MYDEBUG2
        if(rval == 1) {
            printf("Successfully written output matrix.\n");
            rval = read_from_file(file_c,d,N*N);
        } else {
            printf("Could not write output matrix\n");
        }

        if(rval == 1) {
	        for(i=0; i<N*N; i++) {
	            if (c[i] != d[i]) break;
            }
        
            if(i==N*N) {
                printf("integrity verified. All %d elements match.\n", N*N);
            } else {
                printf("Mismatch at element %d (a[%d] = %6.4f; b[%d] = %6.4f;)\n",
                      i, i, c[i], i, d[i]);
            }
        } else {
            printf("Could read output matrix for verification\n"); 
        }
        free(d);
#endif
    } else {
        /***WORKING PROCESSOR***/
        //receive data from main to start work
        if(g_numThreads > 1) {
            MPI_Recv(a, chunk_size*N, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &work_status);
            MPI_Recv(b, N*N, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &work_status);

            //the work
            for(i=0; i<chunk_size; i++) {
                for(j=0; j<N; j++) {
                    *((c+i*N) + j) = 0;
                    for(k=0; k<N; k++) {
                        *((c+i*N) + j) += *((a+i*N) + k) * *((b+k*N) + j);
                    }
                }
            }
            MPI_Send(c, chunk_size*N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
        }
    }

    // Finalize the MPI environment.
    free(a);
    free(b);
    free(c);
    MPI_Finalize();
    return 0;
}
