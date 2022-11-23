// Board: NodeMCU-32S
//.https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
// https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/
/*
  Rui Santos
  Complete project details
   - Arduino IDE: https://RandomNerdTutorials.com/esp32-ota-over-the-air-arduino/
   - VS Code: https://RandomNerdTutorials.com/esp32-ota-over-the-air-vs-code/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
// https://m1cr0lab-esp32.github.io/remote-control-with-websocket/websocket-data-exchange/
// https://techtutorialsx.com/2019/03/04/esp32-arduino-serving-bootstrap/

// Import required libraries
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include <Arduino_JSON.h>
#include <AsyncElegantOTA.h>

#include <WiFi.h>
#include "time.h"

// include library to read and write from flash memory
#include <Preferences.h>
//#include <nvs_flash.h>

#define RXD2 16
#define TXD2 17
#define SERIAL_BAUD 115200

// Overige modes gaan hier dwars doorheen naar de klok, alleen opnemen indien nodig
#define MODE_CLOCK 1
#define MODE_TEST 10
#define MODE_SYNC 8

// Allocate the JSON document
// This one must be bigger than for the sender because it must store the strings
// https://arduinojson.org/v6/api/staticjsondocument/
StaticJsonDocument<600> docIn;
StaticJsonDocument<600> docUit;
StaticJsonDocument<100> mqtt;
DeserializationError err;
 
Preferences preferences;

// Replace with your network credentials
// https://techtutorialsx.com/2021/01/04/esp32-soft-ap-and-station-modes/amp/
const char* soft_ap_ssid     = "Wordclock-Access-Point2";
const char* soft_ap_password = "123456789";

String wifi_network_ssid = "";
String wifi_network_password =  "";

String inputSSID;
String inputWachtwoord;
IPAddress ipadres;      

unsigned long _next_1000_ms;
unsigned long _millis;

long  gmtOffset_sec = 3600;
String ntpServer = "europe.pool.ntp.org";

uint8_t minBrightness = 12;
uint8_t maxBrightness = 255;
uint8_t stepBrightness = 30; //in seconden
uint8_t startBrightness = 60; //gerekend in minuten na zonsondergang

uint8_t bRood = 255; 
uint8_t bGroen = 255; 
uint8_t bBlauw = 255; 

uint8_t bSRood = 255; 
uint8_t bSGroen = 0; 
uint8_t bSBlauw = 0; 

float longitude = 5.69;
float latitude = 53.03;

// nachtstand
uint8_t uuruit = 23;
uint8_t uuraan = 7;
uint8_t nachtsterkte = 12;

// wordclock
String wordclock = "secaan";
// effecten
String effect = "geeneffect";

uint8_t maan = true; 
uint8_t desOchtends = false; 
uint8_t hetIs = true;
 
uint8_t autoZomertijd = true; 

//String startDimmen;
//String eindeDimmen;
//String startOpdraaien;
//String eindeOpdraaien;

int jaar;
int maand;
int datum;
int uur;
int minuut;
int seconde;

AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

uint8_t mode = MODE_CLOCK;

float temperatuur;
float druk;
String zonop;
String zononder;
uint8_t stap;
uint8_t sterkte;
String maanfase; 

void eeprom_save() {

  // https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/

  preferences.begin("settings", false);
  preferences.putUChar("mode", mode); 
  preferences.putUChar("startBr", startBrightness);
  preferences.putUChar("stepBr", stepBrightness);
  preferences.putUChar("minBr", minBrightness);
  preferences.putUChar("maxBr", maxBrightness);
  preferences.putUChar("bRood", bRood);
  preferences.putUChar("bGroen", bGroen);
  preferences.putUChar("bBlauw", bBlauw);
  preferences.putUChar("bSRood", bSRood);
  preferences.putUChar("bSGroen", bSGroen);
  preferences.putUChar("bSBlauw", bSBlauw);
  preferences.putFloat("lengte", longitude);
  preferences.putFloat("breedte", latitude);

  preferences.putUChar("uuruit", uuruit);
  preferences.putUChar("uuraan", uuraan);
  preferences.putUChar("nachtsterkte", nachtsterkte);
  
  preferences.putString("wordclock", wordclock);
  preferences.putString("effect", effect);
  
  preferences.putUChar("maan", maan);
  preferences.putUChar("desOchtends", desOchtends);
  preferences.putUChar("hetIs", hetIs);

  preferences.putString("ntpServer", ntpServer);
  preferences.putInt("gmtOffset_sec", gmtOffset_sec);
  preferences.putUChar("autoZomertijd", autoZomertijd);

  preferences.end();
    
  preferences.begin("credentials", false);
  preferences.putString("ssid", wifi_network_ssid); 
  preferences.putString("password", wifi_network_password);
  preferences.end();

}

void eeprom_retrieve() {

  preferences.begin("settings", false);
  mode = preferences.getUChar("mode", MODE_CLOCK); 
  startBrightness = preferences.getUChar("startBr", 0);
  stepBrightness = preferences.getUChar("stepBr", 15);
  minBrightness = preferences.getUChar("minBr", 20);
  maxBrightness = preferences.getUChar("maxBr", 255);
  bRood = preferences.getUChar("bRood", 255);
  bGroen = preferences.getUChar("bGroen", 255);
  bBlauw = preferences.getUChar("bBlauw", 255);
  bSRood = preferences.getUChar("bSRood", 255);
  bSGroen = preferences.getUChar("bSGroen", 255);
  bSBlauw = preferences.getUChar("bSBlauw", 255);
  longitude = preferences.getFloat("lengte", 5.69);
  latitude = preferences.getFloat("breedte", 53.03);
  
  uuruit = preferences.getUChar("uuruit", 23);
  uuraan = preferences.getUChar("uuraan", 7);
  nachtsterkte = preferences.getUChar("nachtsterkte", 12);
  
  wordclock = preferences.getString("wordclock", "secaan");
  effect = preferences.getString("effect", "geeneffect");
  
  maan = preferences.getUChar("maan", true);
  desOchtends = preferences.getUChar("desOchtends", false);
  hetIs = preferences.getUChar("hetIs", true);

  ntpServer = preferences.getString("ntpServer", "europe.pool.ntp.org");
  gmtOffset_sec = preferences.getInt("gmtOffset_sec", 3600);
  autoZomertijd = preferences.getUChar("autoZomertijd", true);

  preferences.end();  

  preferences.begin("credentials", false);
  wifi_network_ssid = preferences.getString("ssid", ""); 
  wifi_network_password = preferences.getString("password", "");
  preferences.end();
}

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

String voorloopNul(int j){
  String s = "";
  if(j < 10) s = "0";
  s = s + String(j);
  return s;
}

String getStatus(){

  // Deze wordt elke seconde naar de webclients verstuurd
  JSONVar myArray;

  String tijdNu = String(jaar) + "-" + voorloopNul(maand) + "-" + voorloopNul(datum) + " " + voorloopNul(uur) + ":" + voorloopNul(minuut) + ":" + voorloopNul(seconde);
  myArray["tijd"] = String(tijdNu);
  
  myArray["temperatuur"] = String(temperatuur);
  myArray["druk"] = String(druk);
        
  myArray["zonop"] = zonop;
  myArray["zononder"] = zononder;
        
  myArray["stap"] = stap;
  myArray["sterkte"] = sterkte;
  myArray["maanfase"] = maanfase;
  
//  myArray["startDimmen"] = startDimmen;
//  myArray["eindeDimmen"] = eindeDimmen;
//  myArray["startOpdraaien"] = startOpdraaien;
//  myArray["eindeOpdraaien"] = eindeOpdraaien;

  String jsonString = JSON.stringify(myArray);
  Serial.println(jsonString);
  return jsonString;
}

String getStates(){

  // Deze wordt naar de client verstuurd bij openen van de connectie
  JSONVar myArray;
  
  myArray["mode"] = mode;
  myArray["ipadres"] = ipToString(ipadres);
  myArray["minBrightness"] = minBrightness;
  myArray["maxBrightness"] = maxBrightness;
  myArray["stepBrightness"] = stepBrightness;
  myArray["startBrightness"] = startBrightness;

  //Kleur
  myArray["rood"] = bRood;
  myArray["groen"] = bGroen;
  myArray["blauw"] = bBlauw;
  
  //Kleur seconden
  myArray["srood"] = bSRood;
  myArray["sgroen"] = bSGroen;
  myArray["sblauw"] = bSBlauw;
  
  myArray["lengtegraad"] = String(longitude);
  myArray["breedtegraad"] = String(latitude);

  myArray["uuruit"] = uuruit;
  myArray["uuraan"] = uuraan;
  myArray["nachtsterkte"] = nachtsterkte;

  myArray["wordclock"] = wordclock;
  myArray["effect"] = effect;

  myArray["maan"] = maan;
  myArray["desOchtends"] = desOchtends;
  myArray["hetIs"] = hetIs;

  myArray["timeserver"] = ntpServer;
  myArray["timeoffset"] = gmtOffset_sec / 3600;
  myArray["zomer"] = autoZomertijd;

//  myArray["startDimmen"] = startDimmen;
//  myArray["eindeDimmen"] = eindeDimmen;
//  myArray["startOpdraaien"] = startOpdraaien;
//  myArray["eindeOpdraaien"] = eindeOpdraaien;

  String jsonString = JSON.stringify(myArray);
  Serial.println(jsonString);
  return jsonString;
}

void notifyClients(String state) {
  // Dit stuurt een bericht aan alle verbonden clients, dat er iets is gewijzigd
  // input is de jsonString uit getStates() of de getStatus()
  ws.textAll(state);
}
7void readTime(void) {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  Serial.print(F("Setting time using SNTP "));

  // Haal de tijd van het internet
  configTime(gmtOffset_sec, 0, ntpServer.c_str());
  printLocalTime();

}

void printLocalTime() {

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "states") == 0) {
      // De 'states' is de opdracht die vanuit de Javascript wordt gestuurd en hier wordt afgevangen en afgehandeld
      // Dit gebeurt bij het openen van de connectie, dus gewoon de hele toestand ophalen
      notifyClients(getStates());
    } else {
      Serial.printf("Ontvangen bericht %s\n", (char*)data );
      docIn.clear();
      err = deserializeJson(docIn, (char*)data);
      Serial.print("Usage docIn: ");
      Serial.println(docIn.memoryUsage());
      boolean setDeTime = false;
      if (err == DeserializationError::Ok) {
        if (docIn.containsKey("kleurmode")) {
          String kleurMode = docIn["kleurmode"].as<String>();
          if (kleurMode == "minBrightness") {
            minBrightness = docIn["minBrightness"].as<byte>();
          }
          if (kleurMode == "maxBrightness") {
            maxBrightness = docIn["maxBrightness"].as<byte>();
          }
          if (kleurMode == "stepBrightness") {
            stepBrightness = docIn["stepBrightness"].as<byte>();
          }
          if (kleurMode == "startBrightness") {
            startBrightness = docIn["startBrightness"].as<byte>();
          }
          if (kleurMode == "rood") {
            bRood = docIn["rood"].as<byte>();
          }
          if (kleurMode == "groen") {
            bGroen = docIn["groen"].as<byte>();
          }
          if (kleurMode == "blauw") {
            bBlauw = docIn["blauw"].as<byte>();
          }
          if (kleurMode == "srood") {
            bSRood = docIn["rood"].as<byte>();
          }
          if (kleurMode == "sgroen") {
            bSGroen = docIn["groen"].as<byte>();
          }
          if (kleurMode == "sblauw") {
            bSBlauw = docIn["blauw"].as<byte>();
          }
          if (kleurMode == "sblauw") {
            bSBlauw = docIn["blauw"].as<byte>();
          }
          if (kleurMode == "lengtegraad") {
            longitude = docIn["lengtegraad"].as<float>();
            if (longitude == 0) {
              longitude = 5.69;
            }
          }
          if (kleurMode == "breedtegraad") {
            latitude = docIn["breedtegraad"].as<float>();
            if (latitude == 0) {
              latitude = 53.03;
            }
          }
          if (kleurMode == "uuruit") {
            uuruit = docIn["uuruit"].as<byte>();
            if (uuruit == 0) {
              uuruit = 24;
            }
          }
          if (kleurMode == "uuraan") {
            uuraan = docIn["uuraan"].as<byte>();
            if (uuraan == 24) {
              uuraan = 0;
            }
          }
          if (kleurMode == "nachtsterkte") {
            nachtsterkte = docIn["nachtsterkte"].as<byte>();
          }
          if (kleurMode == "wordclock") {
            wordclock = docIn["wordclock"].as<String>();
          }
          if (kleurMode == "effect") {
            effect = docIn["effect"].as<String>();
          }
          if (kleurMode == "uuruit") {
            uuruit = docIn["uuruit"].as<byte>();
          }
          if (kleurMode == "uuraan") {
            uuraan = docIn["uuraan"].as<byte>();
          }
          if (kleurMode == "maan") {
            maan = docIn["maan"].as<byte>();
          }
          if (kleurMode == "desOchtends") {
            desOchtends = docIn["desOchtends"].as<byte>();
          }
          if (kleurMode == "hetIs") {
            hetIs = docIn["hetIs"].as<byte>();
          }
          if (kleurMode == "timeserver") {
            ntpServer = docIn["timeserver"].as<String>();
            ntpServer.trim();
            if (ntpServer.length() == 0 ) {
              ntpServer = "europe.pool.ntp.org";
            }
            setDeTime = true;
          }
          if (kleurMode == "timeoffset") {
            gmtOffset_sec = docIn["timeoffset"].as<int>() * 3600;
            setDeTime = true;
          }
          if (kleurMode == "zomer") {
            autoZomertijd = docIn["zomer"].as<byte>();
          }
        }
        
        struct tm timeinfo;
        getLocalTime(&timeinfo);
        
        docUit.clear();

        if (docIn.containsKey("klokmode") or setDeTime) {
          byte modeNw;
          if (docIn.containsKey("klokmode")) {
            modeNw = docIn["klokmode"].as<byte>();
          } else {
            // mode ongemoeid laten
            modeNw = mode;
          }
          if (modeNw == MODE_SYNC or setDeTime) {
            //init and get the time
            readTime();
            // mode ongemoeid laten
            modeNw = mode;
            getLocalTime(&timeinfo);
            docUit["uur"] = timeinfo.tm_hour;
          } else {
            // Met 0 in uur gaat de Arduino zich niet aanpassen
            docUit["uur"] = 0;
          }
          mode = modeNw;
        }

       // Hierboven is het inkomende bericht gelezen en verwerkt, nu de hele status naar de klok sturen
        // Create the JSON document to send to clock
        docUit["mode"] = mode;
        docUit["jaar"] = timeinfo.tm_year;
        docUit["maand"] = timeinfo.tm_mon + 1;
        docUit["dag"] = timeinfo.tm_mday;
        docUit["minuut"] = timeinfo.tm_min;
        docUit["seconde"] = timeinfo.tm_sec;
        docUit["minBrightness"] = minBrightness;
        docUit["maxBrightness"] = maxBrightness;
        docUit["stepBrightness"] = stepBrightness;
        docUit["startBrightness"] = startBrightness;
        
        docUit["rood"] = bRood;
        docUit["groen"] = bGroen;
        docUit["blauw"] = bBlauw;
        
        docUit["srood"] = bSRood;
        docUit["sgroen"] = bSGroen;
        docUit["sblauw"] = bSBlauw;
        
        docUit["lengtegraad"] = String(longitude);
        docUit["breedtegraad"] = String(latitude);

        docUit["uuruit"] = uuruit;
        docUit["uuraan"] = uuraan;
        docUit["nachtsterkte"] = nachtsterkte;

        docUit["wordclock"] = wordclock;
        docUit["effect"] = effect;
        
        docUit["maan"] = maan;
        docUit["desOchtends"] = desOchtends;
        docUit["hetIs"] = hetIs;

        // De gegevens van de tijd server hoeven niet doorgestuurd, alleen evt de tijd zelf
        docUit["zomer"] = autoZomertijd;

        // Send the JSON document over the "link" serial port naar de Arduino
        serializeJson(docUit, Serial2);
        //Serial.print("Usage docUit: ");
        //Serial.println(docUit.memoryUsage());
        
        eeprom_save();

        // Stel de verbonden clients op de hoogte
        notifyClients(getStates());
      }
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type, void *arg, uint8_t *data, size_t len) {
  
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

void setup(){
  
  // Serial port for debugging purposes
  Serial.begin(SERIAL_BAUD);

  // Serial port met de Arduino
  Serial2.begin(SERIAL_BAUD, SERIAL_8N1, RXD2, TXD2);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  initSPIFFS();

  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(soft_ap_ssid, soft_ap_password);

  // https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/#example2-2
  
  preferences.begin("credentials", false);
  wifi_network_ssid = preferences.getString("ssid", ""); 
  wifi_network_password = preferences.getString("password", "");
  preferences.end();

  int geprobeerd = 0;
  
  if (wifi_network_ssid == "" || wifi_network_password == ""){
    Serial.println("No values saved for ssid or password");
  } else {
    // Connect to Wi-Fi
    WiFi.begin(wifi_network_ssid.c_str(), wifi_network_password.c_str());
    while (WiFi.status() != WL_CONNECTED and geprobeerd < 15) {
      Serial.print('.');
      geprobeerd++;
      delay(1000);
    }
  } 

  if (geprobeerd == 15) {
    wifi_network_ssid = ""; 
    wifi_network_password = "";
    preferences.begin("credentials", false);
    preferences.putString("ssid", ""); 
    preferences.putString("password", "");
    preferences.end();
  } else {
    ipadres = WiFi.localIP();
  }
  
  initWebSocket();
  eeprom_retrieve();

  if (mode == MODE_TEST) {
    mode = MODE_CLOCK;
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
   if (ON_STA_FILTER(request)) {
      request->send(SPIFFS, "/index.html", "text/html", false);
      return;
  } else if (ON_AP_FILTER(request)) {
      if (wifi_network_ssid == "" || wifi_network_password == ""){
        request->send(SPIFFS, "/settings.html", "text/html", false);
      } else {
        request->send(SPIFFS, "/index.html", "text/html", false);
      }
      return;
    }
  });

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {

    // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
    if (request->hasParam("inputSSID")) {
      inputSSID = request->getParam("inputSSID")->value();
    }
    if (request->hasParam("inputWachtwoord")) {
      inputWachtwoord = request->getParam("inputWachtwoord")->value();
    }
    Serial.println(inputSSID);
    Serial.println(inputWachtwoord);
    preferences.begin("credentials", false);
    preferences.putString("ssid", inputSSID); 
    preferences.putString("password", inputWachtwoord);
    preferences.end();
    ESP.restart();
    return;
  });
  
  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (ON_STA_FILTER(request)) {
      request->send(200, "text/plain", "Hello from STA");
      return;

    } else if (ON_AP_FILTER(request)) {
      request->send(200, "text/plain", "Hello from AP");
      return;
    }

    request->send(200, "text/plain", "Hello from undefined");
  });
  
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
    preferences.begin("credentials", false);
    preferences.clear();
    preferences.end();

    preferences.begin("settings", false);
    preferences.clear();
    preferences.end();
    
    request->send(200, "text/plain", "ESP32 settings zijn gereset, herstarten...");
    ESP.restart();
    return;
  });
  
  server.serveStatic("/", SPIFFS, "/");

  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);
  
  // Start server
  server.begin();

  //init and get the time
  readTime();
  printLocalTime();

}

void loop() {
 
  if (WiFi.status() == WL_CONNECTED and millis() > 900000L and digitalRead(LED_BUILTIN) ) {
    WiFi.mode(WIFI_MODE_STA);
    WiFi.softAPdisconnect (true);
    digitalWrite(LED_BUILTIN, LOW);
  }

  // Check if the Arduino clock is transmitting
  if (Serial2.available() >= 1) {
    // Read the JSON document from the "link" serial port
    docIn.clear();
    err = deserializeJson(docIn, Serial2);

    if (err == DeserializationError::Ok) {

      byte klokmode = docIn["klokmode"].as<byte>();

      temperatuur = docIn["temperatuur"].as<float>();
      druk = docIn["druk"].as<float>();

      jaar = docIn["tijdy"].as<int>();
      maand = docIn["tijdm"].as<int>();
      datum = docIn["tijdd"].as<int>();
      uur = docIn["tijdu"].as<int>();
      minuut = docIn["tijdmm"].as<int>();
      seconde = docIn["tijds"].as<int>();

      zonop = docIn["zonop"].as<String>();
      zononder = docIn["zononder"].as<String>();
      
      stap = docIn["stap"].as<byte>();
      sterkte = docIn["sterkte"].as<byte>();
      maanfase = docIn["maanfase"].as<String>();

//      startDimmen = docIn["startDimmen"].as<String>();
//      eindeDimmen = docIn["eindeDimmen"].as<String>();
//      startOpdraaien = docIn["startOpdraaien"].as<String>();
//      eindeOpdraaien = docIn["eindeOpdraaien"].as<String>();

      if (klokmode != mode) {
        // De klok is in een andere mode dan de webserver heeft staan, dus opnieuw sturen tot het goed is
        // Create the JSON document to send to clock
        docUit.clear();
        docUit["mode"] = mode;
        docUit["jaar"] = 0;
        docUit["maand"] = 0;
        docUit["dag"] = 0;
        docUit["uur"] = 0;
        docUit["minuut"] = 0;
        docUit["seconde"] = 0;
        docUit["minBrightness"] = minBrightness;
        docUit["maxBrightness"] = maxBrightness;
        docUit["stepBrightness"] = stepBrightness;
        docUit["startBrightness"] = startBrightness;
        
        docUit["rood"] = bRood;
        docUit["groen"] = bGroen;
        docUit["blauw"] = bBlauw;
        
        docUit["srood"] = bSRood;
        docUit["sgroen"] = bSGroen;
        docUit["sblauw"] = bSBlauw;
        
        docUit["lengtegraad"] = String(longitude);
        docUit["breedtegraad"] = String(latitude);

        docUit["uuruit"] = uuruit;
        docUit["uuraan"] = uuraan;
        docUit["nachtsterkte"] = nachtsterkte;

        docUit["wordclock"] = wordclock;
        docUit["effect"] = effect;

        docUit["maan"] = maan;
        docUit["desOchtends"] = desOchtends;
        docUit["hetIs"] = hetIs;

        docUit["zomer"] = autoZomertijd;

        // Send the JSON document over the "link" serial port naar de Arduino
        serializeJson(docUit, Serial2);
      }
      
    } else {
      // Flush all bytes in the "link" serial port buffer
      while (Serial2.available() >= 1) {
        Serial2.read();
      }
    }
  }

  if (millis() < _millis) {
    _next_1000_ms = millis();
  }

  _millis = millis();
  
  if (millis() > _next_1000_ms) { 
    _next_1000_ms = millis() + 1000L;
    notifyClients(getStatus());
  }
}
