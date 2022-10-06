#include "stopWatch.h"
#include <stdio.h>
#include <sys/time.h>

/*Starts the timer. Also works as a reset*/
void startTimer()
{
    gettimeofday(&curTime, NULL);
    startTime = curTime.tv_sec + (curTime.tv_usec/1000000.0);
    //sleep(1);
}

/*Returns the current start time*/
double getStartTime()
{
    return startTime;
}

/*Updates and returns how long the timer has been running*/
double getElapsedTime()
{
    gettimeofday(&curTime, NULL);
    endTime = curTime.tv_sec + (curTime.tv_usec/1000000.0);
    return endTime;
}

/*Updates and prints how long the timer has been running*/
void printElapsedTime()
{
    getElapsedTime();
    printf("Time for computing: %.2f s\n", endTime-startTime);
}