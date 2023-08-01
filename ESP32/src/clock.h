#ifndef TIME_H
#define TIME_H

#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

#define SPEED_TIME 75
#define PAUSE_TIME 0
#define MAX_MESG 20
#define TIME_ZONE 3

// timezone in seconds, 1 hour = 3600 seconds
const int timezoneinSeconds = TIME_ZONE * 3600;

unsigned int lastClock = 0;
unsigned int lastTime = 0;
unsigned int lastDate = 0;
unsigned int lastDay = 0;
unsigned int lastTimer = 0;
unsigned int lastAlarm = 0;

int dst = 0;
bool showSec = false;
uint16_t h, m, s;
uint8_t wday;
int day;
int month;
int year;
// Global variables
char szTime[30];  // mm:ss\0
char szsecond[4]; // ss
char szQuote[10]; // ss
char Wday[8][6] = {"TODAY", "SUN", "MON", "TUES", "WED", "THURS", "FRI", "SAT"};
char fullWdays[8][10] = {"TODAY", "Sunday", "Monday", "Tuesday", "Wensday", "Thursday", "Friday", "Saturday"};
char months[13][10] = {"", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

// For the alarm
bool alarmStarted = false;
bool alarmFinished = false;
bool alarmStop = false;
bool alarmFirstTime = true;
int touched = 0;
int ringtone = 1;
int level = 1;
static uint32_t alarmTimePassed = 0;
vector<vector<string>> alarms(5);
enum AlarmFields
{
    LEVEL,
    MODE,
    RINGTONE,
    TIME,
    WDAY
};

// Alt Server
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", timezoneinSeconds);

void getsec(char *psz)
{
    sprintf(psz, "%02d", s);
}

void setVars(struct tm *p_tm)
{
    h = p_tm->tm_hour;
    m = p_tm->tm_min;
    s = p_tm->tm_sec;

    day = p_tm->tm_mday;
    month = p_tm->tm_mon + 1;
    year = p_tm->tm_year - 100;

    wday = p_tm->tm_wday + 1;
}

void updateTime()
{
    time_t now = time(nullptr);
    struct tm *p_tm = localtime(&now);

    setVars(p_tm);
}

void getTime(char *psz)
{
    updateTime();

    if (showSec)
        sprintf(psz, "%02d%c%02d%c%c%c", h, ':', m, ':', s / 10 + 1, s % 10 + 1);
    else
        sprintf(psz, "%02d%c%02d", h, ':', m);

    Serial.println(psz);
}

void getDate(char *psz)
{
    updateTime();

    sprintf(psz, "%d%c%d%c%d", day, '.', month, '.', year);
    Serial.println(psz);
}

void getDay(char *psz)
{
    updateTime();

    sprintf(psz, "%s", Wday[wday]);
    Serial.println(psz);
}

void getTimentp()
{
    configTime(timezoneinSeconds, 0, "pool.ntp.org");

    Serial.print("Waiting for time synchronization");
    while (!time(nullptr))
    {
        Serial.print(".");
        delay(100);
    }
}

#endif