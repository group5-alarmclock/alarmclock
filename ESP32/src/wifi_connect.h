#ifndef WIFI_CONNECT_H_
#define WIFI_CONNECT_H_

#include <Arduino.h>
#include <WiFi.h>

bool hasWiFiCredentials()
{
  return !WiFi.SSID().isEmpty();
}

bool wifiConnect(const String &ssid, const String &password)
{
  unsigned long oldBoudRate = Serial.baudRate();
  if (oldBoudRate == 0ul)
  {
    Serial.begin(9600);
  }
  delay(100);
  Serial.println();
  Serial.print("Connecting to " + ssid);

  WiFi.disconnect(true);
  const char *hostname = "ESP32 Alarm Clock";
  WiFi.setHostname(hostname);
  const char *pass = password.c_str();
  if (password.isEmpty())
  {
    pass = (const char *)NULL;
  }
  WiFi.begin(ssid.c_str(), pass);

  uint8_t retries = 0;
  const uint8_t retries_max = 20;
  while (WiFi.status() != WL_CONNECTED && retries < retries_max)
  {
    retries++;
    Serial.print(".");
    delay(500);
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Successfully connected to " + ssid);
    Serial.print("the ESP's STA IP Address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("Unable to Connect to " + ssid);
  }

  // Serial.begin(oldBoudRate);
  return WiFi.status() == WL_CONNECTED;
}

bool wifiDisconnect()
{
  unsigned long oldBoudRate = Serial.baudRate();
  if (oldBoudRate == 0ul)
  {
    Serial.begin(9600);
  }
  String ssid = WiFi.SSID();
  Serial.print("disconnecting from ");
  Serial.println(ssid);

  bool res = WiFi.disconnect(true);
  if (!res)
  {
    Serial.print("failed to disconnect from ");
  }
  else
  {
    Serial.print("Successfully disconnect from ");
  }
  Serial.print(ssid);

  // Serial.begin(oldBoudRate);
  return res;
}

#endif // WIFI_CONNECT_H_
