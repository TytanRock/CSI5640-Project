#pragma once

#include <stdint.h>

typedef struct _timer_instance_t{
    double start_time;

    double *laps; // Malloc'd and re-alloc'd whenever we lap
    char **lap_names;
    uint_least32_t lap_count;

    unsigned int initialized : 1;
    unsigned int started : 1;
} timer_instance_t;

void initTimer(timer_instance_t *tmr);
void startTimer(timer_instance_t *tmr);
void lapTimer(timer_instance_t *tmr, char const *lap_name);
double getStartTime(timer_instance_t const *tmr);
double getElapsedTime(timer_instance_t const *tmr);
void printElapsedTime(timer_instance_t const *tmr);