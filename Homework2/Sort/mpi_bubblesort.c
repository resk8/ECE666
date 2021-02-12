// ECE666
// Homework 2 - Parallel MPI Quicksort
// Renato Oliveira PUID: 033167709

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <mpi.h>

#define DIM 8
int g_numThreads;
int g_woring_threads;

typedef struct sortData {
    int * pArr;
    int bot;
    int top;
} sortdata_t;

int write_to_file(char * filename, double *data, int num) {
    FILE * fp;
    int write_count;
    fp = fopen(filename, "w");
    if (fp ==NULL) {
	    printf("file not found: %s\n", filename);
	    exit(-1);
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
	    exit(-1);
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

void sequential_quickSort(int *arr, int bot, int top) {
    int split_index;

    if ( bot < top ) {
        split_index = do_split(arr, bot, top);
        sequential_quickSort(arr, bot, split_index-1);
        sequential_quickSort(arr, split_index+1, top);
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
    int i, myid, chunk_size, worker, start;
    double tstart1, tend1, tstart2, tend2, exectime1, exectime2;
    MPI_Status work_status;
    int N = pow(2,DIM);
    int myarr1[N];
    int myarr2[N];

    
    MPI_Init(&argc, &argv);                       //Initialize the MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);         //Get the processor id of the execution
    MPI_Comm_size(MPI_COMM_WORLD, &g_numThreads); //Get number of processor of the execution
    chunk_size = N/(g_numThreads-1); //calculate the size of data a worker will work on
    //printf("rank = %d, number of p = %d\n", myid, g_numThreads);

    if (myid == 0) {
        /***MAIN PROCESSOR***/
        printf("\nStarting Sorting Test...\n");

        //initialize arrays with random values
        srand(time(NULL));
        for(i=0; i<N; i++) {
            myarr1[i] = rand() % N + 1;
            myarr2[i] = myarr1[i];
        }
                    
        //send work to workers
        tstart1 = MPI_Wtime();        
        for (worker = 1; worker < g_numThreads; worker++) {            
            start = (worker-1) * chunk_size;            
        }


    } else {
        /***WORKING PROCESSOR***/

    }

    MPI_Finalize();
    return 1;
}
