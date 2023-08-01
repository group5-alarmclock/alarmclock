#ifndef SOUND_H
#define SOUND_H

#include "Arduino.h"
#include "Audio.h"

// Pin definitions for the MAX98357A amplifier
#define MAX98357_BCLK_PIN 26
#define MAX98357_LRC_PIN 25
#define MAX98357_DIN_PIN 27

// microSD Card Reader connections
#define SD_CS 5
#define SPI_MOSI 23 // 32
#define SPI_MISO 19 // 33
#define SPI_SCK 18  // 19

// I2S configuration
const int i2sBitsPerSample = 16;
const int i2sBufferSize = 1024;

// Create audio object
Audio audio;

bool audioDestroyed = false;
bool audioInit = false;

void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
}
void audio_id3data(const char *info)
{ // id3 metadata
    Serial.print("id3data     ");
    Serial.println(info);
}
void audio_eof_mp3(const char *info)
{ // end of file
    Serial.print("eof_mp3     ");
    Serial.println(info);
}
void audio_showstation(const char *info)
{
    Serial.print("station     ");
    Serial.println(info);
}
void audio_showstreaminfo(const char *info)
{
    Serial.print("streaminfo  ");
    Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
    Serial.print("streamtitle ");
    Serial.println(info);
}
void audio_bitrate(const char *info)
{
    Serial.print("bitrate     ");
    Serial.println(info);
}
void audio_commercial(const char *info)
{ // duration in sec
    Serial.print("commercial  ");
    Serial.println(info);
}
void audio_icyurl(const char *info)
{ // homepage
    Serial.print("icyurl      ");
    Serial.println(info);
}
void audio_lasthost(const char *info)
{ // stream URL played
    Serial.print("lasthost    ");
    Serial.println(info);
}
void audio_eof_speech(const char *info)
{
    Serial.print("eof_speech  ");
    Serial.println(info);
}

#endif