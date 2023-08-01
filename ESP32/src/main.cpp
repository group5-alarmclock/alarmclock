#include <WiFi.h>
#include <HTTPClient.h>
#include <driver/i2s.h>
#include <FirebaseESP32.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "display.h"
#include "sound.h"
#include "timer.h"
#include "modes.h"
#include "reminders.h"
#include "SD.h"
#include "FS.h"
#include "access_point.h"
#include "wifi_connect.h"

// For settings
bool autoBrightness = true;
int brightness = 12;
int volume = 10;
bool isReminders = false;
char language[10] = "arabic";
char snakeGame[3] = "10";

// WIFI config
const char *ssid = "free wifi";
const char *password = "3lejlejle";
// const char *ssid = "Tabri";
// const char *password = "0507428601";
// const char *ssid = "iPhone";
// const char *password = "12345678";

// Firebase Config and Variables
char const *URL = "https://iot-alarm-clock-4-default-rtdb.europe-west1.firebasedatabase.app/";
char const *key = "AIzaSyC7sqi414Nfn6gFGcsSfKMM0ju5r3RavIQ";

FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

FirebaseData firebaseData;
FirebaseData firebaseChanges;
unsigned long alarmsTimeCheck;
unsigned long settingsTimeCheck;

// Weather Related
String openWeatherMapApiKey = "4fd3ca02728346871e8c2d96dcf582d8";
String city = "Nazareth";
String countryCode = "IL";
int temp, minTemp, maxTemp, humidity, windSpeed;
int weatherMode = 1;

// Helper Function
void connectToWifi();
void configAudio();
void InitSDcard();
void showFiles();
void updateClock();
void InitFirebase();
void InitDatabase();
void printDatabase();
void handleChanges();
void DestroyAudio();

void checkAlarm();
bool isItTime();
void handleAlarms();
void updateAlarms();
int isItTimeAux();
bool isAlarmHandeled();
void playRingtone();
void printAlarms();
void printAlarm(int alarm);

void playAudio();
bool playFile(const char *file);
bool playText(const char *text);
void playMorningFile();

void updateSettings();
string getField(String path);
void printSettings();

void showTime();
void showDate();
void showDay();

String httpGETRequest(const char *serverName);
void weather();
void getWeather(char *psz);

void updateReminders();
void showReminders();
void playReminders();
void printReminders();

void timer();
void startTimer();
void showTimer();
void updateTimer();
void updateAndShowTimer();
void updateStudyMode();
void printTimer();

void freeHeap(int num)
{
  size_t freeHeap = esp_get_free_heap_size();
  Serial.print("Free heap size ");
  Serial.print(num);
  Serial.print(": ");
  Serial.print(freeHeap);
  Serial.print(" bytes\n");
}

/********************************************************** setup ***********************************************************/
void setup()
{
  Serial.begin(9600);
  delay(500);
  freeHeap(1);

  connectToWifi();
  DestroyAudio();
  InitSDcard();
  initButtons();
  configAudio();
  showFiles();

  freeHeap(2);

  /*********************** Dispaly ***********************/
  getTimentp();

  myDisplay.begin(3);
  myDisplay.setZone(0, 0, 3);
  myDisplay.setFont(0, numeric7Se);
  myDisplay.displayZoneText(0, szTime, PA_CENTER, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);
  myDisplay.setIntensity(brightness);

  getTime(szTime);

  /*********************** Fireabse ***********************/
  freeHeap(9);
  InitFirebase();

  if (!Firebase.RTDB.beginStream(&firebaseData, "/"))
    Serial.print("Data Error\n");

  getTime(szTime);
  freeHeap(10);
}

/*********************************************************** loop ************************************************************/
void loop()
{
  if (isButtonPressed(BUTTON_NEXT_PIN) == SHORT_PRESS) // if pressed change mode
  {
    mode = mode == 5 ? 1 : mode + 1;
    modeChanged = true;
    Serial.print("mode = ");
    Serial.println(mode);
  }
  else if (isButtonPressed(BUTTON_PREV_PIN) == SHORT_PRESS) // if pressed change mode
  {
    mode = mode == 1 ? 5 : mode - 1;
    modeChanged = true;
    Serial.print("mode = ");
    Serial.println(mode);
  }

  /************************ Modes ************************/
  switch (mode)
  {
  case 1:
    showTime();
    modeChanged = false;
    break;
  case 2:
    showDate();
    modeChanged = false;
    break;
  case 3:
    showDay();
    modeChanged = false;
    break;
  case 4:
    showReminders();
    modeChanged = false;
    break;
  case 5:
    timer();
    modeChanged = false;
    break;
  }

  /*********************** Dispaly ***********************/
  int val = analogRead(INPUT_PIN);
  if (autoBrightness)
    myDisplay.setIntensity(15 - (val / (4095 / 15)));
  else
  {
    myDisplay.setIntensity(brightness);
  }
  myDisplay.displayAnimate();

  /************************ Other ************************/
  updateClock();
  handleChanges();
  checkAlarm();
  playAudio();

  if (isTTS && !audio.isRunning())
  {
    playReminders();
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    ESP.restart();

    // sprintf(szTime, "NoConn");
    // myDisplay.displayReset(0);
    // connectToWifi();
    // modeChanged = true;
  }
}

/****************************************************** Helper Functions ******************************************************/

void playRingtone()
{
  String file = "/ringtone";
  file += String(ringtone) += ".mp3";

  if (!alarmStop)
  {
    if (alarmFinished || alarmFirstTime)
    {
      new (&audio) Audio();
      audio.setPinout(MAX98357_BCLK_PIN, MAX98357_LRC_PIN, MAX98357_DIN_PIN);
      audio.setVolume(volume);
      if (!playFile(file.c_str()))
        audio.connecttoFS(SPIFFS, "ringtone1.mp3");
      audioDestroyed = false;

      alarmFinished = false;
      alarmFirstTime = false;
    }
    if (!audio.isRunning())
    {
      alarmTimePassed += audio.getAudioCurrentTime();
      DestroyAudio();
      alarmFinished = true;
    }
    audio.loop();
  }
  if (isAlarmHandeled() || alarmTimePassed >= 5 * 60)
  {
    // Reset variables
    alarmTimePassed = 0;
    alarmFinished = false;
    alarmStarted = false;
    alarmStop = true;
    alarmFirstTime = true;

    DestroyAudio();

    if (isReminders)
    {
      isAlarm = true;
      isTTS = true;
      textIndex = -1;
    }
  }
}

void playAudio()
{
  if (audioInit)
  {
    if (audio.isRunning())
      audio.loop();
    else
      DestroyAudio();
  }
}

bool playFile(const char *file)
{
  Serial.print(file);
  if (SD.open(file))
    Serial.println(" exist");
  else
    Serial.println(" does not exist");

  if (!audioInit)
  {
    new (&audio) Audio();
    audioDestroyed = false;
    audio.setPinout(MAX98357_BCLK_PIN, MAX98357_LRC_PIN, MAX98357_DIN_PIN);
    audio.setVolume(volume);

    if (!audio.connecttoFS(SD, file))
    {
      DestroyAudio();
      return false;
    }

    audioInit = true;
    return true;
  }

  return true;
}

bool playText(const char *text)
{
  if (!audioInit)
  {
    new (&audio) Audio();
    audioDestroyed = false;
    audio.setPinout(MAX98357_BCLK_PIN, MAX98357_LRC_PIN, MAX98357_DIN_PIN);
    audio.setVolume(volume);

    if (!audio.connecttospeech(text, "en"))
    {
      DestroyAudio();
      return false;
    }

    audioInit = true;
    return true;
  }
}

void playMorningFile()
{
  char text[40];
  if (textIndex == -5)
  {
    sprintf(text, "today is %s, %s %d, %d", fullWdays[wday], months[month], day, 2000 + year);
    playText(text);
    textIndex++;
  }
  if (textIndex == -4)
  {
    sprintf(text, "Good morning");
    playText(text);
    textIndex++;
  }
  if (textIndex == -3)
  {
    sprintf(text, "it is: %02s:%02s %s", String(h).c_str(), String(m).c_str(), h < 12 ? "am" : "pm");
    playText(text);
    textIndex++;
  }
  if (textIndex == -2)
  {
    sprintf(text, "today is %s, %s %d, %d", fullWdays[wday], months[month], day, 2000 + year);
    playText(text);
    textIndex++;
  }
  else if (textIndex >= -1)
  {
    playReminders();
  }
}

bool isAlarmHandeled()
{
  if (level == 1)
  {
    return isButtonPressed(BUTTON_ALARM_PIN) == SHORT_PRESS;
  }
  else if (level == 2)
  {
    return isButtonPressed(BUTTON_ALARM_PIN, 10) == LONG_PRESS;
  }
  else if (level == 3)
  {
    if (snakeGame[0] == '1')
      return true;
    else
      return false;
  }
}

void checkAlarm()
{
  if (alarmStarted || (s >= 0 && s < 3 && isItTime()))
  {
    // Initialize Variables
    alarmStarted = true;
    alarmStop = false;
    playRingtone();
  }
}

bool isItTime()
{
  // updateAlarms();
  if (millis() - lastAlarm > 1000)
  {
    lastAlarm = millis();
    int res = isItTimeAux();
    return res > 0 ? true : false;
  }
  else
    return false;
}

int isItTimeAux()
{
  for (int i = 0; i < alarms.at(0).size(); i++)
  {
    string alarmTime;
    alarmTime += h <= 9 ? std::to_string(0) + std::to_string(h) : std::to_string(h);
    alarmTime += string(":");
    alarmTime += m <= 9 ? std::to_string(0) + std::to_string(m) : std::to_string(m);
    char alarmDay = wday + 48 - 1;

    Serial.print("alarmTime = ");
    Serial.println(alarms.at(TIME).at(i).c_str());
    Serial.print("alarmDay = ");
    Serial.println(alarmDay);

    if (alarmTime == alarms.at(TIME).at(i) && alarms.at(MODE).at(i) == string("1") &&
        (alarms.at(WDAY).at(i) == string("[]") || alarms.at(WDAY).at(i).find(alarmDay) != std::string::npos))
    {
      ringtone = std::stoi(alarms.at(RINGTONE).at(i));
      level = std::stoi(alarms.at(LEVEL).at(i));
      if (level == 3)
        Firebase.RTDB.setString(&firebaseChanges, "/Alarms/is alarm off", "00");
      firebaseChanges.clear();

      return i + 1; // the alarm number
    }
  }
  return 0;
}

void handleAlarms()
{
  if (Firebase.ready() && signupOK && millis() - alarmsTimeCheck > 1000 * 60)
  {
    alarmsTimeCheck = millis();
    updateAlarms();
  }
}

void updateAlarms()
{
  for (int i = 0; i < 5; i++)
    if (!alarms.at(i).empty())
      alarms.at(i).clear();

  Serial.print("k1\n");

  Firebase.getJSON(firebaseChanges, "/Alarms");
  const char *json = firebaseChanges.stringData().c_str();

  Serial.print("k2\n");

  // Parse the JSON object
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, json);
  if (error)
  {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  Serial.print("k3\n");

  deserializeJson(doc, json);

  Serial.print("k4\n");

  string alarmNum;
  for (int i = 0; i < 10; i++)
  {
    alarmNum = "alarm" + std::to_string(i);
    if (!doc[alarmNum]["level"])
      continue;

    Serial.print("k5\n");
    alarms.at(LEVEL).push_back(doc[alarmNum]["level"]);
    alarms.at(MODE).push_back(doc[alarmNum]["mode"]);
    alarms.at(RINGTONE).push_back(doc[alarmNum]["ringtone"]);
    alarms.at(TIME).push_back(doc[alarmNum]["time"]);
    alarms.at(WDAY).push_back(doc[alarmNum]["wdays"]);

    alarmNum = "alarm" + std::to_string(i);
  }
  strcpy(snakeGame, doc["is alarm off"]);

  doc.clear();
  printAlarms();
}

void showTime()
{
  if (modeChanged)
  {
    getTime(szTime);
    myDisplay.setFont(numeric7Se);
    myDisplay.displayReset(0);
    isPressed = false;
  }
  else
  {
    int doButtonPressType = isButtonPressed(BUTTON_DO_PIN);

    if (doButtonPressType == SHORT_PRESS)
    {
      isPressed = true;
    }
    else if (doButtonPressType == LONG_PRESS)
    {
      isAlarm = false;
      isTTS = true;
      textIndex = -1;
    }

    if (isPressed)
    {
      char fileName[25];
      sprintf(fileName, "/%s/%02d/%02d_%02d.mp3", language, h, h, m);
      if (!playFile(fileName))
      {
        sprintf(fileName, "it is: %02d:%02d %s", h, m, h < 12 ? "am" : "pm");
        playText(fileName);
      }
      isPressed = false;
    }

    // update time
    if (millis() - lastTime >= 1000)
    {
      lastTime = millis();
      getTime(szTime);

      myDisplay.displayReset(0);
    }
  }
}

void showDate()
{
  if (modeChanged)
  {
    getDate(szTime);
    myDisplay.setFont(numeric7Se1);
    myDisplay.displayReset(0);
    isPressed = false;
  }
  else
  {
    if (isButtonPressed(BUTTON_DO_PIN) == SHORT_PRESS)
    {
      isPressed = true;
    }

    if (isPressed)
    {
      char fileName[30];
      sprintf(fileName, "/%s/%02s_%02s_%04s.mp3", language, String(day).c_str(), String(month).c_str(), String(2000 + year).c_str());
      if (!playFile(fileName))
      {
        sprintf(fileName, "Today is %s %d, %d", months[month], day, 2000 + year);
        playText(fileName);
      }
      isPressed = false;
    }

    // update date
    if (h == 0 && m == 0 && (s == 0 || s == 1) && millis() - lastDate >= 1000)
    {
      lastDate = millis();
      getDate(szTime);

      myDisplay.displayReset(0);
    }
  }
}

void showDay()
{
  if (modeChanged)
  {
    getDay(szTime);
    myDisplay.setFont(numeric7Se);
    myDisplay.displayZoneText(0, szTime, PA_CENTER, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);
    myDisplay.displayReset(0);
  }
  else
  {
    if (isButtonPressed(BUTTON_DO_PIN) == SHORT_PRESS)
    {
      isPressed = true;
    }

    if (isPressed)
    {
      char fileName[20];
      sprintf(fileName, "/%s/%s.mp3", language, Wday[wday]);
      if (!playFile(fileName))
      {
        sprintf(fileName, "Today is %s", fullWdays[wday]);
        playText(fileName);
      }
      isPressed = false;
    }

    // update day
    if (h == 0 && m == 0 && (s == 0 || s == 1) && millis() - lastDay >= 1000)
    {
      lastDay = millis();
      getDay(szTime);

      myDisplay.displayReset(0);
    }
  }
}

void showReminders()
{
  char quote[10] = "SUIIIII";
  if (modeChanged)
  {
    sprintf(szText, "You have %s %s for today", reminderNum == 0 ? "no" : String(reminderNum), reminderNum == 1 ? " task" : " tasks");
    myDisplay.displayClear();
    myDisplay.displayScroll(szText, PA_RIGHT, PA_SCROLL_LEFT, 50);
    reminderIndex = 0;
  }

  int doButtonPressType = isButtonPressed(BUTTON_DO_PIN);

  if (doButtonPressType == SHORT_PRESS)
  {
    if (reminderNum == 0)
    {
    }
    else if (reminderIndex == reminderNum)
    {
      sprintf(szText, "You have %s%s for today", reminderNum == 0 ? "no" : String(reminderNum), reminderNum == 1 ? " Task" : " Tasks");
      myDisplay.displayClear();
      myDisplay.displayScroll(szText, PA_RIGHT, PA_SCROLL_LEFT, 50);
      reminderIndex = 0;
    }
    else
    {
      sprintf(szText, "%s at %s", reminders.at(TASK).at(reminderIndex).c_str(), reminders.at(SCHED).at(reminderIndex).c_str());
      myDisplay.displayClear();
      myDisplay.displayScroll(szText, PA_RIGHT, PA_SCROLL_LEFT, 50);
      reminderIndex++;
    }
  }
  else if (doButtonPressType == LONG_PRESS)
  {
    Serial.print("long pressed\n");
    isTTS = true;
    textIndex = -1;
  }

  if (isTTS && !audio.isRunning())
  {
    playReminders();
  }

  if (myDisplay.displayAnimate())
  {
    myDisplay.displayReset();
  }
}

void playReminders()
{
  Serial.print("\nindex = ");
  Serial.println(textIndex);

  char text[200];
  DestroyAudio();
  if (textIndex == -1)
  {
    sprintf(text, "%s it is: %02d:%02d %s, %s %d, %d. You have %s %s for today.",
            isAlarm == true ? (h < 12 ? "Good morning," : "Good evening,") : "", h, m, h < 12 ? "am" : "pm",
            months[month], day, 2000 + year,
            reminderNum == 0 ? "no" : String(reminderNum), reminderNum == 1 ? " task" : " tasks");
    playText(text);
    textIndex++;
  }
  else if (textIndex == 0 || textIndex < reminderNum) // need to change
  {
    sprintf(text, "");
    // for (int i = 0; i < reminderNum; i++)
    for (int i = 0; i < 3; i++)
    {
      char task[40];
      // sprintf(task, "%s at %s %s. ", reminders.at(TASK).at(i).c_str(), reminders.at(SCHED).at(i).c_str(), atoi(reminders.at(SCHED).at(i).c_str()) < 12 ? "am" : "pm");
      sprintf(task, "task %d at %02d:30 pm. ", i + 1, i + 12);
      strcat(text, task);
    }
    playText(text);
    textIndex++;
  }
  else
  {
    isTTS = false;
  }
}

void updateTimer()
{
  if (isTimerRunning)
  {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;

      if (secondsRemaining == 0)
      {
        if (minutesRemaining > 0)
        {
          minutesRemaining--;
          secondsRemaining = 59;
        }
        else // If timer is finished
        {
          if (timerState == WORK)
          {
            timerState = REST;
            // playRingtone(1, 1);
            startTimer();
          }
          else if (timerState == REST)
          {
            timerState = WORK;
            // playRingtone(2, 1);
            if (--repetition == 0) // if all repetitions are finished
            {
              isTimerRunning = false;
              return;
            }
            else
              startTimer();
          }
        }
      }
      else
      {
        secondsRemaining--;
      }
    }
  }
}

void showTimer()
{
  sprintf(szTime, "%s%02d:%02d", "@ ", minutesRemaining, secondsRemaining);
  myDisplay.displayReset(0);

  Serial.print("timer = ");
  Serial.println(szTime);
}

void updateAndShowTimer()
{
  if (isTimerRunning)
  {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;

      if (secondsRemaining == 0)
      {
        if (minutesRemaining > 0)
        {
          minutesRemaining--;
          secondsRemaining = 59;
        }
        else // If timer is finished
        {
          // play finishing alarm
          char fileName[15] = "ringtone0.mp3";
          playFile(fileName);

          if (timerState == WORK)
          {
            timerState = REST;
            startTimer();
          }
          else if (timerState == REST)
          {
            timerState = WORK;
            if (--repetition == 0) // if all repetitions are finished
            {
              isTimerRunning = false;
              return;
            }
            else
              startTimer();
          }
        }
      }
      else
      {
        secondsRemaining--;
      }
      showTimer();
    }
  }
}

void startTimer()
{
  minutesRemaining = timerState == WORK ? remainingWorkingMinutes : remainingRestingMinutes;
  secondsRemaining = timerState == WORK ? remainingWorkingSeconds : remainingRestingSeconds;

  sprintf(szTime, "%s%02d:%02d", "@ ", minutesRemaining, secondsRemaining);
  myDisplay.displayReset(0);
}

void timer()
{
  if (modeChanged)
  {
    myDisplay.displayZoneText(0, szTime, PA_CENTER, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);
  }

  int doButtonPressType = isButtonPressed(BUTTON_DO_PIN);

  if (doButtonPressType == SHORT_PRESS)
  {
    isTimerRunning = 1 - isTimerRunning;
  }
  else if (doButtonPressType == LONG_PRESS)
  {
    Serial.print("long pressed\n");
    isTimerRestartet = true;
  }

  if (modeChanged || isTimerRestartet)
  {
    // Inuputs should be from firebase
    remainingWorkingMinutes = study / 100;
    remainingWorkingSeconds = study % 100;
    remainingRestingMinutes = rest / 100;
    remainingRestingSeconds = rest % 100;

    pauseTimer = false;
    isTimerRunning = false;
    isTimerStartet = false;
    isTimerRestartet = false;

    repetition = reps;
    timerState = WORK;

    startTimer();
  }
  else if (isTimerRunning)
  {
    updateAndShowTimer();
  }
}

void handleChanges()
{
  if (Firebase.ready() && millis() - settingsTimeCheck > 200)
  {
    settingsTimeCheck = millis();
    if (Firebase.RTDB.readStream(&firebaseData))
    {
      if (firebaseData.streamAvailable())
      {
        DestroyAudio();

        int x1 = millis();
        updateSettings();
        int x2 = millis();
        Serial.print("x2 = ");
        Serial.println(x2 - x1);
        updateStudyMode();
        updateReminders();
        updateAlarms();
        int x5 = millis();
        Serial.print("overall = ");
        Serial.println(x5 - x1);
        firebaseChanges.clear();
        freeHeap(666);
      }
    }
  }
}

void DestroyAudio()
{
  if (!audioDestroyed)
  {
    Serial.print("Audio destroyed\n");
    audio.~Audio();
    audioDestroyed = true;
    audioInit = false;
    isPressed = false;
    freeHeap(8);
  }
}

void updateSettings()
{
  Firebase.getJSON(firebaseChanges, "/Settings");
  const char *json = firebaseChanges.stringData().c_str();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, json);
  if (error)
  {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  deserializeJson(doc, json);
  volume = doc["Audio"]["volume"];
  brightness = doc["Display"]["brightness"];
  autoBrightness = strcmp(doc["Display"]["auto brightness"], "on") == 0 ? true : false;
  isReminders = strcmp(doc["Reminders"], "on") == 0 ? true : false;
  strcpy(language, doc["Language"]);

  doc.clear();
  printSettings();
}

void updateStudyMode()
{
  Firebase.getJSON(firebaseChanges, "/Study Mode");
  const char *json = firebaseChanges.stringData().c_str();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, json);
  if (error)
  {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  study = doc["study"];
  rest = doc["rest"];
  reps = doc["reps"];

  doc.clear();
  printTimer();
}

void updateReminders()
{
  for (int i = 0; i < 2; i++)
    if (!reminders.at(i).empty())
      reminders.at(i).clear();
  reminderIndex = reminderNum = 0;

  char TasksPath[17];
  sprintf(TasksPath, "%s%02d%c%02d%c%04d", "Tasks/", day, '-', month, '-', 2000 + year);
  Firebase.getJSON(firebaseChanges, TasksPath);
  const char *json = firebaseChanges.stringData().c_str();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, json);
  if (error)
  {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Retrieve keys and values
  JsonObject obj = doc.as<JsonObject>();
  for (JsonPair pair : obj)
  {
    const char *key = pair.key().c_str();
    const char *value = pair.value().as<const char *>();

    reminders.at(SCHED).push_back(key);
    reminders.at(TASK).push_back(value);

    reminderNum++;
  }

  doc.clear();
  printReminders();
}

String httpGETRequest(const char *serverName)
{
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

string getField(String path)
{
  string token = string(path.c_str());
  int pos = token.rfind('/');
  return token.erase(0, token.size() == 0 ? pos : pos + 1);
}

void connectToWifi()
{
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

  Serial.println("Connected to WiFi");

  // openServer();
  // bool connected = wifiConnect(String(ssid), String(password));
  // while (!connected)
  // {
  //   StringVector info;
  //   Serial.println("ready");
  //   while (info.length() == 0)
  //   {
  //     info = getSSIDandPASSWORD();
  //     delay(500);
  //   }
  //   info.print();
  //   if (info[0] == "wifi")
  //   {
  //     String ssid = info[1];
  //     String password = info[2];
  //     connected = wifiConnect(ssid, password);
  //   }
  // }
  // closeServer();
}

void configAudio()
{
  // Set up the I2S interface for output
  i2s_config_t i2sConfig = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = 16000,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 2,
      .dma_buf_len = 1024};

  i2s_pin_config_t pinConfig = {
      .bck_io_num = MAX98357_BCLK_PIN,
      .ws_io_num = MAX98357_LRC_PIN,
      .data_out_num = MAX98357_DIN_PIN,
      .data_in_num = I2S_PIN_NO_CHANGE};

  // i2s_driver_install(I2S_NUM_1, &i2sConfig, 0, NULL);
  // i2s_set_pin(I2S_NUM_0, &pinConfig);

  Serial.println("Audio Setup complete.");
}

void InitSDcard()
{
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SPI.setFrequency(1000000);
  if (!SD.begin(SD_CS))
  {
    Serial.println("Error accessing microSD card!");
  }
}

void showFiles()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file)
  {

    Serial.print("FILE: ");
    Serial.println(file.name());

    file = root.openNextFile();
  }
}

void updateClock()
{
  if (mode != 1 && millis() - lastClock >= 1000)
  {
    lastClock = millis();
    updateTime();
  }

  if (m == 0 && s >= 3 && s < 5 && !audio.isRunning())
  {
    char fileName[25];
    sprintf(fileName, "%s/%02s/%02s_%02s.mp3", language, String(h).c_str(), String(h).c_str(), String(m).c_str());
    if (!playFile(fileName))
    {
      sprintf(fileName, "it is: %02s:%02s %s", String(h).c_str(), String(m).c_str(), h < 12 ? "am" : "pm");
      playText(fileName);
    }
  }
}

void InitFirebase()
{
  config.api_key = key;
  config.database_url = URL;

  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("ok");
    signupOK = true;
  }
  else
    Serial.printf("%s\n", config.signer.signupError.message.c_str());

  Firebase.begin(&config, &auth);

  Serial.println("Firebase Setup complete.");
}

void InitDatabase()
{
  freeHeap(11);
  // Init Sittings
  Firebase.getString(firebaseChanges, "Settings/Audio/volume");
  volume = atoi(firebaseChanges.stringData().c_str());
  Firebase.getString(firebaseChanges, "Settings/Display/auto brightness");
  autoBrightness = strcmp(firebaseChanges.stringData().c_str(), "on") == 0 ? true : false;
  Firebase.getString(firebaseChanges, "Settings/Display/brightness");
  brightness = atoi(firebaseChanges.stringData().c_str());
  Firebase.getString(firebaseChanges, "Settings/Language");
  strcpy(language, firebaseChanges.stringData().c_str());
  firebaseChanges.clear();

  // Init Timer
  Firebase.getString(firebaseChanges, "Study Mode/reps");
  reps = atoi(firebaseChanges.stringData().c_str());
  Firebase.getString(firebaseChanges, "Study Mode/rest");
  rest = atoi(firebaseChanges.stringData().c_str());
  Firebase.getString(firebaseChanges, "Study Mode/study");
  study = atoi(firebaseChanges.stringData().c_str());
  firebaseChanges.clear();

  // Init Reminders
  updateReminders();

  // Init Alarms
  updateAlarms();
}

void printDatabase()
{
  Serial.println("\n******Database******");
  printSettings();
  printTimer();
  printReminders();
  printAlarms();
}

void printSettings()
{
  Serial.println();
  Serial.print("volume = ");
  Serial.println(volume);
  Serial.print("auto = ");
  Serial.println(autoBrightness);
  Serial.print("bright = ");
  Serial.println(brightness);
  Serial.print("reminders = ");
  Serial.println(isReminders);
  Serial.print("language = ");
  Serial.println(language);
  Serial.println();
}

void printTimer()
{
  Serial.print("reps = ");
  Serial.println(reps);
  Serial.print("rest = ");
  Serial.println(rest);
  Serial.print("study = ");
  Serial.println(study);
  Serial.println();
}

void printAlarms()
{
  for (int i = 0; i < alarms.at(0).size(); i++)
    printAlarm(i);

  Serial.print("snakeGame = ");
  Serial.println(snakeGame);
  Serial.println();
}

void printAlarm(int alarm)
{
  Serial.print("alarm");
  Serial.print(alarm + 1);
  Serial.print(": \n");
  Serial.print("level = ");
  Serial.println(alarms.at(LEVEL).at(alarm).c_str());
  Serial.print("mode = ");
  Serial.println(alarms.at(MODE).at(alarm).c_str());
  Serial.print("ringtone = ");
  Serial.println(alarms.at(RINGTONE).at(alarm).c_str());
  Serial.print("time = ");
  Serial.println(alarms.at(TIME).at(alarm).c_str());
  Serial.print("wday = ");
  Serial.println(alarms.at(WDAY).at(alarm).c_str());
  Serial.println();
}

void printReminders()
{
  for (int i = 0; i < reminderNum; i++)
  {
    Serial.print(reminders.at(SCHED).at(i).c_str());
    Serial.print(" - ");
    Serial.println(reminders.at(TASK).at(i).c_str());
  }
  Serial.println();
}
