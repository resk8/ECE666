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

// void * quickSort(void * data) {
//     int split_index, i;
//     sortdata_t split_bot;
//     sortdata_t split_top;
//     sortdata_t * incoming = data;
    
//     //deep copy
//     split_top.pArr = split_bot.pArr = incoming->pArr;
//     split_top.top = split_bot.top = incoming->top;
//     split_top.bot = split_bot.bot = incoming->bot;

//     pthread_mutex_lock(&g_worker_mutex);
//     if (g_woring_threads < g_numThreads) {
//         int id = g_woring_threads;
//         g_woring_threads++;
//         pthread_mutex_unlock(&g_worker_mutex);

//         if ( incoming->bot < incoming->top ) {
//             split_index = do_split(incoming->pArr, incoming->bot, incoming->top);
        
//             split_top.bot = split_index+1;
//             //quickSort(&split_top);
//             if (pthread_create(&g_worker_threads[id], NULL, quickSort, &split_top)) {
//                 printf("Error creating thread for top split\n");
//                 return NULL;
//             }

//             split_bot.top = split_index-1;
//             quickSort(&split_bot);
//             pthread_join(g_worker_threads[id],NULL);
//         }
//     } else {
//         pthread_mutex_unlock(&g_worker_mutex);
//         _quickSort(incoming->pArr, incoming->bot, incoming->top);
//     }
//     return NULL;
// }

int main(int argc, char* argv[]) {
    int i, myid, chunk_size, worker, start, N, rval;
    double tstart1, tend1, tstart2, tend2, exectime1, exectime2;
    MPI_Status work_status;
    char file_in[15];
    char file_out[15];
    int* myarr1;
    int* myarr2;

    if(argc < 2) {
        printf("Need to pass matrix dimension\n");
        return -1;
    } else {
        N = atoi(argv[1]);
        sprintf(file_in,"input.%d.bin",N);
        sprintf(file_out,"output.%d.bin",N);
    }
    
    //create data array 
    myarr1 = (int*)malloc(N*N*sizeof(int));
    
    MPI_Init(&argc, &argv);                       //Initialize the MP
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);         //Get the processor id of the execution
    MPI_Comm_size(MPI_COMM_WORLD, &g_numThreads); //Get number of processor of the execution
    chunk_size = N/(g_numThreads);                //calculate the size of data a worker will work on
    //printf("rank = %d, number of p = %d\n", myid, g_numThreads);
    if (myid == 0) {
        /***MAIN PROCESSOR***/
        if (read_from_file(file_in,myarr1,N*N) != 1) {
#ifdef MYDEBUG
            printf("could not read input files\n");
#endif
            goto end;
        }                
#ifdef MYDEBUG
        printf("\nStarting Parallel Sorting Test...\n");
        //initialize arrays with random values
        // srand(time(NULL));
        // for(i=0; i<N; i++) {
        //     myarr1[i] = rand() % N + 1;
        //     myarr2[i] = myarr1[i];
        // }
        tstart1 = MPI_Wtime();
#endif
        //send work to workers    
        for (worker = 1; worker < g_numThreads; worker++) {            
            start = (worker-1) * chunk_size;            
        }

#ifdef MYDEBUG
        tend1 = MPI_Wtime();

        //sequential
        printf("\nStarting Sequential Sorting Test...\n");
        myarr2 = (int*)malloc(N*N*sizeof(int));
        tstart2 = MPI_Wtime();
        sequential_sort(myarr2,0,(N*N)-1);
        tend2 = MPI_Wtime();

        char result[20];
        sprintf(result, "%s", "PASS");
        for(i=0; i < N*N-1; i++) {
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
        rval = write_to_file(file_out,myarr1,N*N);
#ifdef MYDEBUG
        if(rval == 1) {
            printf("Successfully written output matrix.\n");
            rval = read_from_file(file_out,myarr2,N*N);
        } else {
            printf("Could not write output matrix\n");
        }

        if(rval == 1) {
	        for(i=0; i<N*N; i++) {
	            if (myarr1[i] != myarr2[i]) break;
            }
        
            if(i==N*N) {
                printf("integrity verified. All %d elements match.\n", N*N);
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

    }

end:
    free(myarr1);
#ifdef MYDEBUG    
    free(myarr2);
#endif
    MPI_Finalize();
    return 1;
}
