#ifndef TIMER_H
#define TIMER_H

#include "clock.h"

char szTimer[6]; // mm:ss
int repetition;
bool pauseTimer = false;
bool isTimerRunning = false;
bool isTimerStartet = false;
bool isTimerRestartet = false;

int reps;
int rest;
int study;

int remainingWorkingMinutes;
int remainingWorkingSeconds;
int remainingRestingMinutes;
int remainingRestingSeconds;
int minutesRemaining;
int secondsRemaining;

enum TimerState
{
    WORK,
    REST
};
TimerState timerState;

unsigned long previousMillis = 0;
const unsigned long interval = 1000;

void getTimer(char *psz, bool atWork = true)
{
    if (atWork)
    {
        sprintf(psz, "%02d:%02d", remainingWorkingMinutes, remainingWorkingSeconds);
        Serial.println(psz);
    }
    else
    {
        sprintf(psz, "%02d:%02d", remainingRestingMinutes, remainingRestingSeconds);
        Serial.println(psz);
    }
}

#endif