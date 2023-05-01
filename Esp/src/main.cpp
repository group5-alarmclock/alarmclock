#include <FirebaseESP32.h>
#include <WiFi.h>
#include "display.h"
#include "sound.h"
#include "clock.h"

// WIFI Config Setting
char const *ssid = "free wifi";
char const *password = "3lejlejle";

// Firebase Config and Variables
char const *URL = "https://iot-alarm-clock-4-default-rtdb.europe-west1.firebasedatabase.app";
char const *key = "AIzaSyC7sqi414Nfn6gFGcsSfKMM0ju5r3RavIQ";

FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

FirebaseData firebaseData;
FirebaseData FirebaseVolume;
FirebaseData FirebaseAutoBrightness;
FirebaseData FirebaseBrightness;

// For multitaskig
unsigned long previousMillisAudio = 0;
unsigned long intervalAudio = 10; // Interval for audio playback in milliseconds

unsigned long previousMillisOther = 0;
unsigned long intervalOther = 100; // Interval for other function in milliseconds

// Helper Function

// Helper variables
bool autoBrightness;
int brightness = 3;

int volume;
bool radioOn = false;
int radioChannel;

/************************************************************ setup *************************************************************/

void setup(void)
{
  Serial.begin(9600);
  delay(10);

  /************************WIFI************************/
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int timeout_counter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
    timeout_counter++;
    if (timeout_counter >= 20 * 5)
    {
      ESP.restart();
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(3000);
  WiFi.mode(WIFI_STA);

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

  /********************** Firebase **********************/
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
  Firebase.reconnectWiFi(true);

  if (!Firebase.RTDB.beginStream(&FirebaseVolume, "Audio/volume"))
    Serial.print("Volume Error\n");
  if (!Firebase.RTDB.beginStream(&FirebaseAutoBrightness, "Display/auto brightness"))
    Serial.print("auto brightness Error\n");
  if (!Firebase.RTDB.beginStream(&FirebaseBrightness, "Display/brightness"))
    Serial.print("brightness Error\n");

  // Initializing Variablesif (Firebase.ready())
  Firebase.RTDB.getInt(&FirebaseVolume, "Audio/volume");
  // volume = FirebaseVolume.intData();

  // brightness = FirebaseBrightness.intData();
  // autoBrightness = FirebaseAutoBrightness.intData();

  // Serial.print("auto = ");
  // Serial.print(FirebaseAutoBrightness.intData());
  // Serial.print("\nbright = ");
  // Serial.print(FirebaseBrightness.intData());
  // Serial.print("\nvolume = ");
  // Serial.println(FirebaseVolume.intData());

  /************************ time ************************/
  getTimentp();

  myDisplay.begin(3);
  // myDisplay.setInvert(true);
  myDisplay.setZone(0, 0, 0);
  myDisplay.setZone(1, 1, 3);
  myDisplay.setFont(0, numeric7Seg);
  myDisplay.setFont(1, numeric7Se);
  myDisplay.displayZoneText(0, szsecond, PA_CENTER, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);
  myDisplay.displayZoneText(1, szTime, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);

  getTime(szTime);

  /************************ audio ************************/
  // Connect MAX98357 I2S Amplifier Module
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  // Set thevolume (0-100)
  audio.setVolume(FirebaseVolume.intData());

  // Connect to an Internet radio station (select one as desired)
  // audio.connecttohost("0n-80s.radionetz.de:8000/0n-70s.mp3");
  // audio.connecttohost("www.antenne.de/webradio/antenne.m3u");
  // audio.connecttohost("http://vis.media-ice.musicradio.com/CapitalMP3");
  // audio.connecttohost("www.surfmusic.de/m3u/100-5-das-hitradio,4529.m3u");
  // audio.connecttohost("stream.1a-webradio.de/deutsch/mp3-128/vtuner-1a");
}

/************************************************************* Loop *************************************************************/

int touched = 0;
void loop(void)
{
  /*********************** firebase **********************/
  // if (Firebase.ready())
  // {
  //   if (Firebase.RTDB.readStream(&FirebaseVolume))
  //   {
  //     if (FirebaseVolume.streamAvailable())
  //     {
  //       Serial.print("volume = ");
  //       Serial.print(FirebaseVolume.intData());
  //       Serial.println();
  //       volume = FirebaseVolume.intData();
  //     }
  //   }
  //   if (Firebase.RTDB.readStream(&FirebaseBrightness))
  //   {
  //     if (FirebaseBrightness.streamAvailable())
  //     {
  //       Serial.print("brightness = ");
  //       Serial.print(FirebaseBrightness.intData());
  //       Serial.println();
  //     }
  //   }
  //   if (Firebase.RTDB.readStream(&FirebaseAutoBrightness))
  //   {
  //     if (FirebaseAutoBrightness.streamAvailable())
  //     {
  //       Serial.print("AutoBrightness = ");
  //       autoBrightness = FirebaseAutoBrightness.stringData() == "on" ? true : false;
  //       Serial.print(autoBrightness);
  //       Serial.println();
  //     }
  //   }
  // }

  /*********************** Display **********************/

  unsigned long currentMillis = millis(); // for multitasking

  static uint32_t lastTime = 0; // millis() memory
  static uint8_t display = 0;   // current display mode
  static bool flasher = false;  // seconds passing flasher

  // for the input
  int val = analogRead(INPUT_PIN);
  if (autoBrightness)
    myDisplay.setIntensity(15 - (val / (4095 / 15)));
  else
    myDisplay.setIntensity(brightness);

  // change UI by touch:
  // if (touchRead(4) < 50) // if touched
  // {
  //   if (touched == 0)
  //   {
  //     myDisplay.setZone(0, 0, 3);
  //     myDisplay.setFont(0, numeric7Se);
  //     myDisplay.displayZoneText(0, szsecond, PA_LEFT, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);
  //   }
  //   else
  //   {
  //     myDisplay.setZone(0, 0, 0);
  //     myDisplay.setZone(1, 1, 3);
  //     myDisplay.setFont(0, numeric7Seg);
  //     myDisplay.setFont(1, numeric7Se);
  //     myDisplay.displayZoneText(0, szsecond, PA_CENTER, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);
  //     myDisplay.displayZoneText(1, szTime, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
  //   }
  //   touched = 1 - touched;
  // }

  myDisplay.displayAnimate();

  if (millis() - lastTime >= 1000)
  {
    lastTime = millis();
    getsec(szsecond);
    getTime(szTime, flasher);
    flasher = !flasher;

    myDisplay.displayReset(0);
    myDisplay.displayReset(1);
  }

  /************************ audio ************************/
  audio.loop();
  audio.setVolume(volume);
}