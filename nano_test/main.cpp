// #include <Arduino.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C // Address 0x3D for 128x64
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
void setup() {
  // Wire.begin(5, 4);
  Serial.begin(115200);
  Serial.println(F("SSD1306 allocation GOOD nano_test"));

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;);
    }
    else {
      Serial.println(F("SSD1306 allocation GOOD"));
    }
    delay(2000);
    display.clearDisplay();  
    display.display();             // Clear display buffer
    delay(500);
    display.setTextSize(1.5);             // Set text size
    display.setTextColor(WHITE);          // Set text color
    display.setCursor(5, 5);              // Define position
    display.println("Hello, world!");     // Display static text
    display.display();
    delay(500);
    display.setCursor(5, 18);      
    display.println("Good bye nano test!");  
    display.display();
    delay(500);
    display.drawRect(0, 0, 90, 30, WHITE);// Draw rectangle
    display.display();                    // Display the text and shape on the screen
}

void loop() {
  
}