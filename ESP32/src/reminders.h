#ifndef REMINDER_H
#define REMINDER_H

#include "clock.h"
 
bool isAlarm = false;
bool isTTS = false;
int textIndex = 0;

int reminderIndex = 0;
int reminderNum = 0;
char szText[50];

vector<vector<string>> reminders(2);
enum ReminderFields
{
    SCHED,
    TASK
};

#endif