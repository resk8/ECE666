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

void do_swap(int *x, int *y) { 
    int tmp = *x; 
    *x = *y; 
    *y = tmp; 
}

int do_split(int my_arr[], int bot, int top) {
    int i = bot, j = top, middle = bot; 

    while(i < j) {
        while(my_arr[i] <= my_arr[middle] && i < top) {
            i++;
        }
        while(my_arr[j] > my_arr[middle]) {
            j--;
        }
        if(i < j) {
            do_swap(&my_arr[i], &my_arr[j]);
        }
    }
    do_swap(&my_arr[middle], &my_arr[j]);
    return j;
}

void _quickSort(int *arr, int bot, int top) {
    int split_index;

    if ( bot < top ) {
        split_index = do_split(arr, bot, top);
        _quickSort(arr, bot, split_index-1);
        _quickSort(arr, split_index+1, top);
    }
}

int write_to_file(char * filename, int *data, int num) {
    FILE * fp;
    int write_count;
    fp = fopen(filename, "w");
    if (fp ==NULL) {
	    printf("file not found: %s\n", filename);
	    return -1;
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
	    return -1;
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
#if MYDEBUG
        sprintf(file_in,"bin/input.%d.bin",N);
        sprintf(file_out,"bin/output.%d.bin",N);
#else
        sprintf(file_in,"input.%d.bin",N);
        sprintf(file_out,"output.%d.bin",N);
#endif
        N = N * N;
    } 

    MPI_Init(&argc, &argv);                       //Initialize the MP
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);         //Get the processor id of the execution
    MPI_Comm_size(MPI_COMM_WORLD, &g_numThreads); //Get number of processor of the execution
    chunk_size = N/(g_numThreads);                //calculate the size of data a worker will work on      
    //printf("rank = %d, number of p = %d\n", myid, g_numThreads);
    if (myid == 0) {
        /***MAIN PROCESSOR***/
        myarr1 = (int*)malloc(N*sizeof(int));
        if (read_from_file(file_in, myarr1, N) != 1) {
            printf("could not read input file for myarr1\n");
            free(myarr1);
            MPI_Abort(MPI_COMM_WORLD,-1);
            exit(-1);
        }     
#ifdef MYDEBUG2
        myarr2 = (int*)malloc(N*sizeof(int));
        if (read_from_file(file_in, myarr2, N) != 1) {
            printf("could not read input file for myarr2\n");
            free(myarr1);
            free(myarr2);
            MPI_Abort(MPI_COMM_WORLD,-1);
            exit(-1);
        }
#endif
#ifdef MYDEBUG
        printf("Starting Parallel Sorting Test...\n");
        tstart1 = MPI_Wtime();
#endif  
    }

    //do parallel if number of processors are more than 1 otherwise do sequential
    if(g_numThreads > 1) {
        //send work to workers
        mysubarr1 = (int*)malloc(chunk_size*sizeof(int));    
        MPI_Scatter(myarr1, chunk_size, MPI_INT, mysubarr1, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
        //main trhead does work on first chunk 
        _quickSort(mysubarr1,0 , chunk_size-1);
        //receive results from workers
        MPI_Gather(mysubarr1, chunk_size, MPI_INT, myarr1, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);    
        free(mysubarr1);
    }
    
    if(myid == 0) {
        //do final sort on all data received
        _quickSort(myarr1,0 , N-1);
#ifdef MYDEBUG
        tend1 = MPI_Wtime();
    #ifdef MYDEBUG2
        //sequential
        printf("Starting Sequential Sorting Test...\n");
        tstart2 = MPI_Wtime();
        _quickSort(myarr2,0 , N-1);
        tend2 = MPI_Wtime();

        char result[20];
        sprintf(result, "%s", "PASS");
        for(i=0; i < N-1; i++) {
            //printf("myarr1[%d] = %d, myarr2[%d] = %d\n", i, myarr1[i, i, myarr2[i]);
            if(myarr1[i] > myarr1[i+1]) {
                sprintf(result, "%s", "FAIL not sorted");
            }
        }
        printf("MPI Parallel sort. Result is %s\n", result);
        exectime2 = (tend2 - tstart2) * 1000.0; // sec to ms
        exectime2 += (tend2 - tstart2) / 1000.0; // us to ms       
        printf("Sequential execution time %.3lf ms\n", exectime2);
    #endif
        exectime1 = (tend1 - tstart1) * 1000.0; // sec to ms
        exectime1 += (tend1 - tstart1) / 1000.0; // us to ms
        printf("Parallel MPI execution time %.3lf ms\n", exectime1);
#endif
        //output data to file
        rval = write_to_file(file_out,myarr1,N);
#ifdef MYDEBUG
        if(rval == 1) {
            printf("Successfully written output sorted data.\n");
            rval = read_from_file(file_out,myarr2,N);
        } else {
            printf("Could not write output sorted data\n");
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
end:
        free(myarr1);
#ifdef MYDEBUG2
        free(myarr2);
#endif
    } 

    // Finalize the MPI environment.    
    MPI_Finalize();
    return 0;
}
