#include <Arduino.h>
#include "Oled096.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C // Address 0x3D for 128x64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Oled096::Oled096()
{
}

void Oled096::init(int wirePin1Sda, int wirePin2Scl)
{
    Serial.begin(115200);
    // Wire.begin(5, 4);
    Wire.begin(wirePin1Sda, wirePin2Scl);

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    else
    {
        Serial.println(F("SSD1306 allocation GOOD"));
    }

    display.clearDisplay();                 // Clear display buffer
    display.setTextSize(1.5);               // Set text size
    display.setTextColor(WHITE, BLACK);     // Set text color
    display.setCursor(5, 5);                // Define position
    display.println("Hello, Leonid Meow!"); // Display static text
    display.drawRect(0, 0, 128, 43, WHITE); // Draw rectangle
    display.display();                      // Display the text and shape on the screen
}

void Oled096::printWiFi(const char *ssid, const char *password)
{
    display.setCursor(5, 18);
    display.print("ssid: ");
    display.println(ssid);
    display.setCursor(5, 31);
    display.print("pass: ");
    display.println(password);
    display.display();
}
void Oled096::printWiFiStatus(const String ip)
{
    display.clearDisplay();
    display.setCursor(5, 5);
    display.println("Acc Point connected");
    display.setCursor(5, 18);
    display.println("AP IP address: ");
    display.setCursor(5, 28);
    display.println(ip);
    display.setCursor(5, 38);
    display.setCursor(5, 48);
    display.display();
}

void Oled096::drawPins(const char *msg)
{
    display.setCursor(5, 45);
    display.setTextColor(WHITE, BLACK);
    const char *msg2 = "Pins: ";
    display.println(msg);
    display.display();

    Serial.println(msg);
}

void Oled096::printByte(const uint32_t pinValues, int dataWidth)
{
    byte i;

    Serial.println("*Shift Register Values:*\r\n");

    for (byte i = 0; i <= dataWidth - 1; i++)
    {
        Serial.print("P");
        Serial.print(i + 1);
        Serial.print(" ");
    }

    Serial.println();

    for (byte i = 0; i <= dataWidth - 1; i++)
    {
        Serial.print(pinValues >> i & 1, BIN);

        if (i > 8)
        {
            Serial.print(" ");
        }
        Serial.print("  ");
        display.setCursor(5 + i * 10, 45);
        display.setTextColor(WHITE, BLACK);
        const char *msg2 = "Pins: ";
        display.println(pinValues >> i & 1, BIN);
        display.println(" ");
        display.display();
    }

    Serial.println("pinValues");
    Serial.print("Pin1:  ");
    Serial.println(pinValues >> 0 & 1);
    Serial.print("Pin2:  ");
    Serial.println(pinValues >> 1 & 1);
    Serial.print("Pin3:  ");
    Serial.println(pinValues >> 2 & 1);
    char *c = (char *)pinValues;
}
