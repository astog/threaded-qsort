#include "thsort.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <float.h>
#include <signal.h>
#include <math.h>

#define RAND_SEED 3468

// Array Defines
#define MAX_LENGTH 1000000
#define MIN_LENGTH 0
#define AR_STEP_SIZE 1000

// Segment Defines
#define MAX_SEG_SIZE 30000
#define MIN_SEG_SIZE 1000
#define SEG_STEP_SIZE 1000

// Statistic Defines
#define N_AVG 20    // no of values to average out
#define PERCENT_BARS 20

// #define PRINT_TABLE

static unsigned int a[MAX_LENGTH];
static unsigned int b[MAX_LENGTH];

static int log_file_fd;
static int data_file_fd;

double comp_time(struct timeval time_s, struct timeval time_e)
{

    double elap = 0.0;

    if (time_e.tv_sec > time_s.tv_sec) {
        elap += (time_e.tv_sec - time_s.tv_sec - 1) * 1000000.0;
        elap += time_e.tv_usec + (1000000 - time_s.tv_usec);
    }
    else {
        elap = time_e.tv_usec - time_s.tv_usec;
    }
    return elap;
}

double time_thsort(int ar_size, int seg_size)
{
    unsigned int i;
    for (i = 0; i < ar_size; i++) {
        b[i] = a[i];
    }

    struct timeval time_s, time_e;
    /* start timer */
    gettimeofday (&time_s, NULL);

    thsort(b, ar_size, seg_size);

    /* end timer */
    gettimeofday(&time_e, NULL);

    return comp_time(time_s, time_e);
}

double time_quicksort(int ar_size)
{
    long i;
    for (i = 0; i < ar_size; i++) {
        b[i] = a[i];
    }

    struct timeval time_s, time_e;
    /* start timer */
    gettimeofday (&time_s, NULL);

    quicksort(b, ar_size);

    /* end timer */
    gettimeofday(&time_e, NULL);

    return comp_time(time_s, time_e);
}

void safe_exit(int sig_num)
{
    printf("\n%s\n\n", "Safe Exit!");
    close(log_file_fd);
    close(data_file_fd);
    exit(0);
}

void print_percent_bar(double percent_completed, int total_bars)
{
    fprintf(stderr, "<");

    int bars_to_print = ceil((total_bars / 100.0) * percent_completed);
    int bars_to_leave = total_bars - bars_to_print;
    int i;

    for (i = 0; i < bars_to_print; ++i)
    {
        fprintf(stderr, "=");
    }

    for (i = 0; i < bars_to_leave; ++i)
    {
        fprintf(stderr, " ");
    }

    fprintf(stderr, "> %3.0f%% completed\r", percent_completed);
}

void clear_percent_bar(int total_bars)
{
    int i;
    for (i = 0; i < total_bars + 17; ++i)
    {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "\r");
}


int main(int argc, char const *argv[])
{
    // Initialize table
    srand(RAND_SEED);

    size_t index;
    for (index = 0; index < MAX_LENGTH; ++index)
    {
        a[index] = rand();
    }

    // Open log file
    log_file_fd = open("thsort.log", O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXG);
    if (log_file_fd < 0)
    {
        perror("Failed to open log file");
        exit(-1);
    }

    // Open data file
    data_file_fd = open("thsort_data.csv", O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXG);
    if (log_file_fd < 0)
    {
        perror("Failed to open log file");
        exit(-1);
    }

    // Set up safe exit
    signal(SIGINT, safe_exit);

    long double seg_size_sum = 0;
    double improvedby_sum = 0;
    int count = 0;

    int ar_size;
    for (ar_size = MIN_LENGTH; ar_size <= MAX_LENGTH; ar_size += AR_STEP_SIZE)
    {
        count++;
        printf("Calculating for ar_size =  %d\n", ar_size);
        print_percent_bar(0, PERCENT_BARS);

        // Get time for thsort
        int seg_size;
        int min_time_seg_size = MIN_SEG_SIZE;
        double min_time = time_thsort(ar_size, MIN_SEG_SIZE);
        for (seg_size = MIN_SEG_SIZE; seg_size < MAX_SEG_SIZE; seg_size += SEG_STEP_SIZE)
        {
            // Calculate the average time
            long double timeSum = 0;
            int j;
            for (j = 0; j < N_AVG; ++j)
            {
                double t = time_thsort(ar_size, seg_size);
                timeSum += t;
            }

            double avgTime = timeSum / N_AVG;

            if (min_time > avgTime)
            {
                min_time = avgTime;
                min_time_seg_size = seg_size;
            }

            print_percent_bar(((double)seg_size / MAX_SEG_SIZE * 100), PERCENT_BARS);
        }

        // Get time for quicksort
        long double timeSum = 0;
        int j;
        for (j = 0; j < N_AVG; ++j)
        {
            double t = time_quicksort(ar_size);
            timeSum += t;
        }

        double avgTime = timeSum / N_AVG;

        dprintf(log_file_fd, "qsort = %.2f, thsort = %.2f with seg_size = %d\n", avgTime, min_time, min_time_seg_size);
        dprintf(data_file_fd, "%d, %.2f, %.2f, %d\n",ar_size, avgTime, min_time, min_time_seg_size);
        fprintf(stderr, "qsort = %.2f, thsort = %.2f with seg_size = %d\n", avgTime, min_time, min_time_seg_size);

        seg_size_sum += min_time_seg_size;

        if (min_time > DBL_MIN)
            improvedby_sum += avgTime / min_time;

        clear_percent_bar(PERCENT_BARS);
    }

    double avg_seg_size = seg_size_sum / count;
    double avg_improvement = improvedby_sum / count;

    dprintf(log_file_fd, "\n######################\n\n");
    dprintf(log_file_fd, "Best seg_size = %.3f\n", avg_seg_size);
    dprintf(log_file_fd, "Avg improvement = %.3f\n", avg_improvement);

    fprintf(stderr, "\n######################\n\n");
    fprintf(stderr, "Best seg_size = %.3f\n", avg_seg_size);
    fprintf(stderr, "Avg improvement = %.3f\n", avg_improvement);

    close(log_file_fd);
    close(data_file_fd);
    return 0;
}
