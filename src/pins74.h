//derfault
// const int data_pin = 15; // W-Brown 9 pin, Q7 /данные или MISO DataPin 12 /12
// const int shld_pin = 17; // W-Orange 1 pin, !PL /защелка LoadPin / 13
// const int clk_pin = 18; // Orange 2 pin, CP /такты или SCK ClockPin / 14
// const int ce_pin = 16; // W-Blue 15 pin, !CE / чипселект EnablePin / 15

//ESP32 OLED1
// const int data_pin = 12; //black // W-Brown 9 pin, Q7 /данные или MISO DataPin 12 /12
// const int shld_pin = 13; // yellow // W-Orange 1 pin, !PL /защелка LoadPin / 13
// const int clk_pin = 14; // orange // Orange 2 pin, CP /такты или SCK ClockPin / 14
// const int ce_pin = 15; // grey // W-Blue 15 pin, !CE / чипселект EnablePin / 15


//ESP32 OLED2 --- 2 plate
//1conn GND
const int shld_pin = 14; //2conn Orange //2pin74 yellow // W-Orange 1 pin, !PL /защелка LoadPin / 13
const int clk_pin = 13; //3conn Yellow//1pin74 orange // Orange 2 pin, CP /такты или SCK ClockPin / 14
const int data_pin = 15; //4conn Grey//black // W-Brown 9 pin, Q7 /данные или MISO DataPin 12 /12
const int ce_pin = 12; //5conn Black// grey // W-Blue 15 pin, !CE / чипселект EnablePin / 15
//6pin Vcc +3.3v

//74hc165


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
//Shld Clk
//  |   |   |   |   |   |   |   |
//  2---3---_---_---_---_---_---1 // conn pins
//  Ora Yellow                  Blu