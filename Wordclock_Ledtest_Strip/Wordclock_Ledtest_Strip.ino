// Simple NeoPixel test.  Lights just a few pixels at a time so a
// 1m strip can safely be powered from Arduino 5V pin.  Arduino
// may nonetheless hiccup when LEDs are first connected and not
// accept code.  So upload code first, unplug USB, connect pixels
// to GND FIRST, then +5V and digital pin 6, then re-plug USB.
// A working strip will show a few pixels moving down the line,
// cycling between red, green and blue.  If you get no response,
// might be connected to wrong end of strip (the end wires, if
// any, are no indication -- look instead for the data direction
// arrows printed on the strip).
 
#include <Adafruit_NeoPixel.h>
 
#define PIN      30
#define N_LEDS 16 

int aantal = 0;
int tellerop = 0;
int tellerneer = 0;
boolean ledsOp = true;
 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);
 
void setup() {
  strip.begin();
  setAll(0);  // Initialize all pixels to 'off'
  strip.show();
}
 
void loop() {
  
  if (aantal < 64) {
    chase(strip.Color(255, 0, 0)); // Red
  } else {
    setAll(strip.Color(255, 255, 255));  
    strip.show();
    delay(1000);
  
    setAll(strip.Color(255, 0, 0));  
    strip.show();
    delay(1000);
  
    setAll(strip.Color(0, 255, 0));  
    strip.show();
    delay(1000);
  
    setAll(strip.Color(0, 0, 255));  
    strip.show();
    delay(1000);
    
    aantal = 0;
    
  }
 }
 
static void chase(uint32_t c) {

  // Op
  if (tellerop < 1) {
    ledsOp = true;
    tellerneer = tellerop + 5;
  }
  
  // Neer
  if (tellerop > N_LEDS - 2) {
    ledsOp = false;
    tellerneer = tellerop - 5;
  }

  if (ledsOp) {
    if (tellerneer > 0) {
      tellerneer--;
      strip.setPixelColor(tellerneer, 0); 
      strip.setPixelColor(tellerneer-1, strip.Color(255-250, 0, 0)); 
      strip.setPixelColor(tellerneer-2, strip.Color(255-225, 0, 0)); 
    }
  } else {
    if (tellerneer < N_LEDS) {
      tellerneer++;
      strip.setPixelColor(tellerneer, 0); 
      strip.setPixelColor(tellerneer+1, strip.Color(255-250, 0, 0)); 
      strip.setPixelColor(tellerneer+2, strip.Color(255-225, 0, 0)); 
    }
  }

  if (ledsOp) {
    tellerop++;
    strip.setPixelColor(tellerop  , c); // Draw new pixel
    strip.setPixelColor(tellerop-1, strip.Color(255-150, 0, 0)); 
    strip.setPixelColor(tellerop-2, strip.Color(255-200, 0, 0)); 
    strip.setPixelColor(tellerop-3, strip.Color(255-225, 0, 0)); 
    strip.setPixelColor(tellerop-4, strip.Color(255-250, 0, 0)); 
    strip.setPixelColor(tellerop-5, 0); 
  } else {
    tellerop--;
    strip.setPixelColor(tellerop  , c); // Draw new pixel
    strip.setPixelColor(tellerop+1, strip.Color(255-150, 0, 0)); 
    strip.setPixelColor(tellerop+2, strip.Color(255-200, 0, 0)); 
    strip.setPixelColor(tellerop+3, strip.Color(255-225, 0, 0)); 
    strip.setPixelColor(tellerop+4, strip.Color(255-250, 0, 0)); 
    strip.setPixelColor(tellerop+5, 0); 
  }

  aantal++;
  strip.show();
  delay(150);
}

void setAll(uint32_t c) {
  for(int i = 0; i < N_LEDS; i++ ) {
    strip.setPixelColor(i, c);
  }
}
