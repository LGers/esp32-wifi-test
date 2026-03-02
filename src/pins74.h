// derfault / esp32 on shield + oled + 74hc165
 const int data_pin = 16; // yellow       / W-Brown 9 pin, Q7   /данные или MISO DataPin 12 /12
 const int shld_pin = 18; // light-orange / W-Orange 1 pin, !PL /защелка LoadPin / 13
 const int clk_pin = 17; // orange        / Orange 2 pin, CP    /такты или SCK ClockPin / 14
 const int ce_pin = 15; // grey           / W-Blue 15 pin, !CE  / чипселект EnablePin / 15

#define CLK_595 13  // W-ORA   // YELLOW     //Pin 74hc595 --- 11
#define CS_595 12   // W-GREEN // GREEN      //Pin 74hc595 --- 12
#define DAT_595 14 // W-BLUE  // TRANPARENT //Pin 74hc595 --- 14

// ESP32 OLED1 __ one plate with ESP32 & oled 0.96
// ESP32 pins TOP SUN - 16 -  5  -   4  -   0  -   2   -  14  -  12  - 13  - 15 - RX - TX - 3V3  -  GND  - GND  - 5V
// ESP32 pins TOP  - (TRANS)  OLED   OLED (GREEN) (YEL)  |ORA   |BLK  |YEL  |GRE            |RED    |BLU    NC    NC
// Pin 74hc595         14                  12       11

// const int data_pin = 12; // black // W-Brown 9 pin, Q7 /данные или MISO DataPin 12 /12
// const int shld_pin = 13; // yellow // W-Orange 1 pin, !PL /защелка LoadPin / 13
// const int clk_pin = 14;  // orange // Orange 2 pin, CP /такты или SCK ClockPin / 14
// const int ce_pin = 15;   // grey // W-Blue 15 pin, !CE / чипселект EnablePin / 15
// orange

// #define CLK_595 2  // YELLOW //Pin 74hc595 --- 11
// #define CS_595 0   // GREEN //Pin 74hc595 --- 12
// #define DAT_595 16 // TRANPARENT //Pin 74hc595 --- 14

// ESP32 OLED2 --- 2nd plate ESP + Oled - one Board
// 1conn GND
//  const int shld_pin = 14; //2conn Orange //2pin74 yellow // W-Orange 1 pin, !PL /защелка LoadPin / 13
//  const int clk_pin = 13; //3conn Yellow//1pin74 orange // Orange 2 pin, CP /такты или SCK ClockPin / 14
//  const int data_pin = 15; //4conn Grey//black // W-Brown 9 pin, Q7 /данные или MISO DataPin 12 /12
//  const int ce_pin = 12; //5conn Black// grey // W-Blue 15 pin, !CE / чипселект EnablePin / 15
// 6pin Vcc +3.3v

// ESP32 OLED3 box shield --- 3 plate
// 1conn GND
//  const int shld_pin = 18; //2conn Orange //2pin74 yellow // W-Orange 1 pin, !PL /защелка LoadPin / 13
//  const int clk_pin = 17; //3conn Yellow//1pin74 orange // Orange 2 pin, CP /такты или SCK ClockPin / 14
//  const int data_pin = 16; //4conn Grey//black // W-Brown 9 pin, Q7 /данные или MISO DataPin 12 /12
//  const int ce_pin = 15; //5conn Black// grey // W-Blue 15 pin, !CE / чипселект EnablePin / 15
// 6pin Vcc +3.3v
// 74hc165

// ESP connector

// Red Gray                     Black
//  6   5                       6 // conn pins
//      ce                      Data
// Vcc !CE  D3  D2  D1  D0      Q DataPin
//  |   |   |   |   |   |   |   |
//  16  15  14  13  12  11  10  9
//
//
//  1   2   3   4   5   6   7   8
//  |   |   |   |   |   |   |   |
// PL  CP   D4  D5  D6  D7  !Q  GND
// Shld Clk
//  |   |   |   |   |   |   |   |
//  2---3---_---_---_---_---_---1 // conn pins
//  Ora Yellow                  Blu