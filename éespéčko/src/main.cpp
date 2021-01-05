#include "Arduino.h"
//#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
//#include <ArduinoJson.h>
#include <NTPClient.h>
//#include <WiFiUdp.h>
//#include <TaskScheduler.h>
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
String inputHodiny;
int inputDavka;
String inputDatum;
int inputIndex = 0;
int porovnaniHod = 1;
int porovnaniMin = 25;
struct tm timeinfo;
char porovnaniCas[8];
int i = 0;

typedef struct
{
  String hodiny;
  int davka;
  String datum;
} DATA;
DATA *data = (DATA *)malloc(100);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void vypis()
{
  int ind = 0;
  for (ind = 0; ind < 4; ind++)
  {
    Serial.println(data[ind].hodiny);
    Serial.println(data[ind].davka);
    Serial.println(data[ind].datum);
  }
}

void ziskani()
{
  server.on("/i", HTTP_POST, [](AsyncWebServerRequest *request) {
    inputIndex = request->arg("i").toInt();
    Serial.println(inputIndex);
    request->send_P(200, "text/plain", "{\"result\":\"ok\"}");
  });
  delay(10);
  server.on("/hodiny", HTTP_POST, [](AsyncWebServerRequest *request) {
    data[inputIndex].hodiny = request->arg("hodiny");
    Serial.println(data[inputIndex].hodiny);
    request->send_P(200, "text/plain", "{\"result\":\"ok\"}");
  });
  server.on("/davka", HTTP_POST, [](AsyncWebServerRequest *request) {
    data[inputIndex].davka = request->arg("davka").toInt();
    Serial.println(data[inputIndex].davka);
    request->send_P(200, "text/plain", "{\"result\":\"ok\"}");
  });
  server.on("/datum", HTTP_POST, [](AsyncWebServerRequest *request) {
    data[inputIndex].datum = request->arg("datum");
    Serial.println(data[inputIndex].datum);
    request->send_P(200, "text/plain", "{\"result\":\"ok\"}");
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
  char den[15];
  String porovnani;
  String denporovnani;
  struct tm timeinfo;
  // Jestliže nebyl získán čas z NTP serveru napíše to hlášku že nebyl získan čas
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Selhání získání času.");
    return;
  }
  // Vypíše získaný čas v celé jeho podobě (den měsíc...)
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  // nakoopíruje mi do mojí proměnné současné hodiny a minuty
  strftime(porovnaniCas, 6, "%H:%M", &timeinfo);
  
  // to samé jako ta předchozí jenom nakopíruje pouze den
  strftime(den, 10, "%A", &timeinfo);
  porovnani = porovnaniCas;
  denporovnani = den;
  Serial.println(denporovnani);
  Serial.println(porovnani);
  int x = 0;
  // standartní funkce for která mi projede celé DATA od hodnoty 0 do hodnoty která je menší nebo rovna indexu.
  for (x = 0; x <= inputIndex; x++)
  {
    //jestliže je den který jsme vložili do web serveru ten stejný který je dnes
    if (denporovnani == data[x].datum)
    {
      // a zároven je i čas zadaný na web serveru ten stejný jako čas současný
      if (porovnani == data[x].hodiny)
      {
        // zapneme funkci motorek a zároven pro neopakování otevření zásobníku je zde nutný delay.
        motorek();
        Serial.println("Motorek jede!\n");
        delay(61000);
      }
    }
  }
}
// Funkce pro pripojeni se k místní wifi síti
void pripojeni()
{
  //vypis připojováni
  Serial.printf("Connecting to %s ", ssid);
  //samotné pripojení pomocí ssid a hesla
  WiFi.begin(ssid, password);
  //dokud nebude pripojeno bude to psát tečky co 0.5s
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  //vypis připojeno
  Serial.println(" CONNECTED");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //vyvolání funkce pro získání času
  getTime();

  Serial.begin(115200);

  // Vyvolání SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

//
//
//
//
//
//
//
//
//
//
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
  //vypis();
  //getTime();
}
