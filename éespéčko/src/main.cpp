#include "Arduino.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TaskScheduler.h>
#include <Stepper.h>
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
const int stepsPerRevolution = 2048;




void motorek() {
  Stepper myStepper = Stepper(stepsPerRevolution, 5, 19, 18, 21);
 myStepper.setSpeed(1);
  // Step one revolution in one direction:
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(500);
  // Step one revolution in the other direction:
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(500);
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
    motorek();  
    delay(70000);
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
  //pinMode(in1, OUTPUT);

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

  pripojeni();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/css");
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
