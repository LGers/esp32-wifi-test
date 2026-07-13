#pragma once
#include <Arduino.h> //TODO: pragma once

#ifndef zalcLED_H
#define zalcLED_H

// #define ledState_struct ledsState2[8];

// #define struct ledState_struct
// {
//     uint8_t id = 0;
//     uint8_t blinkOnTime = 0;
//     uint8_t blinkOffTime = 0;
//     uint16_t blinkTimePeriod = 300;
//     uint8_t blinkNumberOfTimes = 0;
//     uint32_t prevMillis = 0;
//     bool isLedOn = false;
//     z_ledmode_t mode = LED_MODE_ON_OFF;
// };

class zalcLED
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

    void onBit(uint32_t bitNumber);
    void offBit(uint32_t bitNumber);
    bool isLedOn(uint32_t ledNumber);
    char getBits(uint32_t n);
    void printBits(uint32_t n);
    void fn(uint32_t n);
    void intFn(uint32_t n);
    void switchToDefaultLedState(uint32_t n);

public:
    int dataPin;
    int latchPin;
    int clockPin;
    int leds = B00000000;

    zalcLED(); // Конструктор
    // void printData(); // Метод вывода
    // int multi2(); // Метод вывода

    void init(int dataPin1, int latchPin1, int clockPin1);
    void onAll();
    void offAll();
    void on(int ledNumber);
    void off(int ledNumber);

    void blink1(int ledNumber);
    void blink1(int ledNumber, uint16_t ledBlinkDelay);
    void blinkTimes(int ledNumber, uint16_t ledBlinkDelay, uint8_t times);
    void toggle();

    unsigned int getPinState(int pinNumber);

    void loop();
};

#endif