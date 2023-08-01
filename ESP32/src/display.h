#ifndef DISPLAY_H
#define DISPLAY_H

#include <MD_Parola.h>
#include <SPI.h>
#include "Font.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

// Define Max7219 connections
#define DATA_PIN 13 // 23 // or MOSI
#define CS_PIN 12   // or SS
#define CLK_PIN 14  // 18  // or SCK

// light sensor
#define INPUT_PIN 39 // which is "VN" pin

// Arbitrary output pins
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#endif