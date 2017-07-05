/*
Written by: Gaurav Singh (singh431@umn.edu)
*/

#ifndef THSORT_H
#define THSORT_H

#define MIN_SEGMENT_SIZE  32768// Wont create a new thread if segment is smaller than this

typedef struct arInfo {
    int* start_addr;
    int length;
    int seg_size;
} arInfo_t;

void thsort(int* ar, int ar_size, int seg_size);
void quicksort(int *A, int len);

#endif //THSORT_H
