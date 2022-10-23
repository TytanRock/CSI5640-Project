#include "stopWatch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static double get_current_time()
{
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    double seconds = time.tv_sec;
    seconds += time.tv_nsec / 1.0e9;

    return seconds;
}

void initTimer(timer_instance_t *tmr)
{
    if (tmr == NULL) return;
    tmr->initialized = 1;
    tmr->started = 0;
    tmr->start_time = 0;
    tmr->laps = NULL;
    tmr->lap_names = NULL;
    tmr->lap_count = 0;
}

/*Starts the timer. Also works as a reset*/
void startTimer(timer_instance_t *tmr)
{
    tmr->start_time = get_current_time();
    tmr->started = 1;
}

void lapTimer(timer_instance_t *tmr, char const *lap_name)
{
    if (tmr->laps == NULL) {
        /* Initialize it with enough space for a double */
        tmr->laps = malloc(sizeof(double) * 1);
        tmr->lap_names = malloc(sizeof(char *) * 1);
        tmr->lap_names[tmr->lap_count] = malloc(strlen(lap_name));
    } else {
        tmr->laps = realloc(tmr->laps, sizeof(double) * (tmr->lap_count + 1));
        tmr->lap_names = realloc(tmr->lap_names, sizeof(char *) * (tmr->lap_count + 1));
        tmr->lap_names[tmr->lap_count] = malloc(strlen(lap_name));
    }
    tmr->laps[tmr->lap_count] = get_current_time();
    strcpy(tmr->lap_names[tmr->lap_count], lap_name);
    tmr->lap_count++;
}

/*Returns the current start time*/
double getStartTime(timer_instance_t const *tmr)
{
    return tmr->start_time;
}

/*Updates and returns how long the timer has been running*/
double getElapsedTime(timer_instance_t const *tmr)
{
    return get_current_time() - getStartTime(tmr);
}

/*Updates and prints how long the timer has been running*/
void printElapsedTime(timer_instance_t const *tmr)
{
    printf("Total time spent: %.3f s\n", getElapsedTime(tmr));
    for (int i = 0; i < tmr->lap_count; ++i)
    {
        printf(" %s took %.3f s\n", tmr->lap_names[i], tmr->laps[i] - (i == 0 ? tmr->start_time : tmr->laps[i-1]));
    }
}