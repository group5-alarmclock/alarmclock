#ifndef SCAN_NETWORK_H_
#define SCAN_NETWORK_H_

#include <Arduino.h>
#include <WiFi.h>
#include "string_vector.h"

bool isSTAEnabled()
{
  wifi_mode_t currentMode = WiFi.getMode();
  return (currentMode & WIFI_STA) != 0;
}

//disconnects from every network and AP. not with (false,false,true,100U)
void scanNetwork()
{
  unsigned long oldBoudRate = Serial.baudRate();
  if (oldBoudRate==0ul) {
    Serial.begin(9600);
  }

  bool isPrevSTASet = isSTAEnabled();
  WiFi.enableSTA(true);

  Serial.println("scan start");

  int16_t numOfNetworks = WiFi.scanNetworks(false,false,true,100U);
  Serial.println("scan done");
  if (numOfNetworks == 0) {
    Serial.println("no networks found");
  } else {
    Serial.println(String(numOfNetworks)+" networks found");
    for (int16_t i = 0; i < numOfNetworks; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": "+WiFi.SSID(i));
      Serial.print(" ("+String(WiFi.RSSI(i))+")");
      Serial.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      Serial.print(" - "+WiFi.BSSIDstr(i));
      Serial.println(" - "+String(WiFi.channel(i)));
      delay(10);
    }
  }
  Serial.println("");

  WiFi.enableSTA(isPrevSTASet);
  // Serial.begin(oldBoudRate);
}

StringVector scanNetworkString()
{
  unsigned long oldBoudRate = Serial.baudRate();
  if (oldBoudRate==0ul) {
    Serial.begin(9600);
  }
  StringVector vec;
  bool isPrevSTASet = isSTAEnabled();
  WiFi.enableSTA(true);

  Serial.println("scan start");

  int16_t numOfNetworks = WiFi.scanNetworks(false,false,true,100U);
  Serial.println("scan done");
  if (numOfNetworks == 0) {
    Serial.println("no networks found");
  } else {
    Serial.println(String(numOfNetworks)+" networks found");
    for (int16_t i = 0; i < numOfNetworks; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": "+WiFi.SSID(i));
      Serial.print(" ("+String(WiFi.RSSI(i))+")");
      Serial.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      Serial.print(" - "+WiFi.BSSIDstr(i));
      Serial.println(" - "+String(WiFi.channel(i)));
      delay(10);
      vec.insert(WiFi.SSID(i)+(WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
    }
  }
  Serial.println("");

  WiFi.enableSTA(isPrevSTASet);
  // Serial.begin(oldBoudRate);
  return vec;
}


#endif // SCAN_NETWORK_H_
