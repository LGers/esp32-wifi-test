#pragma once
#include <Arduino.h>

#ifndef Oled096_H
#define Oled096_H

class Oled096
{
private:
public:
    // int dataPin;
    // int latchPin;
    // int clockPin;
    // int leds = B00000000;

    Oled096(); // Конструктор

    void init(int wirePin1Sda, int wirePin2Scl);
    void printWiFi(const char *ssid, const char *password);
    void printWiFiStatus(const String ip);
    void drawPins(const char *msg);
    void drawStatus(uint8_t ledNumber, const uint8_t status);
    void printByte(const uint32_t pinValues, int dataWidth);
};

#endif
