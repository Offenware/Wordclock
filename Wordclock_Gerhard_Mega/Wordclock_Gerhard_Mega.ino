/*

Word Clock
Copyright (C) 2015 by Xose Pérez <xose dot perez at gmail dot com>
Copyright (C) 2021 Modified by Gerhard Bouma

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <gbTime.h>         //http://www.arduino.cc/playground/Code/Time  
#include <gbTimeLib.h>
#include <RTClib.h> // 0x68

#include "wordclock.h"
#include "nederlands.h"
#include "TimeLord.h"

#include "MoonPhase.h"

// =============================================================================
// Globals
// =============================================================================
MoonPhase mp;
unsigned long _next_1000_ms;
unsigned long _millis;

int current_year;
int current_month;
int current_day;
int current_hour;
int current_minute;
int current_second;

int zon_op_uu;
int zon_op_mm;
int zon_onder_uu;
int zon_onder_mm;
int tellerop = 0;

uint32_t wit;
uint32_t rood;
uint32_t groen;
uint32_t blauw;
uint32_t geel;
uint32_t oranje;

unsigned long startDimmen;
unsigned long eindeDimmen;
unsigned long eindeOpdraaien;
unsigned long startOpdraaien;

color_struct kleurtje[6];
color_struct regenboog[16];

uint8_t mode;
uint8_t minBrightness;
uint8_t maxBrightness;
uint8_t stepBrightness; //in seconden
uint8_t startBrightness; //gerekend in minuten voor zonsondergang
uint8_t bRood; 
uint8_t bGroen; 
uint8_t bBlauw; 
uint8_t bSRood; 
uint8_t bSGroen; 
uint8_t bSBlauw; 
uint8_t uuruit; 
uint8_t uuraan; 
uint8_t nachtsterkte; 
uint8_t bWordclock; 
uint8_t bEffect; 
uint8_t maan; 
uint8_t desOchtends; 
uint8_t hetIs; 

uint32_t kleur;
uint32_t kleurSec;

// what is our longitude (west values negative) and latitude (south values negative)
float longitude;
float latitude;

String wordclock = "secaan";
String effect = "geeneffect";
String effectOud;
uint8_t modeOud;

// The matrix array holds the time pattern matrix values
// De time_pattern bevat MATRIX_HEIGHT regels met daarin MATRIX_WIDTH tekens
unsigned int time_pattern[MATRIX_HEIGHT] = {0};

uint8_t brightness = DEFAULT_BRIGHTNESS;
uint8_t tabBrightness;

int _druk; // doorgeefluikje, heb geen zin het anders te doen

boolean ledState = 0;
boolean toggle = false;
boolean isZomertijd = false;
boolean autoZomertijd = true;
boolean opstarten = true;
boolean gedimd = false;

boolean berichtOntvangenOK = false;
boolean berichtOntvangenNOK = false;

// Allocate the JSON document
// This one must be bigger than for the sender because it must store the strings
StaticJsonDocument<600> docIn;
StaticJsonDocument<600> docUit;
DeserializationError err;

// =============================================================================
// Pixel matrix
// =============================================================================
Adafruit_NeoPixel matrix = Adafruit_NeoPixel(TOTAL_PIXELS, PIN_LEDmatrix, NEO_GRB + NEO_KHZ800);

// =============================================================================
// RTC
// =============================================================================
RTC_DS3231 rtc;

// =============================================================================
// BME
// =============================================================================
#define SEALEVELPRESSURE_HPA (1013.76)
Adafruit_BME280 bme; // I2C

boolean vlagje = false;

// =============================================================================
// Methods
// =============================================================================

void eeprom_save() {

  // https://www.arduino.cc/en/Reference/EEPROMPut
  
  int eeAddress = 0;   //Location we want the data to be put.
  
  if (wordclock == "secaan") {
    bWordclock = 1;
  } else if (wordclock == "secuit") {
    bWordclock = 2;
  } else if (wordclock == "seckleur") {
    bWordclock = 3;
  }
  
  if (effect == "geeneffect") {
    bEffect = 1;
  } else if (effect == "vlag") {
    bEffect = 2;
  } else if (effect == "regenboog") {
    bEffect = 3;
  } else if (effect == "gradient") {
    bEffect = 4;
  }

  //Data to store.
  MyObject customVar = {
    mode,
    minBrightness,
    maxBrightness,
    stepBrightness,
    startBrightness,
    bRood,
    bGroen, 
    bBlauw, 
    bSRood, 
    bSGroen, 
    bSBlauw,
    uuruit, 
    uuraan, 
    nachtsterkte, 
    bWordclock, 
    bEffect, 
    maan, 
    desOchtends, 
    hetIs, 
    autoZomertijd 
  };

  EEPROM.put(eeAddress, customVar);

  eeAddress = 19;
  EEPROM.put(eeAddress, longitude);
  
  eeAddress += sizeof(float); //Move address to the next byte after float 'f'.
  EEPROM.put(eeAddress, latitude);

}

void eeprom_retrieve() {

  // https://www.arduino.cc/en/Reference/EEPROMGet
  
  int eeAddress = 0; //EEPROM address to start reading from
   
  MyObject customVar; //Variable to store custom object read from EEPROM.
  EEPROM.get(eeAddress, customVar);

  eeAddress = 19;
  EEPROM.get(eeAddress, longitude);
  
  eeAddress += sizeof(float); //Move address to the next byte after float 'f'.
  EEPROM.get(eeAddress, latitude);
 
  if (isnan(customVar.mode)) mode = MODE_CLOCK; mode = customVar.mode;
  if (isnan(customVar.startBrightness)) startBrightness = 60; startBrightness = customVar.startBrightness;
  if (isnan(customVar.stepBrightness)) stepBrightness = 30; stepBrightness = customVar.stepBrightness;
  if (isnan(customVar.minBrightness)) minBrightness = 11; minBrightness = customVar.minBrightness;
  if (isnan(customVar.maxBrightness)) maxBrightness = 255; maxBrightness = customVar.maxBrightness;
  if (isnan(customVar.bRood)) bRood = 255; bRood = customVar.bRood;
  if (isnan(customVar.bGroen)) bGroen = 255; bGroen = customVar.bGroen;
  if (isnan(customVar.bBlauw)) bBlauw = 255; bBlauw = customVar.bBlauw;
  if (isnan(customVar.bSRood)) bSRood = 255; bSRood = customVar.bSRood;
  if (isnan(customVar.bSGroen)) bSGroen = 255; bSGroen = customVar.bSGroen;
  if (isnan(customVar.bSBlauw)) bSBlauw = 255; bSBlauw = customVar.bSBlauw;
  if (isnan(customVar.uuruit)) uuruit = 23; uuruit = customVar.uuruit;
  if (isnan(customVar.uuraan)) uuraan = 7; uuraan = customVar.uuraan;
  if (isnan(customVar.nachtsterkte)) nachtsterkte = 12; nachtsterkte = customVar.nachtsterkte;
  if (isnan(customVar.bWordclock)) bWordclock = 1; bWordclock = customVar.bWordclock;
  if (isnan(customVar.bEffect)) bEffect = 1; bEffect = customVar.bEffect;
  if (isnan(customVar.maan)) maan = true; maan = customVar.maan;
  if (isnan(customVar.desOchtends)) desOchtends = false; desOchtends = customVar.desOchtends;
  if (isnan(customVar.hetIs)) hetIs = true; hetIs = customVar.hetIs;
  if (isnan(customVar.autoZomertijd)) autoZomertijd = true; autoZomertijd = customVar.autoZomertijd;

  if (bWordclock == 1) {
    wordclock = "secaan";
  } else if (bWordclock == 2) {
    wordclock = "secuit";
  } else if (bWordclock == 3) {
    wordclock = "seckleur";
  }
  
  if (bEffect == 1) {
    effect = "geeneffect";
  } else if (bEffect == 2) {
    effect = "vlag";
  } else if (bEffect == 3) {
    effect = "regenboog";
  } else if (bEffect == 4) {
    effect = "gradient";
  }
  
}

// Get pixel indexje in matrix from X,Y coords
unsigned int pixelindex(int karakter, int regel) {
  
  int ledpositie = regel * MATRIX_WIDTH;
  ledpositie += karakter;

  // de ledpositie is nu van 0-255, we willen 255-0
  ledpositie = 255 - ledpositie;
  return ledpositie;
  
}

/**
 * Prints current time part throu serial
 * @param  int  digits        Number to print
 * @param  bool semicolon     Whether to prepend a semicolon or not, defaults to false
 */
void printDigits(int digits, bool semicolon = false){
  
  if (semicolon) Serial.print(F(":"));
  if(digits < 10) Serial.print(F("0"));
  Serial.print(digits);
  
}

void drawScreen(const byte* buffertje, int offsetX, int offsetY, int breedte, int hoogte) { 
   
  for (int regel = 0; regel < hoogte; ++regel){
    for (int karakter = 0; karakter < breedte; ++karakter){
      const unsigned char mask = 1 << karakter; // shift 1 karakters naar links
      int pix = pixelindex(breedte - karakter - 1 + offsetX, regel + offsetY);
      if (buffertje[regel] & mask) { // Dit bit is geset
        Matrix_setPixelColor(pix, getColorSchema(pix));
      }
    }
  }
}

unsigned long getColorSchema(int pix) {
  
  if (mode == MODE_BAROMETER) {
    if (_druk <= LUCHTDRUK_LAAG) {
      return rood; 
    }
    if (_druk > LUCHTDRUK_LAAG and _druk <= LUCHTDRUK_HOOG) {
      return oranje;
    }
    if (_druk > LUCHTDRUK_HOOG) {
      return groen;
    }
  }
  
// =============================================================================
// Dutch flag colors
// =============================================================================
  if (effect == "vlag") {
    if (pix >= 0 and pix < (5 * 16)) {
      return blauw; 
    } else if (pix >= (5 * 16) and pix < (10 * 16)) {
      return wit; 
    } else if (pix >= (10 * 16)) {
      if ((current_month == 4 and current_day == 27)) {
        if (pix >= (15 * 16)) {
          return oranje;
        }
      }
      return rood;
    }
  }

  static int kleurstap = 0;
  static int hoofdkleur = 0;
  
// =============================================================================
// Rainbow colors
// =============================================================================
  if ((mode == MODE_CLOCK or mode == MODE_DIGICLOCK or mode == MODE_TEMPERATUUR) and effect == "regenboog") {
    int kolomNw = ((pix * 16) % 256 / 16);
    return Matrix_Color(ledLookupTable[regenboog[kolomNw].r], ledLookupTable[regenboog[kolomNw].g], ledLookupTable[regenboog[kolomNw].b]);
  }

// =============================================================================
// Gradient colors (shifting slowly over the spectrum)
// =============================================================================
  if ((mode == MODE_CLOCK or mode == MODE_DIGICLOCK or mode == MODE_TEMPERATUUR) and effect == "gradient") {
    int volgende = hoofdkleur + 1;
    if (volgende == 6) {
      volgende = 0;
    }
    
    if (pix == 255) {
      if (kleurstap++ >= 64) {
        kleurstap = 0;
        hoofdkleur++;
      }
    }
   
    if (hoofdkleur == 6) {
        hoofdkleur = 0;
        volgende = 1;
    }
    
    int red = map(kleurstap, 0, 64, kleurtje[hoofdkleur].r, kleurtje[volgende].r);  
    int green = map(kleurstap, 0, 64, kleurtje[hoofdkleur].g, kleurtje[volgende].g);    
    int blue = map(kleurstap, 0, 64, kleurtje[hoofdkleur].b, kleurtje[volgende].b);  

    return Matrix_Color(ledLookupTable[red], ledLookupTable[green], ledLookupTable[blue]);
  }
  
// =============================================================================
// Seconds different color
// =============================================================================
  if (mode == MODE_CLOCK) {
    if (wordclock == "seckleur" and ((pix >= ((8 * 16) - 8)) and (pix < ((10 * 16)- 14)))) {
      return kleurSec;
    } else {
      return kleur;  
    }
  }
  
  if (mode == MODE_DIGICLOCK) {
    if (wordclock == "seckleur" and ((pix >= 0 and pix < (9 * 16)))) {
      return kleurSec;
    } else {
      return kleur;  
    }
  }

  return kleur;  
}

void vulGRDigit(int digit, int pos, int nulpuntX, int nulpuntY) {

// =============================================================================
// Big digits
// =============================================================================
  byte dest[8] = {};
  int offsetX;
  int offsetY;
  
  switch (digit) {
    case 0:
      memcpy(dest, NR_NUL, GR_DIGIT_HOOGTE);
      break;
    case 1:
      memcpy(dest, NR_EEN, GR_DIGIT_HOOGTE);
      break;
    case 2:
      memcpy(dest, NR_TWEE, GR_DIGIT_HOOGTE);
      break;
    case 3:
      memcpy(dest, NR_DRIE, GR_DIGIT_HOOGTE);
      break;
    case 4:
      memcpy(dest, NR_VIER, GR_DIGIT_HOOGTE);
      break;
    case 5:
      memcpy(dest, NR_VIJF, GR_DIGIT_HOOGTE);
      break;
    case 6:
      memcpy(dest, NR_ZES, GR_DIGIT_HOOGTE);
      break;
    case 7:
      memcpy(dest, NR_ZEVEN, GR_DIGIT_HOOGTE);
      break;
    case 8:
      memcpy(dest, NR_ACHT, GR_DIGIT_HOOGTE);
      break;
    case 9:
      memcpy(dest, NR_NEGEN, GR_DIGIT_HOOGTE);
      break;
  }

  if (pos < 4) {
    offsetY = nulpuntY;
  } else {
    offsetY = nulpuntY + GR_DIGIT_HOOGTE + 1;
    pos = pos - 4;
  }

  offsetX = pos * (GR_DIGIT_BREEDTE + 1) + nulpuntX;
  
  drawScreen(dest, offsetX, offsetY, GR_DIGIT_BREEDTE, GR_DIGIT_HOOGTE);
}

void vulKLDigit(int digit, int pos, int nulpuntX, int nulpuntY) {

// =============================================================================
// Small digits
// =============================================================================
  byte dest[8] = {};
  int offsetX;
  int offsetY;
  
  switch (digit) {
    case 0:
      memcpy(dest, NR_NUL_KL, KL_DIGIT_HOOGTE);
      break;
    case 1:
      memcpy(dest, NR_EEN_KL, KL_DIGIT_HOOGTE);
      break;
    case 2:
      memcpy(dest, NR_TWEE_KL, KL_DIGIT_HOOGTE);
      break;
    case 3:
      memcpy(dest, NR_DRIE_KL, KL_DIGIT_HOOGTE);
      break;
    case 4:
      memcpy(dest, NR_VIER_KL, KL_DIGIT_HOOGTE);
      break;
    case 5:
      memcpy(dest, NR_VIJF_KL, KL_DIGIT_HOOGTE);
      break;
    case 6:
      memcpy(dest, NR_ZES_KL, KL_DIGIT_HOOGTE);
      break;
    case 7:
      memcpy(dest, NR_ZEVEN_KL, KL_DIGIT_HOOGTE);
      break;
    case 8:
      memcpy(dest, NR_ACHT_KL, KL_DIGIT_HOOGTE);
      break;
    case 9:
      memcpy(dest, NR_NEGEN_KL, KL_DIGIT_HOOGTE);
      break;
  }

  if (pos < 4) {
    offsetY = nulpuntY;
  } else {
    offsetY = nulpuntY + KL_DIGIT_HOOGTE + 1;
    pos = pos - 4;
  }

  offsetX = pos * (KL_DIGIT_BREEDTE + 1) + nulpuntX;
  
  drawScreen(dest, offsetX, offsetY, KL_DIGIT_BREEDTE, KL_DIGIT_HOOGTE);
  
}

// =============================================================================
// === CLOCK ===================================================================
// =============================================================================
/**
 * Displays time through serial
 * @param  DateTime now           DateTime object
 */
void digitalClockDisplay(){
  
  Serial.print(F("Time: "));
  printDigits(current_year, false);
  Serial.print("-");
  printDigits(current_month, false);
  Serial.print("-");
  printDigits(current_day, false);
  Serial.print(" ");

  printDigits(current_hour, false);
  printDigits(current_minute, true);
  printDigits(current_second, true);

  Serial.println();
  
}

/**
 * Loads current time pattern in time_pattern matrix
 */
void prepareTimePattern() {

  DateTime now = rtc.now();
  current_year = now.year();
  current_month = now.month();
  current_day = now.day();
  current_hour = now.hour();
  current_minute = now.minute();
  current_second = now.second();

// =============================================================================
// This is a bit complicated. In effect it calculates based on sunup or sundown how fast to lower/increase light intensity en when it should be done
// All configurable through the web interface on the ESP32
// =============================================================================

  //=============================================================
  // Bepalen lichtsterkte
  //=============================================================
  if (isZomertijd) {
    current_hour++;
    if (current_hour > 23) {
      current_hour -= 24;
    }
  }

  if (current_hour == 2 and current_minute == 0 and current_second == 00 or zon_op_uu == 0){
    bepaalZonOpOnder();
  }

  //                      opdraaien                  zonop                                            zononder   dimmen
  //---------------------*------------------------&--|------------------------------------------------------|--*-------------------------&-------------------
  //                              periode                                                                             periode   
  
  unsigned long nu = (current_hour * 3600L) + (current_minute * 60L) + current_second; 
 
  //Het verschil tussen min en max maal het aantal stappen 
  //maxBrightness: de hoogste stap
  //minBrightness: de laagste stap
  //stepBrightness: per dit aantal seconden een stap op of neer in lichtsterkte 
  
  unsigned long periode = (maxBrightness - minBrightness) * stepBrightness; // dit aantal seconden wordt er gedimd of opgedraaid
 
  //startBrightness: gerekend in minuten voor zonsondergang of na zonsopkomst
  startDimmen = (zon_onder_uu * 3600L) + (zon_onder_mm * 60L) - (startBrightness * 60L);
  eindeDimmen = startDimmen + periode;

  eindeOpdraaien = (zon_op_uu * 3600L) + (zon_op_mm * 60L) + (startBrightness * 60L);
  startOpdraaien = eindeOpdraaien - periode;

  boolean dimmen = false;
  boolean opdraaien = false;
  unsigned long periodeVerstreken = 0; // Hoe lang het dimmen of opdraaien al bezig is
  
  if (nu >= startDimmen and nu <= eindeDimmen) {
    dimmen = true;
  } else if (nu >= startOpdraaien and nu <= eindeOpdraaien) {
    opdraaien = true;
  } else if (nu < startOpdraaien or nu > eindeDimmen) {
    tabBrightness = minBrightness;
  } else if (nu < startDimmen and nu > eindeOpdraaien) {
    tabBrightness = maxBrightness;
  }

  if (dimmen) {
    periodeVerstreken = nu - startDimmen;
    tabBrightness = maxBrightness - (periodeVerstreken / stepBrightness);
  }

  if (opdraaien) {
    periodeVerstreken = nu - startOpdraaien;
    tabBrightness = minBrightness + (periodeVerstreken / stepBrightness);
  }

  if (tabBrightness > maxBrightness) tabBrightness = maxBrightness;
  if (tabBrightness < minBrightness) tabBrightness = minBrightness;
  
  if (uuraan != uuruit) {
    if (current_hour >= uuruit ) {
      tabBrightness = nachtsterkte;
    }
    if (current_hour < uuraan) {
      tabBrightness = nachtsterkte;
    }
  }

// =============================================================================
// Don't turn on leds full blast right away
// =============================================================================
  // Langzaam opstarten, beter voor de leds etc
  if (ledLookupTable[tabBrightness] > brightness + 20) {
    opstarten = true;
  }
  
  if (brightness >= 245) {
    opstarten = false;
  }
  
  if (opstarten) {
    if (brightness < ledLookupTable[tabBrightness]) {
      brightness = brightness + 10;
    } else {
      brightness = ledLookupTable[tabBrightness];
      opstarten = false;
    }
  } else {
    brightness = ledLookupTable[tabBrightness];
  }

}

/**
 * Loads current time pattern in time_pattern matrix
 */
void loadTimePattern() {

  // Reset time pattern
  // De time_pattern bevat MATRIX_HEIGHT regels met daarin MATRIX_WIDTH tekens
  // We zetten de regels op 0
  for (byte i = 0; i < MATRIX_HEIGHT; i++) time_pattern[i] = 0;

  // Output datum tijd naar Seriële monitor
  // digitalClockDisplay();

  // Load strings
  loadLanguageDutch(current_year, current_month, current_day, current_hour, current_minute, current_second, time_pattern, !(mode == MODE_MATRIX or (mode == MODE_CLOCK and wordclock == "secuit")), mp.phaseName, hetIs, desOchtends, maan);

}

void loadTimeInMatrix(unsigned int* pattern) {

  for (byte regel=0; regel < MATRIX_HEIGHT; regel++) {
    unsigned int value = 1;
      
    for (byte karakter=0; karakter < MATRIX_WIDTH; karakter++) {
        
      if ((pattern[regel] & value) > 0) {
        Matrix_setPixelColor(pixelindex(karakter, regel), getColorSchema(pixelindex(karakter, regel)));
      }
      value <<= 1;
    }
  }

  if (berichtOntvangenOK) {
    berichtOntvangenOK = false;
    Matrix_setPixelColor(0, groen);
  }
  
  if (berichtOntvangenNOK) {
    berichtOntvangenNOK = false;
    Matrix_setPixelColor(0, rood);
  }
  
}

/**
 * Load current time into LED matrix
 */
void updateClock() {
  
  Matrix_clear();
  Matrix_setBrightness(brightness);
  loadTimeInMatrix(time_pattern);
  matrix.show();

}

// =============================================================================
// === FIREWORKS ===============================================================
// =============================================================================

/**
 * Calculates color of the ray depending on lifetime
 * @param  byte current       current strength
 * @param  byte total         lifetime
 */
unsigned long getVuurwerkColor(byte current, byte total, byte totalR, byte totalG, byte totalB) {
   byte red = map(current, 0, total, 0, totalR);
   byte green = map(current, 0, total, 0, totalG);
   byte blue = map(current, 0, total, 0, totalB);
   return Matrix_Color(red, green, blue);
}

/**
 * Updates matrix effect
 * @param  bool force         Update regardless the time since last update
 */
void updateVuurwerk() {

   static unsigned long count = 0;
   static unsigned long next_update = millis();
   
   static byte current_num_rays = 0;
   static vuurwerk_struct ray[VUURWERK_MAX_RAYS];
   byte i = 0;

   if (next_update > millis()) return;
  
   if (current_num_rays < VUURWERK_MAX_RAYS) {
      bool do_create = random(0, 100) < VUURWERK_BIRTH_RATIO;
      if (do_create) {
         i=0;
         while (ray[i].life > 0) i++;
         ray[i].x = random(0, MATRIX_WIDTH);
         ray[i].y = random(0, MATRIX_HEIGHT);
         ray[i].life = random(VUURWERK_LIFE_MIN, VUURWERK_LIFE_MAX);
         ray[i].length = ray[i].life;
         ray[i].rood = random(0, 255);
         ray[i].groen = random(0, 255);
         ray[i].blauw = random(0, 255);
         current_num_rays++;
      }
   }
   
   Matrix_clear();

   for (i = 0; i < VUURWERK_MAX_RAYS; i++) {
      if (ray[i].life > 0) {
         // update ray life
         ray[i].life = ray[i].life - 1;
         Matrix_setPixelColor(pixelindex(ray[i].x, ray[i].y), getVuurwerkColor(ray[i].life, ray[i].length, ray[i].rood, ray[i].groen, ray[i].blauw));
      }
     // free ray if dead
     if (ray[i].life == 0) current_num_rays--;
   }

   Matrix_setBrightness(255);
   matrix.show();

   if ((current_num_rays == 0)) {
      count = 0;
   } else {
      count++;
   }

   next_update = millis() + UPDATE_VUURWERK;
}

// =============================================================================
// === KITT 2000 TRADITIONEEL ==================================================
// =============================================================================
void chase(bool herstel = false) {

  static unsigned long next_update = millis();
  static bool create = true;
  static kitt_struct kittray[KITT_STAART];
  static int richting = 1;

  if (create or herstel) {
    create = false;
    for (byte i = 0; i < KITT_STAART; i++) {
       kittray[i].x = i;
       kittray[i].y = KITT_VANRIJ;
    }
  }

  if (next_update > millis()) return;
  
  Matrix_clear();
  Matrix_setBrightness(255);
  
  for (byte i = 0; i < KITT_STAART; i++) {
    byte red = map(i, 0, KITT_STAART, 50, 255);
    Matrix_setPixelColor(pixelindex(kittray[i].x, kittray[i].y), Matrix_Color(ledLookupTable[red], 0, 0));
    if (kittray[i].y % 2 == 0) {
      //Even
      if (++kittray[i].x > 15) {
        kittray[i].x = 15;
        kittray[i].y += richting;
      }
    } else {
      //Oneven
      if (--kittray[i].x < 0) {
        kittray[i].x = 0;
        kittray[i].y += richting;
      }
    }
    
    if (kittray[i].y > KITT_TOTRIJ) {
      richting = -1;
      kittray[i].y = KITT_TOTRIJ;
    }
    if (kittray[i].y < KITT_VANRIJ) {
      richting = 1;
      kittray[i].y = KITT_VANRIJ;
    }
  }
  
  matrix.show();
  next_update = millis() + 100;

}

// =============================================================================
// === THE MATRIX ==============================================================
// =============================================================================

/**
 * Count the number of lit LEDs for current time pattern
 * @return byte Number of LEDs
 */
byte countLEDs() {

  byte char_total = 0;

  for (byte y = 0; y < MATRIX_HEIGHT; y++) {
    unsigned int row = time_pattern[y];
    while (row > 0) {
      if (row & 1) char_total++;
        row >>= 1;
     }
  }

  return char_total;

}

/**
 * Calculates color of the falling ray depending on distance to the tip
 * @param  byte current       Position of current led in ray
 * @param  byte total         Ray length
 */
unsigned long getMatrixColor(byte current, byte total) {
   byte green = map(current, 0, total, 255, 0);
   byte red = current == 0 ? 255 : 0;
   return Matrix_Color(red, green, 0);
}

/**
 * Updates matrix effect
 * @param  bool force         Update regardless the time since last update
 */
void updateMatrix(bool force = false) {

   static unsigned long count = 0;
   static unsigned long next_update = millis();
   
   static byte current_num_rays = 0;
   static ray_struct ray[MATRIX_MAX_RAYS + 100];
   static unsigned int countdown = 0;

   static bool sticky = false;
   static bool create = true;
   static byte char_total = 0;
   static byte char_so_far = 0;
   static unsigned int local_pattern[MATRIX_HEIGHT];

   byte i = 0;

   if (!force && (next_update > millis())) return;

   if (create && (current_num_rays < MATRIX_MAX_RAYS)) {
      bool do_create = random(0, 100) < MATRIX_BIRTH_RATIO;
      if (do_create) {
         i=0;
         while (ray[i].life > 0) i++;
         ray[i].x = random(0, MATRIX_WIDTH);
         ray[i].y = random(-5, 5);
         ray[i].speed = random(MATRIX_SPEED_MAX, MATRIX_SPEED_MIN);
         ray[i].length = random(MATRIX_LENGTH_MIN, MATRIX_LENGTH_MAX);
         ray[i].life = random(MATRIX_LIFE_MIN, MATRIX_LIFE_MAX);
         current_num_rays++;
      }
   }

   if ((!sticky) && (count > STICKY_COUNT)) {
      sticky = true;
      countdown = STICKY_MAX;
      prepareTimePattern();
      loadTimePattern();
      char_total = countLEDs();
      for (i=0; i<MATRIX_HEIGHT; i++) {
         local_pattern[i] = 0;
      }
      char_so_far = 0;
   }

   Matrix_clear();

   for (i=0; i<MATRIX_MAX_RAYS; i++) {
      if (ray[i].life > 0) {

         // update ray position depending on speed
         if (count % ray[i].speed == 0) {
            ray[i].y = ray[i].y + 1;
            ray[i].life = ray[i].life - 1;
         }

         // get colors for each pixel
         byte start = 0;
         if (ray[i].life <= ray[i].length) {
            start = ray[i].length - ray[i].life ;
         }

         bool active = false;
         for (byte p=start; p<ray[i].length; p++) {
            int y = ray[i].y - p;
            if (0 <= y && y < MATRIX_HEIGHT) {
               Matrix_setPixelColor(pixelindex(ray[i].x, y), getMatrixColor(p, ray[i].length));
            }
            active |= (y < MATRIX_HEIGHT);
         }
         if (!active) ray[i].life = 0;

         // we are in sticky mode
         if (sticky) {
            byte y = ray[i].y;
            if (0 <= y && y < MATRIX_HEIGHT) {
               // check if we have hit a led in the time_pattern matrix
               unsigned int value = 1 << ray[i].x;
               if ((time_pattern[y] & value) == value) {
                   // check if we have already hit this led before
                   if ((local_pattern[y] & value) != value) {
                      // kill the ray
                      ray[i].life = ray[i].length - 1;
                      char_so_far++;
                      // save it into local pattern
                      local_pattern[y] = local_pattern[y] + value;
                      // are we done?
                      if (char_so_far == char_total) {
                         create = false;
                      }
                  }
               }
            }
         }

         // free ray if dead
         if (ray[i].life == 0) current_num_rays--;

      }
   }

   if (sticky) {
      if (countdown > 0) {
         if (--countdown == 0) {
            Serial.println(F("Force closed"));
            for (i=0; i<MATRIX_HEIGHT; i++) local_pattern[i] = time_pattern[i];
            create = false;
         }
      }
   }

   if (sticky or !create) {
      // draw hit leds
      loadTimeInMatrix(local_pattern);
   }

   Matrix_setBrightness(brightness);
   matrix.show();

   if ((current_num_rays == 0) and !create) {
    sticky = false;
    create = true;
    count = 0;
    delay(STICKY_PAUSE);

    //=========================
    // de tijd laden in de ray matrix om ze weg te laten lopen
    for (int i = 0; i < MATRIX_HEIGHT; i++) {
      for (int k = 0; k < MATRIX_WIDTH; k++) {
        if (current_num_rays < MATRIX_MAX_RAYS + 100) {
          unsigned int value = 1 << k;
          if ((time_pattern[i] & value) == value) {
            ray[current_num_rays].x = k;
            ray[current_num_rays].y = i;
            ray[current_num_rays].speed = random(MATRIX_SPEED_MAX, MATRIX_SPEED_MIN);
            ray[current_num_rays].length = 1;
            ray[current_num_rays].life = random(MATRIX_LIFE_MIN, MATRIX_LIFE_MAX);
            current_num_rays++;
          }
        }
      }
    }

// =============================================================================
// Drop the time leds before resuming Matrix
// =============================================================================
    // De boel laten zakken 
    while (current_num_rays >= 1) { 
       if (next_update <= millis()) {
         Matrix_clear();
         for (int i = 0; i < MATRIX_MAX_RAYS + 100; i++) {
            if (ray[i].life > 0) {
               // update ray position depending on speed
               if (count % ray[i].speed == 0) {
                  ray[i].y = ray[i].y + 1;
               }
      
               bool active = false;
               if (0 <= ray[i].y && ray[i].y < MATRIX_HEIGHT) {
                  Matrix_setPixelColor(pixelindex(ray[i].x, ray[i].y), getColorSchema(pixelindex(ray[i].x, ray[i].y)));
                  active = (ray[i].y < MATRIX_HEIGHT);
               }
               if (!active) ray[i].life = 0;
      
               // free ray if dead
               if (ray[i].life == 0) current_num_rays--;
            }
         }
         Matrix_setBrightness(brightness);
         matrix.show();
         next_update = millis() + UPDATE_MATRIX;
         count++;
       }
    }
    count = 0;
    //=========================
  } else {
    count++;
  }

  next_update = millis() + UPDATE_MATRIX;

}

// =============================================================================
// === TEMPERATUUR =============================================================
// =============================================================================
void showTemp(int temp, int offsetX, int offsetY) {

  int _t = temp / 10;
  int _d = temp % 10;

  Matrix_clear();
  Matrix_setBrightness(brightness);
  
  vulGRDigit(_t, 0, offsetX, offsetY);
  vulGRDigit(_d, 1, offsetX, offsetY);
  
  drawScreen(NR_GRAAD, offsetX + 2 * GR_DIGIT_BREEDTE + 2, offsetY - 2, 4, 4);
  matrix.show();
}

// =============================================================================
// === SMILEYS == ==============================================================
// =============================================================================

void showHappy() {

  // laat maar zien
  Matrix_clear();
  Matrix_setBrightness(brightness);
  drawScreen(happyface, 4, 4, 8, 8);
  matrix.show();

}

void showNeutral() {

  // laat maar zien
  Matrix_clear();
  Matrix_setBrightness(brightness);
  drawScreen(neutralface, 4, 4, 8, 8);
  matrix.show();

}

void showSad() {

  // laat maar zien
  Matrix_clear();
  Matrix_setBrightness(brightness);
  drawScreen(sadface, 4, 4, 8, 8);
  matrix.show();

}

// =============================================================================
// === LUCHTDRUK =============================================================
// =============================================================================
void showDruk(int druk, int offsetX, int offsetY) {

  // https://kampeerwijzer.com/technieken/weersomstandigheden/
  
  int drukIn = druk;
  
  int _d1 = drukIn / 1000;
  drukIn = drukIn - (_d1 * 1000);
  
  int _d2 = drukIn / 100;
  drukIn = drukIn - (_d2 * 100);
  
  int _d3 = drukIn / 10;
  int _d4 = drukIn % 10;

  Matrix_clear();
  Matrix_setBrightness(brightness);

  if (druk <= LUCHTDRUK_LAAG) {
    drawScreen(sadface, 4, 0, 8, 8);
  }
  
  if (druk > LUCHTDRUK_LAAG and druk <= LUCHTDRUK_HOOG) {
    drawScreen(neutralface, 4, 0, 8, 8);
  }
  
  if (druk > LUCHTDRUK_HOOG) {
    drawScreen(happyface, 4, 0, 8, 8);
  }
  
  vulKLDigit(_d1, 0, offsetX, offsetY);
  vulKLDigit(_d2, 1, offsetX, offsetY);
  vulKLDigit(_d3, 2, offsetX, offsetY);
  vulKLDigit(_d4, 3, offsetX, offsetY);
  
  matrix.show();
}

void updateTemp(int temp) {
  showTemp(temp, 0, 4);
}

void updateDruk(int druk) {
  showDruk(druk, 0, 11);
}

// =============================================================================
// === OFF =====================================================================
// =============================================================================

void showOff() {

  // laat maar zien
  Matrix_clear();
  Matrix_setBrightness(0);

}

// =============================================================================
// === DIGICLOCK ===============================================================
// =============================================================================

void updateDigiClock() {

  Matrix_clear();
  Matrix_setBrightness(brightness);

  int _t;
  int _d;
  
  int offsetx = 0;
  int offsety = 0;

  if (wordclock == "secuit") {
    offsety = Y_OFFSET_DIGICLOCK_SECUIT;
  } else {  
    offsety = Y_OFFSET_DIGICLOCK_SEC;
  }
  
  _t = current_hour / 10;
  _d = current_hour % 10;
  vulKLDigit(_t, 0, offsetx, offsety);
  vulKLDigit(_d, 1, offsetx, offsety);

  _t = current_minute / 10;
  _d = current_minute % 10;
  vulKLDigit(_t, 2, offsetx, offsety);
  vulKLDigit(_d, 3, offsetx, offsety);

  if (wordclock != "secuit") {
    _t = current_second / 10;
    _d = current_second % 10;
    vulKLDigit(_t, 4, offsetx, offsety);
    vulKLDigit(_d, 5, offsetx, offsety);
  }

  matrix.show();
  
}

// =============================================================================
// === BINAIRE CLOCK ===========================================================
// =============================================================================

void updateBinaireClock() {

  Matrix_clear();
  Matrix_setBrightness(brightness);

  int sec = 15;
  int minuut = 31;
  int uur = 47;
  byte mask;

  if (wordclock != "secuit") {
    for (mask = 00000001; mask > 0; mask <<= 1) { //iterate through bit mask
      if (current_second & mask){ // if bitwise AND resolves to true
        if (wordclock == "seckleur") {
          Matrix_setPixelColor(sec, kleurSec); // Draw new pixel
        } else {
          Matrix_setPixelColor(sec, kleur); // Draw new pixel
        }
      }
      sec--;
    }
  }

  for (mask = 00000001; mask > 0; mask <<= 1) { //iterate through bit mask
    if (current_minute & mask){ // if bitwise AND resolves to true
      Matrix_setPixelColor(minuut, kleur); // Draw new pixel
    }
    minuut--;
  }

  for (mask = 00000001; mask > 0; mask <<= 1) { //iterate through bit mask
    if (current_hour & mask){ // if bitwise AND resolves to true
      Matrix_setPixelColor(uur, kleur); // Draw new pixel
    }
    uur--;
  }

  matrix.show();
  
}

void bepaalZonOpOnder() {

  //Zomertijd
  if (autoZomertijd) {
    isZomertijd = bepaalZomertijd(rtc.now().year(), rtc.now().month(), rtc.now().day());
  } else {
    isZomertijd = false;
  }

  //Zon op en onder
  int tljaar = rtc.now().year() - 2000;
  byte today[] = {  0, 0, 12, rtc.now().day(), rtc.now().month(), (byte)tljaar }; // store today's date (at noon) in an array for TimeLord to use
  
  TimeLord tardis; 
  tardis.TimeZone(1 * 60); // tell TimeLord what timezone your RTC is synchronized to. You can ignore DST
  // as long as the RTC never changes back and forth between DST and non-DST
  tardis.Position(latitude, longitude); // tell TimeLord where in the world we are

  if (tardis.SunRise(today)) { // if the sun will rise today (it might not, in the [ant]arctic)
    zon_op_uu = today[tl_hour];
    zon_op_mm = today[tl_minute];
  }
  if (tardis.SunSet(today)) { // if the sun will set today (it might not, in the [ant]arctic)
    zon_onder_uu = today[tl_hour];
    zon_onder_mm = today[tl_minute];
  }

  if (isZomertijd) {
    zon_op_uu++;
    zon_onder_uu++;
  }

}

// =============================================================================
// === UPDATE ==================================================================
// =============================================================================
void update() {

  if (vlagje) {
    //Maan fase
    time_t time = rtc.now().unixtime();
    mp.calculate(time);
    
// =============================================================================
// Below a certain brightness no special effects since they will stop working properly
// =============================================================================
    if (brightness < ledLookupTable[125]) {
      if (!gedimd) {
        gedimd = true;
        effectOud = effect;
        modeOud = mode;
        mode = MODE_CLOCK;
        effect = "geeneffect";
      }
    } else {
      if (gedimd) {
        gedimd = false;
        effect = effectOud;
        mode = modeOud;
      }
    }

    prepareTimePattern();
  }

  switch (mode) {
    case MODE_CLOCK:
      if (vlagje) {
        loadTimePattern();
        updateClock();
      }
      break;

    case MODE_DIGICLOCK:
      if (vlagje) {
        updateDigiClock();
      }
      break;

    case MODE_BINAIR:
      if (vlagje) {
        updateBinaireClock();
      }
      break;

    case MODE_TEMPERATUUR:
      if (vlagje) {
        updateTemp(bme.readTemperature());
      }
      break;

    case MODE_BAROMETER:
      if (vlagje) {
        _druk = bme.readPressure() / 100.0F;
        updateDruk(_druk);
      }
      break;

    case MODE_KITT2000:
      chase(); 
      break;
       
    case MODE_MATRIX:
      updateMatrix();
      break;
       
    case MODE_VUURWERK:
      updateVuurwerk();
      break;
       
    case MODE_OFF:
      if (vlagje) {
        showOff();
        matrix.show();
      }
      break;
   }
}

String printDim(unsigned long tijd) {
  
  unsigned long wwDim = tijd;
  unsigned long tijd_uu = wwDim / 3600L;
  wwDim = wwDim - (tijd_uu * 3600L);
  unsigned long tijd_mm = wwDim / 60L;
  wwDim = wwDim - (tijd_mm * 60L);
  unsigned long tijd_ss = wwDim / 60L;

  String S = "";
  
  if (tijd_uu < 10) {
    S = S + "0" + String(tijd_uu);
  } else { 
    S = S + String(tijd_uu);
  }

  S = S + ":";
  
  if (tijd_mm < 10) {
    S = S + "0" + String(tijd_mm);
  } else { 
    S = S + String(tijd_mm);
  }
  
  S = S + ":";
  
  if (tijd_ss < 10) {
    S = S + "0" + String(tijd_ss);
  } else { 
    S = S + String(tijd_ss);
  }

  return S;
  
}

void sendStatus() {
  
  // Create the JSON document to send to the ESP32 webserver
  docUit["klokmode"] = mode;
  docUit["temperatuur"] = bme.readTemperature();

  if (_druk <= LUCHTDRUK_LAAG) {
    docUit["druk"] = bme.readPressure() / 100.0F;
  }
  if (_druk > LUCHTDRUK_LAAG and _druk <= LUCHTDRUK_HOOG) {
    docUit["druk"] = bme.readPressure() / 100.0F;
  }
  if (_druk > LUCHTDRUK_HOOG) {
    docUit["druk"] = bme.readPressure() / 100.0F;
  }

  docUit["tijdy"] = current_year;
  docUit["tijdm"] = current_month;
  docUit["tijdd"] = current_day;
  docUit["tijdu"] = current_hour;
  docUit["tijdmm"] = current_minute;
  docUit["tijds"] = current_second;

  String zonsop;
  String zonsonder;
  
  if (zon_op_uu < 10) {
    zonsop += "0";
  }
  zonsop += String(zon_op_uu) + ":";
  if (zon_op_mm < 10) {
    zonsop += "0";
  }
  zonsop += String(zon_op_mm);
 
  if (zon_onder_uu < 10) {
    zonsonder += "0";
  }
  zonsonder += String(zon_onder_uu) + ":";
  if (zon_onder_mm < 10) {
    zonsonder += "0";
  }
  zonsonder += String(zon_onder_mm);

  docUit["zonop"] = zonsop;
  docUit["zononder"] = zonsonder;
  
  docUit["stap"] = tabBrightness;
  docUit["sterkte"] = brightness;
  docUit["maanfase"] = String(mp.phaseName) + "<br>" + String(mp.phase) + "%";

  serializeJson(docUit, Serial1);

}

uint32_t Matrix_Color (int r, int g, int b) {
  return matrix.Color(r, g, b);
}

void Matrix_setPixelColor (int pix, uint32_t c) {
  matrix.setPixelColor(pix, c);
}

void Matrix_clear () {
  matrix.clear();
}

void Matrix_setBrightness (int b) {
  matrix.setBrightness(b);
}

void Matrix_show () {
  matrix.show();
}

// =============================================================================
// === ARDUINO =================================================================
// =============================================================================

void setup() {

  Serial.begin(SERIAL_BAUD);

  //19(RX), 18(TX)
  Serial1.begin(SERIAL_BAUD);
 
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  digitalWrite(PIN_BUZZER, HIGH);
  //delay(10);
  digitalWrite(PIN_BUZZER, LOW);

// === RTC =====================================================================
  // Config RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
  } else {
  // enable the 1 Hz output
    rtc.writeSqwPinMode (DS3231_SquareWave1Hz);
    randomSeed(rtc.now().unixtime());  // Initialise random number generation
  
    // set up to handle interrupt from 1 Hz pin
    //pinMode (rtcTimerIntPin, INPUT_PULLUP);
    //attachInterrupt (digitalPinToInterrupt (rtcTimerIntPin), rtc_interrupt, RISING);
  }

// === BME SENSOR ==============================================================
// Dit uitgezet in de Adafruit library regel 109
  // if (_sensorID != 0x60)
  //   return false;
        
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  }
  
// === INIT =====================================================================
  wit = Matrix_Color(255, 255, 255);
  rood = Matrix_Color(255, 0, 0);
  groen = Matrix_Color(0, 255, 0);
  blauw = Matrix_Color(0, 0, 255);
  geel = Matrix_Color(255, 255, 0);
  oranje = Matrix_Color(255, 40, 0);
  kleur = Matrix_Color(255, 255, 255);

  loadTimePattern();
  eeprom_retrieve();

  if (mode == MODE_TEST) {
    mode = MODE_CLOCK;
  }

  kleur = Matrix_Color(bRood, bGroen, bBlauw);
  kleurSec = Matrix_Color(bSRood, bSGroen, bSBlauw);

  matrix.begin();
  Matrix_setBrightness(brightness);
  matrix.show();

  bepaalZonOpOnder();

  // Onder de 12 stappen staat de led alleen maar uit, 8 is mooi genoeg
  // rood
  kleurtje[0].r = 255;
  kleurtje[0].g = 8;
  kleurtje[0].b = 8;
  // geel
  kleurtje[1].r = 255;
  kleurtje[1].g = 255;
  kleurtje[1].b = 8;
  // groen
  kleurtje[2].r = 8;
  kleurtje[2].g = 255;
  kleurtje[2].b = 8;
  // turqoise
  kleurtje[3].r = 8;
  kleurtje[3].g = 255;
  kleurtje[3].b = 255;
  // blauw
  kleurtje[4].r = 8;
  kleurtje[4].g = 8;
  kleurtje[4].b = 255;
  // paars
  kleurtje[5].r = 255;
  kleurtje[5].g = 8;
  kleurtje[5].b = 255;
  
  // paars
  regenboog[0].r = 255;
  regenboog[0].g = 8;
  regenboog[0].b = 255;
  // blauw
  regenboog[1].r = 170;
  regenboog[1].g = 8;
  regenboog[1].b = 255;
  regenboog[2].r = 85;
  regenboog[2].g = 8;
  regenboog[2].b = 255;
  regenboog[3].r = 8;
  regenboog[3].g = 8;
  regenboog[3].b = 255;
  // turqoise
  regenboog[4].r = 8;
  regenboog[4].g = 85;
  regenboog[4].b = 255;
  regenboog[5].r = 8;
  regenboog[5].g = 170;
  regenboog[5].b = 255;
  regenboog[6].r = 8;
  regenboog[6].g = 255;
  regenboog[6].b = 255;
  // groen
  regenboog[7].r = 8;
  regenboog[7].g = 255;
  regenboog[7].b = 170;
  regenboog[8].r = 8;
  regenboog[8].g = 255;
  regenboog[8].b = 85;
  regenboog[9].r = 8;
  regenboog[9].g = 255;
  regenboog[9].b = 8;
  // geel
  regenboog[10].r = 85;
  regenboog[10].g = 255;
  regenboog[10].b = 8;
  regenboog[11].r = 170;
  regenboog[11].g = 255;
  regenboog[11].b = 8;
  regenboog[12].r = 255;
  regenboog[12].g = 255;
  regenboog[12].b = 0;
  // rood
  regenboog[13].r = 255;
  regenboog[13].g = 170;
  regenboog[13].b = 8;
  regenboog[14].r = 255;
  regenboog[14].g = 85;
  regenboog[14].b = 8;
  regenboog[15].r = 255;
  regenboog[15].g = 8;
  regenboog[15].b = 8;
}

void loop() {

  // Check if the ESP32 is transmitting
  if (Serial1.available() >= 1) {
    // Read the JSON document from the "link" serial port
    err = deserializeJson(docIn, Serial1);

    if (err == DeserializationError::Ok) {
      
      berichtOntvangenOK = true;
      // (we must use as<T>() to resolve the ambiguity)
      mode = docIn["mode"].as<byte>();

      if (docIn["uur"].as<byte>() > 0) {
        struct tm timeinfo;
        timeinfo.tm_year = docIn["jaar"].as<byte>() + 1900;
        timeinfo.tm_mon = docIn["maand"].as<byte>();
        timeinfo.tm_mday = docIn["dag"].as<byte>();
        timeinfo.tm_hour = docIn["uur"].as<byte>();
        timeinfo.tm_min = docIn["minuut"].as<byte>();
        timeinfo.tm_sec = docIn["seconde"].as<byte>();
        rtc.adjust(DateTime(timeinfo.tm_year, timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
        bepaalZonOpOnder();
      } 
             
      bRood = docIn["rood"].as<byte>();
      bGroen = docIn["groen"].as<byte>();
      bBlauw = docIn["blauw"].as<byte>();
      kleur = Matrix_Color(bRood, bGroen, bBlauw);
      
      bSRood = docIn["srood"].as<byte>();
      bSGroen = docIn["sgroen"].as<byte>();
      bSBlauw = docIn["sblauw"].as<byte>();
      kleurSec = Matrix_Color(bSRood, bSGroen, bSBlauw);

      minBrightness = docIn["minBrightness"].as<byte>();
      maxBrightness = docIn["maxBrightness"].as<byte>();
      stepBrightness = docIn["stepBrightness"].as<byte>();
      startBrightness = docIn["startBrightness"].as<byte>();

      float _longitude = docIn["lengtegraad"].as<float>();
      float _latitude = docIn["breedtegraad"].as<float>();

      if (_longitude == 0) {
        longitude = 5.69;
        bepaalZonOpOnder();
      } else {
        if (longitude != _longitude) {
          longitude = _longitude;
          bepaalZonOpOnder();
        }
      }

      if (_latitude == 0) {
        latitude = 53.03;
        bepaalZonOpOnder();
      } else {
        if (latitude != _latitude) {
          latitude = _latitude;
          bepaalZonOpOnder();
        }
      }

      uuraan = docIn["uuraan"].as<byte>();
      uuruit = docIn["uuruit"].as<byte>();
      nachtsterkte = docIn["nachtsterkte"].as<byte>();

      wordclock = docIn["wordclock"].as<String>();
      effect = docIn["effect"].as<String>();
      
      if (gedimd) {
        effectOud = effect;
        effect = "geeneffect";
        modeOud = mode;
        mode = MODE_CLOCK;
      }

      maan = docIn["maan"].as<byte>(); 
      desOchtends = docIn["desOchtends"].as<byte>(); 
      hetIs = docIn["hetIs"].as<byte>(); 

      autoZomertijd = docIn["zomer"].as<byte>(); 

      eeprom_save();
   
    } else {
      // Print error to the "debug" serial port
      berichtOntvangenNOK = true;
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());

      // Flush all bytes in the "link" serial port buffer
      while (Serial1.available() >= 1) {
        Serial1.read();
      }
    }
  }

   
  if (millis() < _millis) {
    _next_1000_ms = millis();
  }

  _millis = millis();
  
  if (millis() >= _next_1000_ms) {
    _next_1000_ms = millis() + 1000L;
    vlagje = true;
  }

  // Update display
  update();
  
  if (vlagje) {
    // De update moet eerst
    sendStatus();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  vlagje = false;

}
