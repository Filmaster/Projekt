#include "Arduino.h"
#include "WiFi.h" 
#include "ESPAsyncWebServer.h" 
#include "SPIFFS.h"
#include "ESP32_MailClient.h"
#include <Keypad.h>
//#include <Wire.h>

const char* ssid = "AP_Dusek";
const char* password = "dusikovi";


AsyncWebServer server(80);

/////////////////////////////////////////

// Set LED GPIO - tohle by mela byt ledka na esp32
const int ledPin = 26;
const int ledPin2=27;
const char* ntpServer = "pool.ntp.org";
String ledState;
String ledState2;
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";
const char* PARAM_INPUT_3 = "input3";
const char* PARAM_INPUT_4 = "input4";
const char* PARAM_INPUT_5 = "input5";
String formattedDate;
String dayStamp;
String timeStamp;


// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "ON"; 
    }
    else{
      ledState = "OFF";
    }
    
    Serial.print(ledState);
    return ledState;
  }

  return String();
}
///////////////////////////


String getTime() {
    struct tm timeinfo;
  String time = (&timeinfo, "%H:%M:%S");
  Serial.println(time);
  return String(time);
}










//uložit do proměné  nebo struct
void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
    Serial.println(&timeinfo, "%H:%M:%S");
  
}


void setup()
{
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
 
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
     //////////////////////////////////////////////////////////////////////////////////////////////Send_Email(); 
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);     
    request->send(SPIFFS, "/index.html", String(), false, processor);
  }); 

/////////////////////////////////////////////////////////////////

 server.on("/hod", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain",getTime().c_str());
  });




///////////////////////////////////////////////////////////////

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    String inputMessage3;
    String inputMessage4;
    String inputMessage5;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage3 = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
    }
     else if (request->hasParam(PARAM_INPUT_4)) {
      inputMessage4 = request->getParam(PARAM_INPUT_4)->value();
      inputParam = PARAM_INPUT_4;
    }
     else if (request->hasParam(PARAM_INPUT_5)) {
      inputMessage5 = request->getParam(PARAM_INPUT_5)->value();
      inputParam = PARAM_INPUT_3;
    }
    else {
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
    request->send(200,  inputParam , inputMessage1);
    request->send(200,  inputParam , inputMessage2);
    request->send(200,  inputParam , inputMessage3);
    
  });

  // Start server
  server.begin();
}
 void loop(){
  delay(1000);
  printLocalTime();
  getTime();
  
 }

  

































































































