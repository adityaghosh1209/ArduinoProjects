#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

#define RXPin 2
#define TXPin 3
#define GPSBaud 9600

double speed = 0.0;
// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

#define PIN_NEO_PIXEL  6   // Arduino pin that connects to NeoPixel
#define NUM_PIXELS     16  // The number of LEDs (pixels) on NeoPixel

Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);
int input = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ss.begin(GPSBaud);
  NeoPixel.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  NeoPixel.setBrightness(225); // a value from 0 to 255
}

int i = -7;

void loop() {
  // put your main code here, to run repeatedly:
  if(ss.available()>0){
    // Latitude in degrees (double)
      /*Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6);      
      // Longitude in degrees (double)
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6); 
      // Speed in kilometers per hour (double)*/
      //Serial.print("Speed in km/h = "); 
      speed = gps.speed.kmph();
      Serial.println(speed); 
  }

  clear();
  if(i <16){
    NeoPixel.setPixelColor(i, NeoPixel.Color(225, 0, 0));
    NeoPixel.setPixelColor(i+1, NeoPixel.Color(0, 225, 0));
    NeoPixel.setPixelColor(i+2, NeoPixel.Color(225, 225, 0));
    NeoPixel.setPixelColor(i+3, NeoPixel.Color(225, 0, 225));
    NeoPixel.setPixelColor(i+4, NeoPixel.Color(0, 225, 0));
    NeoPixel.setPixelColor(i+5, NeoPixel.Color(0, 225, 255));
    NeoPixel.setPixelColor(i+6, NeoPixel.Color(225, 0, 0));
    NeoPixel.setPixelColor(i+7, NeoPixel.Color(0, 225, 0));
    i++;
    
  }
  else
    i = -7;

  NeoPixel.show();
  if(speed <5)
  delay(100);
  else if(speed <10)
  delay(75);
  else if(speed <15)
  delay(50);
  else if(speed <20)
  delay(25);
  else
  delay(20);


}
void clear()
{
  // turn off all pixels for two seconds
  NeoPixel.clear();
  NeoPixel.show();  // send the updated pixel colors to the NeoPixel hardware.
}
