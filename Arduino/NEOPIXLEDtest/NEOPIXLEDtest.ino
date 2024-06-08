#include <Adafruit_NeoPixel.h>

#define PIN_NEO_PIXEL  6   // Arduino pin that connects to NeoPixel
#define NUM_PIXELS     16  // The number of LEDs (pixels) on NeoPixel

Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);
int input = 0;
void setup() {
  // put your setup code here, to run once:
  NeoPixel.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  NeoPixel.setBrightness(225); // a value from 0 to 255
  Serial.begin(9600);
}
int i = -4; 
void loop() {
  clear();
  if(i <16){
    NeoPixel.setPixelColor(i, NeoPixel.Color(225, 0, 0));
    NeoPixel.setPixelColor(i+1, NeoPixel.Color(225, 225, 0));
    NeoPixel.setPixelColor(i+2, NeoPixel.Color(225, 0, 225));
    NeoPixel.setPixelColor(i+3, NeoPixel.Color(0, 225, 0));
    NeoPixel.setPixelColor(i+4, NeoPixel.Color(0, 0, 255));
    i++;
    
  }
  else
    i = -4;
  // put your main code here, to run repeatedly:
  
  NeoPixel.show();

  if(input < 1000)
    input+=25;
  else
    input = 0;
  Serial.println(1000-input);
  delay(1000-input);
}

void clear()
{
  // turn off all pixels for two seconds
  NeoPixel.clear();
  NeoPixel.show();  // send the updated pixel colors to the NeoPixel hardware.
}

int serialnput(){
  if(Serial.available()){
      String relay = "";
      relay = Serial.readStringUntil('\n');
      int pixel = relay.toInt();
      return pixel;
  }
  return;
}