#include <Arduino.h>

//--esp--breadbord
const int data_pin = 15;// 12; // W-Brown 9 pin, Q7 /данные или MISO DataPin 12 /12
const int shld_pin = 17;// 13; // W-Orange 1 pin, !PL /защелка LoadPin / 13
const int clk_pin = 18;// 14; // Orange 2 pin, CP /такты или SCK ClockPin / 14
const int ce_pin = 16;// 15; // W-Blue 15 pin, !CE / чипселект EnablePin / 15

//---esp-oled
// const int data_pin = 12; // W-Brown 9 pin, Q7 /данные или MISO DataPin 12 /12
// const int shld_pin = 13; // W-Orange 1 pin, !PL /защелка LoadPin / 13
// const int clk_pin = 14; // Orange 2 pin, CP /такты или SCK ClockPin / 14
// const int ce_pin = 15; // W-Blue 15 pin, !CE / чипселект EnablePin / 15

class Foo {
  public:
    int var;        // переменная (свойство)
    int dataPin;
    int latchPin;
    int clockPin;

    void func() {   // функция (метод)
    }

    void init(int dataPin, int latchPin, int clockPin) {
      latchPin = latchPin;
      pinMode(latchPin, OUTPUT);
      pinMode(clockPin, OUTPUT);
      pinMode(dataPin, OUTPUT);
      // ставим HIGH на "защёлку", чтобы регистр не принимал сигнал
      digitalWrite(latchPin, HIGH);
    }

    void onAll() {   // on all leds
      digitalWrite(latchPin, LOW);                                        // ставим LOW на "защёлку"
      shiftOut(dataPin, clockPin, LSBFIRST, B11111111); // отправляем байт в двоичном виде
      digitalWrite(latchPin, HIGH);                                       // ставим HIGH на "защёлку"
    }

    void offAll() {   // off all leds
      digitalWrite(latchPin, LOW);                                        // ставим LOW на "защёлку"
      shiftOut(dataPin, clockPin, LSBFIRST, B00000000); // отправляем байт в двоичном виде
      digitalWrite(latchPin, HIGH);                                       // ставим HIGH на "защёлку"
    }

    void on(int ledNumber) {   // on led with ledNumber
    }

    void off(int ledNumber) {   // off led with ledNumber
    }

  private:
    // системный код

};
