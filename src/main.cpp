#include <Arduino.h>
#include <env.h>

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

long read_shift_regs();
void print_byte();
void draw_pins(char *msg);
// const char* ssid     = "ssid";     
// const char* password = "password";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char* currentTime;
const char* oldTime;

const uint8_t data_pin = 12; // W-Brown 9 pin, Q7 /данные или MISO DataPin 12 /12
const uint8_t shld_pin = 13; // W-Orange 1 pin, !PL /защелка LoadPin / 13
const uint8_t clk_pin = 14; // Orange 2 pin, CP /такты или SCK ClockPin / 14
const uint8_t ce_pin = 15; // W-Blue 15 pin, !CE / чипселект EnablePin / 15
// const uint8_t led_pin10 = 10; // 
// const uint8_t led_pin11 = 11; // 
// const uint8_t led_pin12 = 12; // 

bool ledState = 0;
const int ledPin = 2; //TODO del it

#define NUMBER_OF_SHIFT_CHIPS   1
#define DATA_WIDTH   NUMBER_OF_SHIFT_CHIPS * 8

unsigned long pinValues;
unsigned long oldPinValues;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C // Address 0x3D for 128x64


// OLED Display 128x64
// SSD1306Wire  display(0x3c, 5, 4);

// #define ssid
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Socket-------------------------------
// Create AsyncWebServer object on port 80
// AsyncWebServer server(80);
AsyncWebServer server(3000);
AsyncWebSocket ws("/ws");

void blink(int qtty, int time) {
  for (int i=0; i<qtty; i++) {
    ledState=!ledState;
    digitalWrite(ledPin, HIGH);
    delay(time);
    ledState=!ledState;
    digitalWrite(ledPin, LOW);
    delay(time);
  }
  digitalWrite(ledPin, !ledState);
  digitalWrite(ledPin, 0);
}

String getPinData() {
  String msg_str = "";
  for(byte i=0; i<=DATA_WIDTH-1; i++) 
  { 
    Serial.print(pinValues >> i & 1, BIN); 
    msg_str += pinValues >> i & 1, BIN;
  } 
  return msg_str;
};

void notifyClients() {
  // ws.textAll(String(ledState));
  // ws.textAll(String("pinValues: "));
  ws.textAll(String("pinValues: " + String(pinValues)));
  // ws.textAll(String(pinValues));
  Serial.print("notifyClients pinValues: ");
  Serial.println(pinValues);

  // String msg_str = "";
  // for(byte i=0; i<=DATA_WIDTH-1; i++) 
  // { 
  //   Serial.print(pinValues >> i & 1, BIN); 
  //   msg_str += pinValues >> i & 1, BIN;
  // } 
  String msg_str = getPinData();
  ws.textAll(String("pinValues2: " + String(msg_str)));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
      notifyClients();
    }
    if (strcmp((char*)data, "blink") == 0) {
      // Serial.println("blink");
      // blink(3, 100);
      notifyClients();
    }
    if (strcmp((char*)data, "getData") == 0) {
      // Serial.println("blink");
      // blink(3, 100);
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}
//Socket-------------------------------

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

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
  display.println("Acc Point connected");
  display.setCursor(5, 18); 
  display.println( "AP IP address: ");
  display.setCursor(5, 28); 
  display.println(WiFi.localIP().toString());
  display.setCursor(5, 38); 
  display.setCursor(5, 48); 
  display.display();

  //74hc165 shift register
  pinMode(shld_pin, OUTPUT); //LoadPin
  pinMode(ce_pin, OUTPUT); // EnablePin
  pinMode(clk_pin, OUTPUT);// ClockPin
  pinMode(data_pin, INPUT);  //DataPin

  // pinMode(led_pin10, OUTPUT);// 
  // pinMode(led_pin11, OUTPUT);// 
  // pinMode(led_pin12, OUTPUT);// 

  // выключаем регистр
  digitalWrite(clk_pin, HIGH);
  digitalWrite(shld_pin, HIGH);
  Serial.println("74hc165 test starts");

  // const char [10] msg3 = "-       -";
  char msg3[10] = "-       -";
  draw_pins(msg3);
  delay(1000);
  pinValues = read_shift_regs();
  print_byte();
  oldPinValues = pinValues;
  
  //Socket setup--------------------------------
  initWebSocket();
  // Start server
  server.begin();
  notifyClients(); // first messsage
  //Socket setup--------------------------------
  delay(1000);
}

void draw_time(char *msg) {
  display.setCursor(5, 55);
  // display.setTextAlignment(TEXT_ALIGN_CENTER);
  // display.setFont(ArialMT_Plain_24);
  // display.setTextColor(WHITE, BLACK); 
  // display.println("            ");
  // display.display();
  display.setTextColor(WHITE, BLACK); 
  display.setCursor(5, 55);
  display.println(msg);
  display.display();
  // delay(100);

  Serial.println(msg);
}

void draw_pins(char *msg) {
  display.setCursor(5, 45);
  // display.setTextAlignment(TEXT_ALIGN_CENTER);
  // display.setFont(ArialMT_Plain_24);
  // display.setTextColor(WHITE, BLACK); 
  // display.println("            ");
  // display.display();
  display.setTextColor(WHITE, BLACK); 
  // display.setCursor(5, 45);
  const char* msg2 = "Pins: ";
  display.println(msg);
  display.display();
  // delay(100);

  Serial.println(msg);
}


void loop() {
  struct tm timeinfo;
  oldTime = "";
  // if (getLocalTime(&timeinfo)) {
  //     char time_str[16];
  //     strftime(time_str, 16, "%H:%M:%S", &timeinfo);

  //     if(oldTime != time_str) {
  //       oldTime = time_str;
  //       draw_time(time_str);
  //     }
  // }  
  // draw_pins("11100000");
  pinValues = read_shift_regs();

  if(pinValues != oldPinValues)
  {
      print_byte();
      oldPinValues = pinValues;
      notifyClients();
      blink(3 , 300);
  }

  // print_byte();
  ws.cleanupClients();
  delay(1000);
}

long read_shift_regs()
{
    long bitVal;
    unsigned long bytesVal = 0;

    digitalWrite(ce_pin, HIGH); //EnablePin
    digitalWrite(shld_pin, LOW); //LoadPin
    delayMicroseconds(5);
    digitalWrite(shld_pin, HIGH); //LoadPin
    digitalWrite(ce_pin, LOW); //EnablePin

    for(int i = 0; i < DATA_WIDTH; i++)
    {
        bitVal = digitalRead(data_pin);
        bytesVal |= (bitVal << ((DATA_WIDTH-1) - i));

        digitalWrite(clk_pin, HIGH);
        delayMicroseconds(5);
        digitalWrite(clk_pin, LOW);
    }

    return(bytesVal);
}

void print_byte() { 
  byte i; 

  Serial.println("*Shift Register Values:*\r\n");

  for(byte i=0; i<=DATA_WIDTH-1; i++) 
  { 
    Serial.print("P");
    Serial.print(i+1);
    Serial.print(" "); 
  }
  Serial.println();
  for(byte i=0; i<=DATA_WIDTH-1; i++) 
  { 
    Serial.print(pinValues >> i & 1, BIN); 
    
    if(i>8){Serial.print(" ");}
    Serial.print("  "); 
    display.setCursor(5 + i * 10, 45);
    display.setTextColor(WHITE, BLACK); 
    // display.setCursor(5, 45);
    const char* msg2 = "Pins: ";
    display.println(pinValues >> i & 1, BIN);
    display.println(" ");
    display.display();
  } 
  
  
  Serial.println("pinValues"); 
  Serial.print("Pin1:  "); 
  Serial.println(pinValues >> 0 & 1); 
  // sw_led(led_pin10, pinValues >> 0 & 1);
  Serial.print("Pin2:  "); 
  Serial.println(pinValues >> 1 & 1); 
  // sw_led(led_pin11, pinValues >> 1 & 1);
  Serial.print("Pin3:  "); 
  Serial.println(pinValues >> 2 & 1); 
  // sw_led(led_pin12, pinValues >> 2 & 1);
  char* c = (char*) pinValues;
  // draw_pins(c);
  

}