// ECE666
// Homework 2 - Parallel MPI Quicksort
// Renato Oliveira PUID: 033167709

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <mpi.h>

int g_numThreads;
int g_woring_threads;

int write_to_file(char * filename, int *data, int num) {
    FILE * fp;
    int write_count;
    fp = fopen(filename, "w");
    if (fp ==NULL) {
	    printf("file not found: %s\n", filename);
	    exit(-1);
    }
    write_count = fwrite( (const void*) data, sizeof(int),num,fp); 
    if (write_count == num) {
	    fclose(fp);
	    return 1;
    } else {
	    fclose(fp);
	    return -1;
    }
} 

int read_from_file(char * filename, int *data, int num) {
    FILE * fp;
    int read_count;
    fp = fopen(filename, "rb");
    if (fp ==NULL) {
	    printf("file not found: %s\n", filename);
	    exit(-1);
    }
    read_count = fread( (void*) data, sizeof(int),num,fp);
    if (read_count == num) {
	    fclose(fp);
	    return 1;
    } else {
	    printf("%d\n", read_count);
	    fclose(fp);
	    return -1;
    }
} 

void do_swap(int *x, int *y) { 
    int tmp = *x; 
    *x = *y; 
    *y = tmp; 
}

int do_split(int my_arr[], int bot, int top) {
    int i;
    int index = bot - 1;
    int pivot = my_arr[top];

    for (i=bot; i <= top-1; i++) {
        if (my_arr[i] < pivot) {
            index++;
            do_swap(&my_arr[index], &my_arr[i]);
        }
    }
    do_swap(&my_arr[index+1], &my_arr[top]);
    return (index+1);
}

void sequential_sort(int *arr, int bot, int top) {
    int split_index;

    if ( bot < top ) {
        split_index = do_split(arr, bot, top);
        sequential_sort(arr, bot, split_index-1);
        sequential_sort(arr, split_index+1, top);
    }
}

int main(int argc, char* argv[]) {
    int i, myid, chunk_size, worker, start, N, rval;
    double tstart1, tend1, tstart2, tend2, exectime1, exectime2;
    MPI_Status work_status;
    char file_in[20];
    char file_out[20];
    int* myarr1 = NULL;
    int* mysubarr1 = NULL;
    int* myarr2 = NULL;

    if(argc < 2) {
        printf("Need to pass matrix dimension\n");
        return -1;
    } else {
        N = atoi(argv[1]);     
        sprintf(file_in,"input.%d.bin",N);
        sprintf(file_out,"output.%d.bin",N);
        N = N * N;
    }
    
    MPI_Init(&argc, &argv);                       //Initialize the MP
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);         //Get the processor id of the execution
    MPI_Comm_size(MPI_COMM_WORLD, &g_numThreads); //Get number of processor of the execution
    chunk_size = N/(g_numThreads);                //calculate the size of data a worker will work on      
    //printf("rank = %d, number of p = %d\n", myid, g_numThreads);
    if (myid == 0) {
        /***MAIN PROCESSOR***/

        //create data arrays
        myarr1 = (int*)malloc(N*sizeof(int));
        myarr2 = (int*)malloc(N*sizeof(int));

        if (read_from_file(file_in, myarr1, N) != 1) {
#ifdef MYDEBUG
            printf("could not read input files\n");
#endif
            goto end;
        }                
#ifdef MYDEBUG
        printf("Starting Parallel Sorting Test...\n");
        tstart1 = MPI_Wtime();
#endif        
        //send work to workers    
        //MPI_Scatter(myarr1, chunk_size, MPI_INT, mysubarr1, chunk_size, MPI_INT, myid, MPI_COMM_WORLD);
        for (worker = 1; worker < g_numThreads; worker++) {
            start = worker * chunk_size; //chunk of data starting address for worker            
            MPI_Send((myarr1+start), chunk_size, MPI_INT, worker, 1, MPI_COMM_WORLD);
        }

        //main trhead does work on first chunk 
        sequential_sort(myarr1,0,chunk_size-1);

        //receive results from workers
        //MPI_Gather(mysubarr1, chunk_size, MPI_INT, myarr1, chunk_size, MPI_INT, myid, MPI_COMM_WORLD);
        for (worker = 1; worker < g_numThreads; worker++) {
            start = worker * chunk_size; //chunk of data starting address for worker
            MPI_Recv((myarr1+start), chunk_size, MPI_INT, worker, 2, MPI_COMM_WORLD, &work_status);
        }

        //do final sort on all data received
        sequential_sort(myarr1, 0, N-1);
#ifdef MYDEBUG
        tend1 = MPI_Wtime();

        //sequential
        printf("Starting Sequential Sorting Test...\n");
        tstart2 = MPI_Wtime();
        sequential_sort(myarr2,0,N-1);
        tend2 = MPI_Wtime();

        char result[20];
        sprintf(result, "%s", "PASS");
        for(i=0; i < N-1; i++) {
            //printf("myarr1[%d] = %d, myarr2[%d] = %d\n", i, myarr1[i, i, myarr2[i]);
            if(myarr1[i] > myarr1[i+1]) {
                sprintf(result, "%s", "FAIL not sorted");
            }
        }

        exectime1 = (tend1 - tstart1) * 1000.0; // sec to ms
        exectime1 += (tend1 - tstart1) / 1000.0; // us to ms
        exectime2 = (tend2 - tstart2) * 1000.0; // sec to ms
        exectime2 += (tend2 - tstart2) / 1000.0; // us to ms

        printf("MPI Parallel sort. Result is %s\n", result);
        printf("Parallel MPI execution time %.3lf ms\n", exectime1);
        printf("Sequential execution time %.3lf ms\n", exectime2);
#endif
        //output data to file
        rval = write_to_file(file_out,myarr1,N);
#ifdef MYDEBUG
        if(rval == 1) {
            printf("Successfully written output matrix.\n");
            rval = read_from_file(file_out,myarr2,N);
        } else {
            printf("Could not write output matrix\n");
        }

        if(rval == 1) {
	        for(i=0; i<N; i++) {
	            if (myarr1[i] != myarr2[i]) break;
            }
        
            if(i==N) {
                printf("integrity verified. All %d elements match.\n", N);
            } else {
                printf("Mismatch at element %d (a[%d] = %d; b[%d] = %d;)\n",
                      i, i, myarr1[i], i, myarr2[i]);
            }
        } else {
            printf("Could read output matrix for verification\n"); 
        }
#endif
    } else {
        /***WORKING PROCESSOR***/
        //Create local array for workers
        mysubarr1 = (int*)malloc(chunk_size*sizeof(int));

        //receive data from main to start work
        MPI_Recv(mysubarr1, chunk_size, MPI_INT, 0, 1, MPI_COMM_WORLD, &work_status);

        //do work
        sequential_sort(mysubarr1, 0, chunk_size-1);

        //send result to main
        MPI_Send(mysubarr1, chunk_size, MPI_INT, 0, 2, MPI_COMM_WORLD);

        //cleanup local data
        free(mysubarr1);
    }

end:
    // Finalize the MPI environment.    
    free(myarr1);    
#ifdef MYDEBUG
    free(myarr2);
#endif
    MPI_Finalize();
    return 0;
}
