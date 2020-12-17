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
String formattedDate;
String dayStamp;
String timeStamp;
int casovac = 50000;
Scheduler runner;
String inputHodiny;
int inputDavka;
String inputDatum;
int porovnaniHod = 1;
int porovnaniMin = 25;
struct tm timeinfo;
char porovnaniCas[8];

typedef struct
{
  String hodiny;
  int davka;
  String datum;
} DATA;
DATA *data = NULL;

void zapis()
{
  int i;
  data[1].hodiny = inputHodiny;
  data[1].hodiny = inputDavka;
  data[1].hodiny = inputDatum;
  Serial.println(data[1].hodiny);
  Serial.println(data[1].hodiny);
  Serial.println(data[1].hodiny);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ziskani()
{
  server.on("/hodiny", HTTP_POST, [](AsyncWebServerRequest *request) {
    inputHodiny = request->arg("hodiny");
    Serial.println(inputHodiny);
    request->send_P(200, "text/json", "{\"result\":\"ok\"}");
  });
  server.on("/davka", HTTP_POST, [](AsyncWebServerRequest *request) {
    inputDavka = request->arg("davka").toInt();
    Serial.println(inputDavka);
    request->send_P(200, "text/json", "{\"result\":\"ok\"}");
  });
  server.on("/datum", HTTP_POST, [](AsyncWebServerRequest *request) {
    inputDatum = request->arg("datum");
    Serial.println(inputDatum);
    request->send_P(200, "text/json", "{\"result\":\"ok\"}");
  });
}
const int stepsPerRevolution = 1024; //2048;
void motorek()
{
  Stepper myStepper = Stepper(stepsPerRevolution, 5, 19, 18, 21);
  myStepper.setSpeed(10);
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
void spifs()
{
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/client.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/client.js", "text/css");
  });
}

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
  spifs();
  server.on("/hod", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", porovnaniCas);
  });
  ziskani();
  server.begin();
}

void loop()
{
  delay(1000);
  printLocalTime();
  //Serial.println(inputHodiny);
  // Serial.println(inputDavka);
  //  Serial.println(inputDatum);

  //getTime();
}
