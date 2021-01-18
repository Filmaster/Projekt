#include "Arduino.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <NTPClient.h>
#include <Stepper.h>
#include "time.h"
#include <EEPROM.h>
#define CASOVAC 5000
// proměnné pro přihlášení se wifi
const char *ssid = "AP_Dusek";
const char *password = "dusikovi";
AsyncWebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


//proměnné pro čas
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

//proměnné pro nahrazení delay
long previousMillis = 0;
long interval = 1000;

//pokus
int pokus;
int addr = 0;

// hodnoty pro porovnání
String denP;
String casP;
String sekP;

typedef struct
{
  String hodiny;
  int davka;
  String datum;
} DATA;
DATA *data = (DATA *)malloc(100);

/*
void eeprom()
{
  pokus = 25;
  EEPROM.put(addr, data);
}
*/
void motorek(int otacky)
{
  int y=0;
  const int stepsPerRevolution = 2048; //2048;
  Stepper myStepper = Stepper(stepsPerRevolution, 5, 19, 18, 21);
  myStepper.setSpeed(20);

  // otáčení se po směru hodinových ručiček
  //Serial.println("po směru ručiček");
  //myStepper.step(stepsPerRevolution);
for(y=0;y<otacky;y++){
  // otáčení se proti směru hodinových ručiček
  Serial.println("proti směru ručiček");
  myStepper.step(-stepsPerRevolution);
}
}
void web()
{
  //  Cesta k webové aplikaci konkrétně k index.html    Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  //  Cesta k webové aplikaci konkrétně k style.css
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  //  Cesta k webové aplikaci konkrétně k client.js
  server.on("/client.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/client.js", "text/css");
  });
  //posílání proměnné ve které je obsažen současný čas na web server
  server.on("/hod", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", porovnaniCas);
  });
  // získání indexu z web serveru slouží k orientaci typedef struct
  server.on("/i", HTTP_POST, [](AsyncWebServerRequest *request) {
    inputIndex = request->arg("i").toInt() + 1;
    Serial.println(inputIndex);
    request->send_P(200, "text/plain", "{\"result\":\"ok\"}");
  });

  delay(10);
  // získání času z webového formuláře
  server.on("/hodiny", HTTP_POST, [](AsyncWebServerRequest *request) {
    data[inputIndex].hodiny = request->arg("hodiny");
    Serial.println(data[inputIndex].hodiny);
    request->send_P(200, "text/plain", "{\"result\":\"ok\"}");
  });
  //získání dávky z webového formuláře
  server.on("/davka", HTTP_POST, [](AsyncWebServerRequest *request) {
    data[inputIndex].davka = request->arg("davka").toInt();
    Serial.println(data[inputIndex].davka);
    request->send_P(200, "text/plain", "{\"result\":\"ok\"}");
  });
  //získání datumu z webového formuláře
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
//Funkce pro práci s časem
void printLocalTime()
{
  char sek[4];
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
  strftime(sek, 3, "%S", &timeinfo);

  // to samé jako ta předchozí jenom nakopíruje pouze den
  strftime(den, 10, "%A", &timeinfo);
  porovnaniC = porovnaniCas;
  denporovnani = den;
  denP = denporovnani;
  casP = porovnaniC;

  sekP = sek;
  Serial.println(sekP);
}

void porovnani(String den, String cas)
{
  int x = 0;
  // standartní funkce for která mi projede celé DATA od hodnoty 0 do hodnoty která je menší nebo rovna indexu.
  for (x = 0; x <= inputIndex + 1; x++)
  {
    //jestliže je den který jsme vložili do web serveru ten stejný který je dnes
    if (den == data[x].datum && sekP == "01")
    {
      // a zároven je i čas zadaný na web serveru ten stejný jako čas současný
      if (cas == data[x].hodiny)
      {
        //nahrazuje cykly if a else if  přijde mi jako elegantější řešení
       switch (data[inputIndex].davka)
        {
          int otacky;
          //jestliže dávka je rovna 1 otacky se nastavi na 5 a poslou se do funkce motorek
        case 1:
          otacky = 5;
          motorek(otacky);
          Serial.println("Motorek jede!  1\n");
          break;
        case 2:
          otacky = 10;
          motorek(otacky);
          Serial.println("Motorek jede!  2\n");
          break;
        case 3:
          otacky = 15;
          motorek(otacky);
          Serial.println("Motorek jede!  3\n");
          break;
        case 4:
          otacky = 20;
          motorek(otacky);
          Serial.println("Motorek jede!  4\n");
          break;
        case 5:
          otacky = 25;
          motorek(otacky);
          Serial.println("Motorek jede!  5\n");
          break;
        }
        //delay(1000);
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




void setup()
{
  Serial.begin(115200);
  pripojeni();
  web();
  server.begin();

  //eeprom();
  //EEPROM.get(addr, data);
  // Serial.println(data[1].hodiny);
}

void loop()
{
  porovnani(denP, casP);
  //delay(100);
  //nahrazení delay. milis() počítá milisekundy od začátku programu, které potom porovná s mikisekundama uloženýma
  //při posledním provedení a pokud jsou větší než mnou nastavený interval pak se funkce provede.
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  {
    previousMillis = currentMillis;
    printLocalTime();
    //vypis();
    //getTime();
  }
}