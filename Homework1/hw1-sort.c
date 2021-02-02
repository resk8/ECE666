// ECE666
// Homework 1 - Parallel Quick Sort
// Renato Oliveira PUID: 033167709

#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 8

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

void quickSort(int arr[], int bot, int top) {
    int split_index;

    if ( bot < top ) {
        split_index = do_split(arr, bot, top);
        quickSort(arr, bot, split_index-1);
        quickSort(arr, split_index+1, top);
    }
}

void sort(int N, int *arr) {
    quickSort(arr, 0, N);
}