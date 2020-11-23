#include "Arduino.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char *ssid = "AP_Dusek";
const char *password = "dusikovi";

AsyncWebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

/////////////////////////////////////////

// Set LED GPIO - tohle by mela byt ledka na esp32
const int in1 =  26;
const int in2 =  25;
const int in3 = 33;
const int in4 = 32;
const char *ntpServer = "pool.ntp.org";
String ledState1;
String ledState2;
String ledState3;
String ledState4;
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
const char *PARAM_INPUT_1 = "input1";
const char *PARAM_INPUT_2 = "input2";
const char *PARAM_INPUT_3 = "input3";
const char *PARAM_INPUT_4 = "input4";
const char *PARAM_INPUT_5 = "input5";
String formattedDate;
String dayStamp;
String timeStamp;


String processor1(const String &var)
{
  Serial.println(var);
  if (var == "STATE")
  {
    if (digitalRead(in1))
    {
      ledState1 = "ON";
      digitalWrite(in1, HIGH);

    }
    else
    {
      ledState1 = "OFF";
      digitalWrite(in1, LOW);

    }

    Serial.print(ledState1);
    return ledState1;
  }

  return String();
}
String processor2(const String &var)
{
  Serial.println(var);
  if (var == "STATE")
  {
    if (digitalRead(in2))
    {
      ledState2 = "ON";
      digitalWrite(in2, HIGH);

    }
    else
    {
      ledState2 = "OFF";
      digitalWrite(in2, LOW);

    }

    Serial.print(ledState2);
    return ledState2;
  }

  return String();
}
String processor3(const String &var)
{
  Serial.println(var);
  if (var == "STATE")
  {
    if (digitalRead(in3))
    {
      ledState1 = "ON";
      digitalWrite(in3, HIGH);

    }
    else
    {
      ledState1 = "OFF";
      digitalWrite(in3, LOW);

    }

    Serial.print(ledState3);
    return ledState3;
  }

  return String();
}
String processor4(const String &var)
{
  Serial.println(var);
  if (var == "STATE")
  {
    if (digitalRead(in4))
    {
      ledState1 = "ON";
      digitalWrite(in4, HIGH);

    }
    else
    {
      ledState1 = "OFF";
      digitalWrite(in4, LOW);

    }

    Serial.print(ledState4);
    return ledState4;
  }

  return String();
}





String getTime()
{
  String time = timeClient.getFormattedTime();
  Serial.println(time);
  return String(time);
}

void setup(){
  Serial.begin(115200);

  //connect to WiFi
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false, processor1);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Route to set GPIO to HIGH
  server.on("/on1", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(in1, HIGH);
    request->send(SPIFFS, "/index.html", String(), false, processor1);
  });

  // Route to set GPIO to LOW
  server.on("/off1", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(in1, LOW);
    request->send(SPIFFS, "/index.html", String(), false, processor1);
  });
  server.on("/on2", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(in2, HIGH);
     ledState2 = "ON";
    request->send(SPIFFS, "/index.html", String(), false, processor2);
  });

  // Route to set GPIO to LOW
  server.on("/off2", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(in2, LOW);
     ledState2 = "OFF";
    request->send(SPIFFS, "/index.html", String(), false, processor2);
  });
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  server.on("/hod", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", getTime().c_str());
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
    Serial.println(inputMessage1);
    Serial.println(inputMessage2);
    Serial.println(inputMessage3);
    Serial.println(inputMessage4);
    Serial.println(inputMessage5);
    request->send(200, inputParam, inputMessage1);
    request->send(200, inputParam, inputMessage2);
    request->send(200, inputParam, inputMessage3);
  });

  // Start server
  server.begin();
}
void loop()
{
  delay(1000);
  //printLocalTime();
  getTime();
}
