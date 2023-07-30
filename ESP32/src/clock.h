#ifndef TIME_H
#define TIME_H

#include <time.h>

void getTimentp();

#define SPEED_TIME 75
#define PAUSE_TIME 0
#define MAX_MESG 20

// timezone in seconds, 1 hour = 3600 seconds
const int timezoneinSeconds = 3 * 3600;

int dst = 0;
uint16_t h, m, s;
uint8_t dow;
int day;
uint8_t month;
String year;
// Global variables
char szTime[9];   // mm:ss\0
char szsecond[4]; // ss
char szMesg[MAX_MESG + 1] = "";

void getsec(char *psz)
{
    sprintf(psz, "%02d", s);
}

void getTime(char *psz, bool f = true)
{
    time_t now = time(nullptr);
    struct tm *p_tm = localtime(&now);
    h = p_tm->tm_hour;
    m = p_tm->tm_min;
    s = p_tm->tm_sec;
    // sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m); // for pulsing effect
    sprintf(psz, "%02d%c%02d", h, ':', m);
    Serial.println(psz);
}

void getTimentp()
{
    configTime(timezoneinSeconds, dst, "pool.ntp.org", "time.nist.gov");
    while (!time(nullptr))
    {
        delay(500);
        Serial.print(".");
    }
    Serial.print("Time Update");
}

#endif