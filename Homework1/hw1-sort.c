// ECE666
// Homework 1 - Parallel Quick Sort
// Renato Oliveira PUID: 033167709

#include <stdio.h>
#include <pthread.h>

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

void * quickSort(void * data) {
    int split_index, i;
    sortdata_t split_bot;
    sortdata_t split_top;
    sortdata_t * incoming = data;
    
    //deep copy
    split_top.pArr = split_bot.pArr = incoming->pArr;
    split_top.top = split_bot.top = incoming->top;
    split_top.bot = split_bot.bot = incoming->bot;

    pthread_t worker_threads[2];
    if ( incoming->bot < incoming->top ) {
        split_index = do_split(incoming->pArr, incoming->bot, incoming->top);
        
        split_bot.top = split_index-1;
        //quickSort(&split_bot);
        if (pthread_create(&worker_threads[0], NULL, quickSort, &split_bot)) {
            printf("Error creating thread for bot split\n");
            return NULL;
        }

        split_top.bot = split_index+1;
        //quickSort(&split_top);
        if (pthread_create(&worker_threads[1], NULL, quickSort, &split_top)) {
            printf("Error creating thread for top split\n");
            return NULL;
        }

        for(i=0; i<2; i++)
        {
            pthread_join(worker_threads[i],NULL);
        }
    }
    return NULL;
}

void sort(int N, int *arr) {
    sortdata_t data;
    data.pArr = arr;
    data.bot = 0;
    data.top = N-1;
    quickSort(&data);
}