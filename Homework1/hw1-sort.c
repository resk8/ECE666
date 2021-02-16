// ECE666
// Homework 1 - Parallel Quick Sort
// Renato Oliveira PUID: 033167709

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_t * g_worker_threads;
pthread_mutex_t g_worker_mutex;
int g_woring_threads;
int g_numThreads;

typedef struct sortData {
    int * pArr;
    int bot;
    int top;
} sortdata_t;


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

void * quickSort(void * data) {
    int split_index, i;
    sortdata_t split_bot;
    sortdata_t split_top;
    sortdata_t * incoming = data;
    
    //deep copy
    split_top.pArr = split_bot.pArr = incoming->pArr;
    split_top.top = split_bot.top = incoming->top;
    split_top.bot = split_bot.bot = incoming->bot;

    pthread_mutex_lock(&g_worker_mutex);
    if (g_woring_threads < g_numThreads) {
        int id = g_woring_threads;
        g_woring_threads++;
        pthread_mutex_unlock(&g_worker_mutex);

        if ( incoming->bot < incoming->top ) {
            split_index = do_split(incoming->pArr, incoming->bot, incoming->top);
        
            split_top.bot = split_index+1;
            //quickSort(&split_top);
            if (pthread_create(&g_worker_threads[id], NULL, quickSort, &split_top)) {
                printf("Error creating thread for top split\n");
                return NULL;
            }

            split_bot.top = split_index-1;
            quickSort(&split_bot);
            pthread_join(g_worker_threads[id],NULL);
        }
    } else {
        pthread_mutex_unlock(&g_worker_mutex);
        _quickSort(incoming->pArr, incoming->bot, incoming->top);
    }
    return NULL;
}

void sort(int N, int *arr, int nThreads) {    
    sortdata_t data;

    g_numThreads = nThreads;
    g_worker_threads = malloc(g_numThreads * sizeof(pthread_t));

    data.pArr = arr;
    data.bot = 0;
    data.top = N-1;
    g_woring_threads = 0;
    quickSort(&data);

    free(g_worker_threads);
}