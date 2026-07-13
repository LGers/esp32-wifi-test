#include <Arduino.h>
#include "ZalcLED.h"

enum ledstate_t
{
    kSwitchOff,
    kSwitchOn,
    kBlinkOn,
    kBlinkOff,
    kBlinkPause,
    kBreathIn,
    kBreathOut
};

enum z_ledmode_t
{
    LED_MODE_ON_OFF = 0,
    LED_MODE_TOGGLE = 2,
    LED_MODE_FADE = 3,
    LED_MODE_BLINK_FOREVER = 4,
    LED_MODE_BLINK_PERIOD = 5,
    LED_MODE_BLINK_NUM_TIME = 6,
};

struct ledState_struct
{
    uint8_t id = 0;
    uint8_t blinkOnTime = 0;
    uint8_t blinkOffTime = 0;
    uint16_t blinkTimePeriod = 300;
    uint8_t blinkNumberOfTimes = 0;
    uint32_t prevMillis = 0;
    bool isLedOn = false;
    z_ledmode_t mode = LED_MODE_ON_OFF;
};

ledState_struct defaultLedState;

ledState_struct ledsState2[8];

zalcLED::zalcLED()
{
    //TODO: custom num of LEDs
    for (int i = 0; i < 8; i++)
    {
        ledsState2[i].id = i;
    }
}

void zalcLED::init(int dataPin1, int latchPin1, int clockPin1)
{
    dataPin = dataPin1;
    latchPin = latchPin1;
    clockPin = clockPin1;

    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    digitalWrite(latchPin, HIGH);
}

void zalcLED::onAll()
{
    for (int i = 0; i < sizeof(ledsState2); i++)
    {
        ledsState2[i].mode = LED_MODE_ON_OFF;
    }
};

void zalcLED::offAll()
{
    for (int i = 0; i < sizeof(ledsState2); i++)
    {
        ledsState2[i].isLedOn = false;
    }
};

void zalcLED::on(int ledNumber)
{
    onBit(ledNumber - 1);
    ledsState2[ledNumber - 1].isLedOn = true;
    ledsState2[ledNumber - 1].mode = LED_MODE_ON_OFF;
};

void zalcLED::off(int ledNumber)
{
    offBit(ledNumber - 1);
    ledsState2[ledNumber - 1].isLedOn = false;
    ledsState2[ledNumber - 1].mode = LED_MODE_ON_OFF;
};

void zalcLED::blink1(int ledNumber)
{
    ledsState2[ledNumber - 1].mode = LED_MODE_BLINK_FOREVER;
};

void zalcLED::blink1(int ledNumber, uint16_t ledBlinkDelay)
{
    ledsState2[ledNumber - 1].mode = LED_MODE_BLINK_FOREVER;
    ledsState2[ledNumber - 1].blinkTimePeriod = ledBlinkDelay;
};

void zalcLED::blinkTimes(int ledNumber, uint16_t ledBlinkDelay, uint8_t times)
{
    ledsState2[ledNumber - 1].mode = LED_MODE_BLINK_NUM_TIME;
    ledsState2[ledNumber - 1].blinkTimePeriod = ledBlinkDelay;
    ledsState2[ledNumber - 1].blinkNumberOfTimes = times;
};

void zalcLED::toggle() {

};

unsigned int zalcLED::getPinState(int pinNumber)
{
    return ledsState2[pinNumber - 1].isLedOn;
};

// LOOP----------------------------------
void zalcLED::loop()
{
    currentMillis = millis();
    if (ledsPinCurrentState != ledsPinLastState)
    {
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, LSBFIRST, ledsPinLastState);
        digitalWrite(latchPin, HIGH);
    }

    unsigned int newLeds = 0;
    unsigned int length = sizeof(ledsState2) / sizeof(ledsState2[0]);

    for (unsigned int i = 0; i < length; i++)
    {
        newLeds <<= 1;

        switch (ledsState2[i].mode)
        {
        case LED_MODE_ON_OFF:
            break;

        case LED_MODE_BLINK_FOREVER:
            if ((currentMillis - ledsState2[i].prevMillis) >= ledsState2[i].blinkTimePeriod)
            {
                ledsState2[i].isLedOn = !ledsState2[i].isLedOn;
            }
            break;

        case LED_MODE_BLINK_NUM_TIME:
            if ((currentMillis - ledsState2[i].prevMillis) >= ledsState2[i].blinkTimePeriod)
            {
                ledsState2[i].isLedOn = !ledsState2[i].isLedOn;

                if (ledsState2[i].blinkNumberOfTimes > 0)
                {
                    if (ledsState2[i].isLedOn)
                    {
                        ledsState2[i].blinkNumberOfTimes -= 1;
                    }
                }
                else // switch LED to default state
                {
                    switchToDefaultLedState(i);
                }
            }
            break;

        default: // LED_MODE_OFF
            switchToDefaultLedState(i);
            break;
        }

        if (ledsState2[i].isLedOn)
        {
            newLeds = newLeds | 0b00000001;
        }
        else
        {
            newLeds &= 0b11111110;
        }

        if ((currentMillis - ledsState2[i].prevMillis) >= ledsState2[i].blinkTimePeriod)
        {
            ledsState2[i].prevMillis = currentMillis;
        }
    }

    ledsPinCurrentState = newLeds;
    if (newLeds != ledsPinLastState)
    {
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, LSBFIRST, newLeds);
        digitalWrite(latchPin, HIGH);
        ledsPinLastState = newLeds;
    }

    if ((currentMillis - prevMillis) >= blinkDelay)
    {
        prevMillis = currentMillis;
    }
};

// PRIVATE---------------------
void zalcLED::onBit(uint32_t bitNumber)
{
    changetBit = (1 << (m_led_counts - 1 - bitNumber)); // Установили bitNumber-й (3) бит: 00001000 (8)
    leds = leds | changetBit;
    ledsPinCurrentState = ledsPinCurrentState | changetBit;
}

void zalcLED::offBit(uint32_t bitNumber)
{
    changetBit = (1 << (m_led_counts - 1 - bitNumber)); // Установили bitNumber-й (3) бит: 00001000 (8)
    leds = leds & ~changetBit;
    ledsPinCurrentState = ledsPinCurrentState & ~changetBit;
    // leds ^= (1 << 7 - bitNumber);  // Установили bitNumber-й бит: 00001000 (8)
}

bool zalcLED::isLedOn(uint32_t ledNumber)
{
    uint8_t shiftedByte = ledsPinLastState >> (m_led_counts - 1 - ledNumber);
    return (shiftedByte & 1) != 0;
}

char zalcLED::getBits(uint32_t n)
{
    char str;

    for (uint8_t i = 1; i <= sizeof(n); i++)
    {
        int bit = (n >> i) & 1; // Сдвигаем и проверяем последний бит
        str += bit;
    }
    return str;
}

void zalcLED::printBits(uint32_t n)
{

    Serial.print("Bit is: ");
    for (int i = sizeof(n); i > 0; i--)
    {
        int bit = (n >> i) & 1; // Сдвигаем и проверяем последний бит
        Serial.print(bit);
    }
    Serial.println(" :END");
}

void zalcLED::switchToDefaultLedState(uint32_t ledNumber)
{
    ledsState2[ledNumber] = defaultLedState;
}
