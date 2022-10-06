#include <sys/time.h>

#pragma once

struct timeval curTime;
double startTime, endTime;

void startTimer();
double getStartTime();
double getElapsedTime();
void printElapsedTime();