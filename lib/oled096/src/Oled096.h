#pragma once
#include <Arduino.h> //TODO: pragma once
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C // Address 0x3D for 128x64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#ifndef Oled096_H
#define Oled096_H


class oledd096
{
private:
    uint8_t m_led_counts = 8;
    uint32_t ledsPinLastState;                       // B010001110 - on/off
    uint32_t ledsPinCurrentState = ledsPinLastState; // B010001110 - on/off

    // ledState_struct ledsState2[8];

    uint32_t currentMillis = millis();
    uint32_t prevMillis = 0;

    uint16_t blinkDelay = 300;

    //---------------------
    unsigned long previousMillis = 0;
    unsigned long changetBit = B00000000;

    // void onBit(uint32_t bitNumber);
    // void offBit(uint32_t bitNumber);
    // bool isLedOn(uint32_t ledNumber);
    // char getBits(uint32_t n);
    // void printBits(uint32_t n);
    // void fn(uint32_t n);
    // void intFn(uint32_t n);
    // void switchToDefaultLedState(uint32_t n);

public:
    int dataPin;
    int latchPin;
    int clockPin;
    int leds = B00000000;

    oledd096(); // Конструктор

    void init(int wirePin1, int wirePin2, int clockPin1);
    // void init(int dataPin1, int latchPin1, int clockPin1);
    // void onAll();
    // void offAll();
    // void on(int ledNumber);
    // void off(int ledNumber);

    // void blink1(int ledNumber);
    // void blink1(int ledNumber, uint16_t ledBlinkDelay);
    // void blinkTimes(int ledNumber, uint16_t ledBlinkDelay, uint8_t times);
    // void toggle();

    // unsigned int getPinState(int pinNumber);

    // void loop();
};

#endif
