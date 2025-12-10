#include <Arduino.h>
// #include <.env.h>

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid     = "ssid";     
const char* password = "password";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C // Address 0x3D for 128x64


// OLED Display 128x64
// SSD1306Wire  display(0x3c, 5, 4);

// #define ssid
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
void setup() {
  Wire.begin(5, 4);
  Serial.begin(115200);
if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  else {
    Serial.println(F("SSD1306 allocation GOOD"));
  }
  delay(2000);
  display.clearDisplay();               // Clear display buffer
  display.setTextSize(1.5);             // Set text size
  display.setTextColor(WHITE);          // Set text color
  display.setCursor(5, 5);              // Define position
  display.println("Hello, Leonid Meow!");     // Display static text
  display.setCursor(5, 18);      
  display.print("ssid: ");  
  display.println(ssid);  
  display.setCursor(5, 31);      
  display.print("pass: ");  
  display.println(password);  
  display.drawRect(0, 0, 128, 43, WHITE);// Draw rectangle
  display.display();                    // Display the text and shape on the screen

  delay(1000);

  WiFi.begin(ssid, password);             // Connect to the network
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.println("Connection established");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); 

  // Get the NTP time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // display.init();
  display.clearDisplay();
  // display.setTextAlignment(TEXT_ALIGN_LEFT);
  // display.setFont(ArialMT_Plain_10);
  display.setCursor(5, 5); 
  display.println("Access Point connected");
  display.setCursor(5, 18); 
  display.println( "AP IP address: ");
  display.setCursor(5, 31); 
  display.println(WiFi.localIP().toString());
  display.setCursor(5, 44); 
  display.setCursor(5, 57); 
  display.display();
  delay(1000);
}
void draw_time(char *msg) {
  display.setCursor(5, 57);
  // display.setTextAlignment(TEXT_ALIGN_CENTER);
  // display.setFont(ArialMT_Plain_24);
  // display.setTextColor(WHITE, BLACK); 
  // display.println("            ");
  // display.display();
  display.setTextColor(WHITE, BLACK); 
  display.setCursor(5, 57);
  display.println(msg);
  display.display();
  // delay(100);

  Serial.println(msg);
}

void loop() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
      char time_str[16];
      strftime(time_str, 16, "%H:%M:%S", &timeinfo);

      draw_time(time_str);
  }  
  delay(1000);
}
