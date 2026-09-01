#include <Arduino.h>
#include <env.h>
#include <pins74.h>
// #include <esp_pins.h>

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <zalcLED.h>
#include <Oled096.h>

zalcLED leds;
Oled096 oled096;

#define dataPin 16 // 6  // пин подключен к входу DS  // DAT_595
#define latchPin 0 // 5 // пин подключен к входу ST_CP // CS_595
#define clockPin 2 // 4 // пин подключен к входу SH_CP // CLK_595

long read_shift_regs();
void print_leds();
void print_leds_state();
void blink_led(int id, int times, int blink_delay);
long get_changed_pin_number(long pinValues, long oldPinValues);

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

const char *currentTime;
const char *oldTime;

// const uint8_t data_pin = 12; // W-Brown 9 pin, Q7 /данные или MISO DataPin 12 /12
// const uint8_t shld_pin = 13; // W-Orange 1 pin, !PL /защелка LoadPin / 13
// const uint8_t clk_pin = 14; // Orange 2 pin, CP /такты или SCK ClockPin / 14
// const uint8_t ce_pin = 15; // W-Blue 15 pin, !CE / чипселект EnablePin / 15

// const uint8_t led_pin10 = 10; //
// const uint8_t led_pin11 = 11; //
// const uint8_t led_pin12 = 12; //
struct SensorData
{
  int id;
  // float value;
  String pins;
  int timeStamp;
};

bool ledState = 0;
const int ledPin = 2; // TODO del it

#define NUMBER_OF_SHIFT_CHIPS 1
#define DATA_WIDTH NUMBER_OF_SHIFT_CHIPS * 8

unsigned long pinValues;
unsigned long oldPinValues;

// LedsStateData array of byte
//  bin2| 10 - status
//-----|----------------------------------------------------------------------------
//  000 |  0 - off / empty    - off
//  001 |  1 - on             - on
//  010 |  2 - error          - blink(200 ms)
//  011 |  3 - ok / inserted  - off -> blinkTimes(300, 3) - off
//  100 |  4 - getReelWaiting - blink(500) -> after pullOut -> blinkTimes(300, 3)
//  101 |  5 - warning        - blink(500 - 1000 ms)

struct LedsStateStatus
{
  int id; // number of row
  // float value;
  byte *state[DATA_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
  u_int8_t state2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int timeStamp;
};

LedsStateStatus ledsStateStatus;
LedsStateStatus oldLedsStateStatus;

// Socket-------------------------------
//  Create AsyncWebServer object on port 80
//  AsyncWebServer server(80);
AsyncWebServer server(3000);
AsyncWebSocket ws("/ws");

void blink(int qtty, int time)
{
  for (int i = 0; i < qtty; i++)
  {
    ledState = !ledState;
    digitalWrite(ledPin, HIGH);
    delay(time);
    ledState = !ledState;
    digitalWrite(ledPin, LOW);
    delay(time);
  }
  digitalWrite(ledPin, !ledState);
  digitalWrite(ledPin, 0);
}

String getPinData()
{
  String msg_str = "";
  for (byte i = 0; i <= DATA_WIDTH - 1; i++)
  {
    Serial.print(pinValues >> i & 1, BIN);
    msg_str += pinValues >> i & 1, BIN;
  }
  return msg_str;
};

void notifyClients()
{
  currentMillis = millis();
  // ws.textAll(String(ledState));
  // ws.textAll(String("pinValues: "));
  ws.textAll(String("pinValues: " + String(pinValues)));
  ws.textAll(String("currentMillis: " + String(currentMillis)));
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
  // SensorData data = {1, String(msg_str), currentMillis};
  //  Cast the struct pointer to uint8_t* and get its size
  //  const size_t capacity = JSON_OBJECT_SIZE(3) + 50; // Adjust capacity as needed
  // JsonDocument doc1;
  //  Add data to the JSON object
  //  doc1["sensor"] = "temperature";
  //  doc1["value"] = 21.5;
  //  doc1["unit"] = String(msg_str);
  //  doc1["millis"] = millis;

  // Serialize the JSON object to a string buffer
  // char output[100]; // Buffer to hold the serialized string
  // char output[100] = serializeJson(doc, Serial); // Buffer to hold the serialized string
  // serializeJson(doc, Serial); // Buffer to hold the serialized string
  // size_t len = serializeJson(doc, wifiClient);
  // ws.textAll(output);
  // ws.sendBIN(clientNum, (uint8_t*)&data, sizeof(data));

  // JsonDocument doc; // fixed size

  // JsonObject          root = doc.to<JsonObject>();
  // root["a"] = "abc";
  //   // ... etc ...

  // char   buffer[200]; // create temp buffer
  // size_t len = serializeJson(root, buffer);  // serialize to buffer

  // ws.textAll(buffer, len); // send buffer to web socket

  JsonDocument doc; // Adjust capacity as needed

  // Create key-value pairs in the JSON document
  doc["sensor"] = "temperature";
  doc["value"] = 2;
  doc["status"] = "ok";
  // doc1["value"] = 21.5;
  doc["pins"] = String(msg_str);
  doc["millis"] = currentMillis;

  // Serialize the JSON object into a String
  char output[256];
  serializeJson(doc, output);

  // Send the String over the WebSocket to all connected clients
  ws.textAll("json  : " + String(output));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;

    Serial.println("");
    Serial.println("data------------------------");
    Serial.println((char *)data);
    Serial.println("data------------------------");

    if (strcmp((char *)data, "toggle") == 0)
    {
      Serial.println("");
      Serial.println("msg------------------------");
      Serial.println("Message received: toggle ");
      Serial.println("msg------------------------");

      if (ledsStateStatus.state2[0] == 0)
      {
        ledsStateStatus.state2[0] = 1;
        ledsStateStatus.state2[3] = 1;
      }
      else
      {
        ledsStateStatus.state2[0] = 0;
        ledsStateStatus.state2[3] = 0;
      }
    }

    if (strcmp((char *)data, "blink") == 0)
    {
      // Serial.println("blink");
      // blink(3, 100);
      Serial.println("");
      Serial.println("msg------------------------");
      Serial.println("Message received: blink ");
      Serial.println("msg------------------------");
      notifyClients();
    }

    if (strcmp((char *)data, "getData") == 0)
    {
      Serial.println("");
      Serial.println("msg------------------------");
      Serial.println("Message received: getData ");
      Serial.println("msg------------------------");
      // blink(3, 100);
      notifyClients();
    }

    if (strcmp((char *)data, "onAll") == 0)
    {
      Serial.println("");
      Serial.println("msg------------------------");
      Serial.println("Message received: onAll ");
      Serial.println("msg------------------------");

      for (int i = 0; i < DATA_WIDTH; i++)
      {
        ledsStateStatus.state2[i] = 1;
      }
    }

    if (strcmp((char *)data, "offAll") == 0)
    {
      Serial.println("");
      Serial.println("msg------------------------");
      Serial.println("Message received: offAll ");
      Serial.println("msg------------------------");

      for (int i = 0; i < DATA_WIDTH; i++)
      {
        ledsStateStatus.state2[i] = 0;
      }
    }

    JsonDocument doc2;

    DeserializationError error = deserializeJson(doc2, data);

    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    const char *sensor = doc2["sensor"];
    long time = doc2["time"];
    double latitude = doc2["data"][0];
    double longitude = doc2["data"][1];

    long id = doc2["id"];
    bool status = doc2["status"];
    long number = doc2["number"];
    JsonArray arr = doc2["arr"].as<JsonArray>();
    // JsonArray arr = doc2["arr"].to<JsonArray>();
    // arr2<int> = doc2["arr"];

    JsonDocument doc3;
    DeserializationError error2 = deserializeJson(doc3, doc2["arr"]);

    // extract the values
    JsonArray array = doc3.as<JsonArray>();
    for (JsonVariant v : array)
    {
      Serial.println("do3---------------------");
      Serial.println(v.as<int>());
      Serial.println("do3+---------------------");
    }
    // Print values.
    Serial.println(sensor);
    Serial.println(time);
    Serial.println(latitude, 6);

    Serial.print("id: ");
    Serial.println(id, 6);

    Serial.print("status: ");
    Serial.println(status, 6);

    Serial.print("number: ");
    Serial.println(number, 6);

    Serial.print("arr: ");
    Serial.println(arr);
    Serial.println("---------------------");

    Serial.println("arr---------------------");
    for (JsonVariant v : arr)
    {
      Serial.println(v.as<int>());
    }
    Serial.println("arr---------------------");

    Serial.println("arr2---------------------");

    int i = 0;
    for (JsonVariant v : arr)
    {
      Serial.println(v.as<int>());
      ledsStateStatus.state2[i] = v.as<int>();
      i++;
    }
    Serial.println("arr2---------------------");

    // for (int i = 0; i < 3; i++)
    // {
    //   Serial.print("arr: ");
    //   Serial.println(arr);
    //   String string_to_print = String(arr[i].c_str());
    //   Serial.println(String(arr[i]));
    // }

    for (JsonVariant v : arr)
    {
      Serial.println(v.as<const char *>());
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
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

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String &var)
{
  Serial.println(var);
  if (var == "STATE")
  {
    if (ledState)
    {
      return "ON";
    }
    else
    {
      return "OFF";
    }
  }
  return String();
}
// Socket-------------------------------

void setup()
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // #define dataPin 6  // пин подключен к входу DS
  // #define latchPin 5 // пин подключен к входу ST_CP
  // #define clockPin 4 // пин подключен к входу SH_CP
  //           6        5          4
  //           16        0         2
  leds.init(dataPin, latchPin, clockPin);

  Wire.begin(5, 4);
  Serial.begin(115200);
  blink(1, 200);

  oled096.init(5, 4);
  leds.loop();

  delay(1000);
  leds.loop();

  for (int i = 0; i < 8; i++)
  {

    leds.on(i);
    leds.loop();
    delay(200);

    leds.off(i);
    leds.loop();
  }
  // END Led test

  int attempts = 0;

  WiFi.begin(ssid, password); // Connect to the network
  while (WiFi.status() != WL_CONNECTED && attempts < 6)
  { // Try for ~3 seconds) { // Wait for the Wi-Fi to connect
    oled096.printWiFi(ssid, password);

    blink(2, 250); // delay(500);
    Serial.print('.');
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nConnected to Network 1!");
  }
  else
  {
    Serial.println("\nFailed to connect to Network 1. Trying Network 2...");
    WiFi.disconnect(); // Disconnect before trying a new network

    oled096.printWiFi(ssid2, password2);

    WiFi.begin(ssid2, password2);
    attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 6)
    { // Try for ~3 seconds
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("\nConnected to Network 2!");
    }
    else
    {
      Serial.println("\nFailed to connect to both networks. Stopping.");
      while (true)
        ; // Stop execution
    }
  }

  do
  {
    delay(500);
    Serial.print('.');
  } while (WiFi.status() != WL_CONNECTED);

  Serial.println('\n');
  Serial.println("Connection established");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  // Get the NTP time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  oled096.printWiFiStatus(WiFi.localIP().toString());

  // 74hc165 shift register
  pinMode(shld_pin, OUTPUT); // LoadPin
  // pinMode(ce_pin, OUTPUT);   // EnablePin
  pinMode(clk_pin, OUTPUT); // ClockPin
  pinMode(data_pin, INPUT); // DataPin

  // pinMode(led_pin10, OUTPUT);//
  // pinMode(led_pin11, OUTPUT);//
  // pinMode(led_pin12, OUTPUT);//

  // выключаем регистр
  digitalWrite(clk_pin, HIGH);
  digitalWrite(shld_pin, HIGH);
  Serial.println("74hc165 test starts");

  // const char [10] msg3 = "-       -";
  char msg3[10] = "-       -";
  oled096.drawPins(msg3);
  delay(1000);
  pinValues = read_shift_regs();

  oled096.printByte(pinValues, DATA_WIDTH);
  oldPinValues = pinValues;

  // Socket setup--------------------------------
  initWebSocket();
  // Start server
  server.begin();
  notifyClients(); // first messsage
  // Socket setup--------------------------------
  delay(1000);
}

void loop()
{
  leds.loop();
  struct tm timeinfo;
  oldTime = "";

  pinValues = read_shift_regs();

  if (pinValues != oldPinValues)
  {
    long changedPinNumber = get_changed_pin_number(pinValues, oldPinValues);
    oled096.printByte(pinValues, DATA_WIDTH);

    if (changedPinNumber >= 0)
    {
      blink_led(changedPinNumber, 3, 100);
      // ledsStateStatus.state2[changedPinNumber] = 5;
    }

    // if (changedPinNumber >= 0 && leds.getPinMode(changedPinNumber) == 0 && leds.getPinState(changedPinNumber) == 0)
    // {
    //   ledsStateStatus.state2[changedPinNumber] = 0;
    // }

    // print_leds();
    // print_leds_state();
    // blink_led(7, 3, 50);
    oldPinValues = pinValues;
    notifyClients();
    // blink(1, 300);
  }

  print_leds_state();

  ws.cleanupClients();
}

long read_shift_regs()
{
  long bitVal;
  unsigned long bytesVal = 0;

  // digitalWrite(ce_pin, HIGH);  // EnablePin
  digitalWrite(shld_pin, LOW); // LoadPin
  delayMicroseconds(5);
  digitalWrite(shld_pin, HIGH); // LoadPin
  // digitalWrite(ce_pin, LOW);    // EnablePin

  for (int i = 0; i < DATA_WIDTH; i++)
  {
    bitVal = digitalRead(data_pin);
    bytesVal |= (bitVal << ((DATA_WIDTH - 1) - i));

    digitalWrite(clk_pin, HIGH);
    delayMicroseconds(5);
    digitalWrite(clk_pin, LOW);
  }

  return (bytesVal);
}

void print_leds()
{
  byte i;

  for (byte i = 0; i <= DATA_WIDTH - 1; i++)
  {
    // reg.write(i, pinValues >> i & 1);
    if (pinValues >> i & 1)
    {
      leds.on(i);
    }
    else
    {
      leds.off(i);
    }
  }
  leds.loop();
  // reg.update();
}

// LedsStateData array of byte
//  bin2| 10 - status
//-----|----------------------------------------------------------------------------
//  000 |  0 - off / empty    - off
//  001 |  1 - on             - on
//  010 |  2 - error          - blink(200 ms)
//  011 |  3 - ok / inserted  - off -> blinkTimes(300, 3) - off
//  100 |  4 - getReelWaiting - blink(500) -> after pullOut -> blinkTimes(300, 3)
//  101 |  5 - warning        - blink(500 - 1000 ms)

void print_leds_state()
{
  byte i;

  for (byte i = 0; i <= DATA_WIDTH - 1; i++)
  {
    if (oldLedsStateStatus.state2[i] != ledsStateStatus.state2[i])
    {
      switch (ledsStateStatus.state2[i])
      {
      case 0: // off / empty
        leds.off(i);
        break;

      case 1: // on
        leds.on(i);
        break;

      case 2: // error
        leds.blink1(i, 50);
        break;

      case 3: // ok / inserted
        leds.blinkTimes(i, 200, 3);
        break;

      case 4: // getReelWaiting
        leds.blink1(i, 500);
        break;

      case 5: // warning
        leds.blink1(i, 1000);
        break;

      default:
        break;
      }
    }

    oled096.drawStatus(i, ledsStateStatus.state2[i]);

    oldLedsStateStatus.state2[i] = ledsStateStatus.state2[i];
  }

  leds.loop();
}

void blink_led(int id, int times, int blink_delay)
{
  byte i;

  leds.off(id);
  leds.loop();

  delay(blink_delay);

  for (byte i = 0; i < times; i++)
  {
    leds.on(id);
    leds.loop();
    delay(blink_delay);

    leds.off(id);
    leds.loop();
    delay(blink_delay);
  }
};

long get_changed_pin_number(long pinValues, long oldPinValues)
{
  long changedPinValues = pinValues ^ oldPinValues;
  byte idx = 0;

  for (byte idx = 0; idx <= DATA_WIDTH - 1; idx++)
  {
    if (changedPinValues >> idx & 1)
    {
      return idx;
    }
  }
  return -1;
};
