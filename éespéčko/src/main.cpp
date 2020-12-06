#include "Arduino.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TaskScheduler.h>
#include "time.h"
#define CASOVAC 5000
const char *ssid = "AP_Dusek";
const char *password = "dusikovi";

AsyncWebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
/////////////////////////////////////////

const int in1 = 5;
const int in2 = 18;
const int in3 = 19;
const int in4 = 21;
const char *PARAM_INPUT_1 = "input1";
const char *PARAM_INPUT_2 = "input2";
const char *PARAM_INPUT_3 = "input3";
const char *PARAM_INPUT_4 = "input4";
const char *PARAM_INPUT_5 = "input5";
String formattedDate;
String dayStamp;
String timeStamp;
int casovac = 50000;
Scheduler runner;
String inputCas;
int inputHodiny = 12;
int inputminuty = 12;
int porovnaniHod = 1;
int porovnaniMin = 25;
struct tm timeinfo;
char porovnaniCas[8];

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int rychlost = 1;
int uhel = 180;

void krok1()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(rychlost);
}
void krok2()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(rychlost);
}
void krok3()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(rychlost);
}
void krok4()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(rychlost);
}
void krok5()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(rychlost);
}
void krok6()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, HIGH);
  delay(rychlost);
}
void krok7()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(rychlost);
}
void krok8()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(rychlost);
}

void rotacePoSmeru()
{
  krok1();
  krok2();
  krok3();
  krok4();
  krok5();
  krok6();
  krok7();
  krok8();
}

void rotaceProtiSmeru()
{
  krok8();
  krok7();
  krok6();
  krok5();
  krok4();
  krok3();
  krok2();
  krok1();
}

void rotace()
{
  for (int i = 0; i < (uhel * 64 / 45); i++)
  {
    rotacePoSmeru();
  }

  for (int i = 0; i < (uhel * 64 / 45); i++)
  {
    rotaceProtiSmeru();
  }
  delay(10000);
  // pauza po dobu 1 vteřiny
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////













String getTime()
{
  String time = timeClient.getFormattedTime();
  Serial.println(time);
  return String(time);
}

void printLocalTime()
{
  String porovnani;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  strftime(porovnaniCas, 6, "%H:%M", &timeinfo);
  porovnani = porovnaniCas;
  //strftime(porovnaniMin,3, "%M", &timeinfo);
  // strftime(timeWeekDay,10, "%A", &timeinfo);
  //Serial.println(timeWeekDay);
  Serial.println(porovnani);
  if (inputCas == porovnani)
  {
    rotace();
  }
}
void pripojeni()
{
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  //init and get the time

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  getTime();

  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(in1, OUTPUT);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pripojeni();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css", "/script.js");
  });
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  server.on("/hodiny", HTTP_POST, [](AsyncWebServerRequest *request) {
    inputCas = request->arg("hodiny").toInt();
    request->send_P(200, "text/json", "{\"result\":\"ok\"}");
  });

  server.on("/hod", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", porovnaniCas);
  });

  ///////////////////////////////////////////////////////////////

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    String inputMessage3;
    String inputMessage4;
    String inputMessage5;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1))
    {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2))
    {
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_3))
    {
      inputMessage3 = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
    }
    else if (request->hasParam(PARAM_INPUT_4))
    {
      inputMessage4 = request->getParam(PARAM_INPUT_4)->value();
      inputParam = PARAM_INPUT_4;
    }
    else if (request->hasParam(PARAM_INPUT_5))
    {
      inputMessage5 = request->getParam(PARAM_INPUT_5)->value();
      inputParam = PARAM_INPUT_3;
    }
    else
    {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
      inputMessage3 = "No message sent";
      inputMessage4 = "No message sent";
      inputMessage5 = "No message sent";
      inputParam = "none";
    }
    request->send(200, inputParam, inputMessage1);
    inputCas = inputMessage1;
    Serial.println(inputCas);
  });

  server.begin();
}

void loop()
{
  delay(1000);
  printLocalTime();

  //getTime();
}
