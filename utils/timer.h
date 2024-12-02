#ifndef TIMER_H
#define TIMER_H

#include <time.h>

struct timer {
    struct timespec start_time;
    struct timespec end_time;
    int is_started;
};

void start(struct timer* timer) {
    clock_gettime(CLOCK_REALTIME, &timer->start_time);
    timer->is_started = 1;
}

double stop(struct timer* timer) {
    if (!timer->is_started)
    {
        return -1;
    }
    clock_gettime(CLOCK_REALTIME, &timer->end_time);
    
    double sec = timer->end_time.tv_sec - timer->start_time.tv_sec;
    double nsec = timer->end_time.tv_nsec - timer->start_time.tv_nsec;
    double time = (double)sec + (double)nsec * 1e-9;
    timer->is_started = 0;
    return time;
}

/*

usage

#include "timer.h"

int main() {
    struct timer timer;
    start(&timer);
    // do something
    double time = stop(&timer);
    printf("Time: %f\n", time);
    return 0;
}

*/

#endif