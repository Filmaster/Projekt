#include "Arduino.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <NTPClient.h>
#include <Stepper.h>
#include "time.h"
#include <EEPROM.h>
#define CASOVAC 5000
const char *ssid = "AP_Dusek";
const char *password = "dusikovi";
AsyncWebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
String inputHodiny;
int inputDavka;
String inputDatum;
int inputIndex = 0;
int porovnaniHod = 1;
int porovnaniMin = 25;
struct tm timeinfo;
char porovnaniCas[8];
long previousMillis = 0;
long interval = 1000;   
int pokus;
int addr = 0;

  String denP;
  String casP;

typedef struct
{
  String hodiny;
  int davka;
  String datum;
} DATA;
DATA *data = (DATA *)malloc(100);
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

void eeprom(){
pokus =25;
EEPROM.put(addr, data);
}


void motorek(int pocetO, int pauza)
{
  const int stepsPerRevolution = 2048; //2048;
  Stepper myStepper = Stepper(stepsPerRevolution, 5, 19, 18, 21);
  myStepper.setSpeed(20);
  // Step one revolution in one direction:
  /*Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  */
  //delay(500);


  // Step one revolution in the other direction:
  for(int i=0;i<=pocetO;i++){
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  }

}
void web()
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
    server.on("/hod", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", porovnaniCas);
  });
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

String getTime()
{
  String time = timeClient.getFormattedTime();
  Serial.println(time);
  return String(time);
}

void printLocalTime()
{
  char den[15];
  String porovnaniC;
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
  porovnaniC = porovnaniCas;
  denporovnani = den;
  denP=denporovnani;
  casP=porovnaniC;
  Serial.println(denporovnani);
  Serial.println(porovnaniC);
}


void porovnani( String den, String cas){
    int x = 0;
  // standartní funkce for která mi projede celé DATA od hodnoty 0 do hodnoty která je menší nebo rovna indexu.
  for (x = 0; x <= inputIndex+1; x++)
  {
    //jestliže je den který jsme vložili do web serveru ten stejný který je dnes
    if (den == data[x].datum)
    {
      // a zároven je i čas zadaný na web serveru ten stejný jako čas současný
      if (cas == data[x].hodiny)
      {
switch (data[inputIndex].davka)
        {
          int pauza;
          int otacky;
        case 1:
          pauza = 0;
          otacky = 5;
          motorek(otacky, pauza);
          Serial.println("Motorek jede!  1\n");
          break;
        case 2:
          pauza = 0;
          otacky = 10;
          motorek(otacky, pauza);
          Serial.println("Motorek jede!  2\n");
          break;
        case 3:
          pauza = 0;
          otacky = 15;
          motorek(otacky, pauza);
          Serial.println("Motorek jede!  3\n");
          break;
        case 4:
          pauza = 0;
          otacky = 20;
          motorek(otacky, pauza);
          Serial.println("Motorek jede!  4\n");
          break;
        case 5:
          pauza = 0;
          otacky = 25;
          motorek(otacky, pauza);
          Serial.println("Motorek jede!  5\n");
          break;
        }
        delay(1000);
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
  web();
  server.begin();

eeprom();
  EEPROM.get(addr, data);
  Serial.println(data[1].hodiny);
}

void loop()
{
  porovnani(denP,casP);
  //delay(100);
unsigned long currentMillis = millis();
 if(currentMillis - previousMillis > interval) { 
    previousMillis = currentMillis;   
  printLocalTime();
  //vypis();
  //getTime();
 }
}
//ntp server zvolit
// okomentovat server on
// každy server on bude v jedne funkci
// uložit do souboru



/*
        switch (data[inputIndex].davka)
        {
          int pauza;
          int rychlost;
        case 1:
          pauza = 0;
          rychlost = 10;
          //motorek(rychlost, pauza);
          Serial.println("Motorek jede!  1\n");
          break;
        case 2:
          pauza = 0;
          rychlost = 8;
         // motorek(rychlost, pauza);
          Serial.println("Motorek jede!  2\n");
          break;
        case 3:
          pauza = 0;
          rychlost = 8;
          //motorek(rychlost, pauza);
          Serial.println("Motorek jede!  3\n");
          break;
        case 4:
          pauza = 0;
          rychlost = 8;
         // motorek(rychlost, pauza);
          Serial.println("Motorek jede!  4\n");
          break;
        case 5:
          pauza = 0;
          rychlost = 8;
         // motorek(rychlost, pauza);
          Serial.println("Motorek jede!  5\n");
          break;
        }
*/