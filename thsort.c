#include "thsort.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// #define DEBUG

// Source: http://rosettacode.org/wiki/Sorting_algorithms/Quicksort#C
void quicksort(int *A, int len)
{
    if (len < 2) return;

    int pivot = A[len / 2];

    int i, j;
    for (i = 0, j = len - 1; ; i++, j--)
    {
        while (A[i] < pivot) i++;
        while (A[j] > pivot) j--;

        if (i >= j) break;

        int temp = A[i];
        A[i]     = A[j];
        A[j]     = temp;
    }

    quicksort(A, i);
    quicksort(A + i, len - i);
}

void* s_thread_routine(void* arg)
{
    int* ar = ((arInfo_t*)arg) -> start_addr;
    int length = ((arInfo_t*)arg) -> length;
    int seg_size = ((arInfo_t*)arg) -> seg_size;

    thsort(ar, length, seg_size);
}

void thsort(int *arr, int len, int seg_size)
{
    if (len < 2) return;

    int pivot = arr[len / 2];
    int i, j;
    for (i = 0, j = len - 1; ; i++, j--)
    {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;

        if (i >= j) break;

        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }

    // Split in two threads if segment size is big enough
    if (len > seg_size)
    {
        pthread_t s_thread1, s_thread2;
        arInfo_t s_arInfo1, s_arInfo2;
        short unsigned int created_thread1 = 0, created_thread2 = 0;

        // thread 1
        if (i > seg_size)
        {
            s_arInfo1.start_addr = arr;
            s_arInfo1.length = i;
            s_arInfo1.seg_size = seg_size;

            #ifdef DEBUG
            printf("Created thread 1, size = %d\n", s_arInfo1.length);
            #endif

            pthread_create(&s_thread1, NULL, s_thread_routine, (void*)&s_arInfo1);
            created_thread1 = 1;
        }
        else
        {
            #ifdef DEBUG
            printf("NOT creating thread 1, size = %d\n", i);
            #endif

            quicksort(arr, i);
        }

        // thread 2
        if (len - i > seg_size)
        {
            s_arInfo2.start_addr = arr + i;
            s_arInfo2.length = len - i;
            s_arInfo2.seg_size = seg_size;

            #ifdef DEBUG
            printf("Created thread 2, size = %d\n", s_arInfo2.length);
            #endif

            pthread_create(&s_thread2, NULL, s_thread_routine, (void*)&s_arInfo2);
            created_thread2 = 1;
        }
        else
        {
            #ifdef DEBUG
            printf("NOT creating thread 2, size = %d\n", len - i);
            #endif

            quicksort(arr + i, len - i);
        }

        // Sync
        if (created_thread1)
        {
            #ifdef DEBUG
            printf("Joining thread 1\n");
            #endif

            pthread_join(s_thread1, NULL);
        }
        if (created_thread2)
        {
            #ifdef DEBUG
            printf("Joining thread 2\n");
            #endif

            pthread_join(s_thread2, NULL);
        }
    }
    else
    {
        quicksort(arr, i);
        quicksort(arr + i, len - i);
    }
}
