
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 7

void set_time(char time[N], time_t my_time);
void set_time(char time[N], int hours, int minutes);
void inc(char time[N]);
int hours(char time[N]);
int minutes(char time[N]);

int main()
{
    time_t now = time(nullptr);
    struct tm *p_tm = localtime(&now);
    int h = p_tm->tm_hour;
    int m = p_tm->tm_min;
    int s = p_tm->tm_sec;
    printf("%02d%c%02d", h, ':', m);
    return 0;
}

void set_time(char time[N], time_t my_time)
{
    time[0] = ' ';
    for (int i = 0; i < 5; i++)
        time[i + 1] = ctime(&my_time)[i + 11];
}

void set_time(char time[N], int hours, int minutes)
{
    time[0] = ' ';
    time[1] = hours / 10 + 48;
    time[2] = hours % 10 + 48;
    time[4] = minutes / 10 + 48;
    time[5] = minutes % 10 + 48;
}

void inc(char time[N])
{
    int min = (minutes(time) + 1) % 60;
    int hour = min == 0 ? (hours(time) + 1) % 24 : hours(time);
    set_time(time, hour, min);
}

void dec(char time[N])
{
    int min = (minutes(time) - 1) % 60;
    int hour = min == 0 ? (hours(time) - 1) % 24 : hours(time);
    set_time(time, hour, min);
}

int hours(char time[N])
{
    int hour = (time[1] - 48) * 10 + time[2] - 48;
    return hour;
}

int minutes(char time[N])
{
    int min = (time[4] - 48) * 10 + time[5] - 48;
    return min;
}
