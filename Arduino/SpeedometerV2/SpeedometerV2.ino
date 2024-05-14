/************************************************************************************
 *  Created By: Tauseef Ahmad
 *  Created On: 7 June, 2022
 *  
 *  YouTube Video: https://youtu.be/gKuJxjxNP-k
 *  My Channel: https://www.youtube.com/channel/UCOXYfOHgu-C-UfGyDcu5sYw/
 *  
 *  *********************************************************************************
 *  Preferences--> Aditional boards Manager URLs : 
 *  For ESP32:
 *  https://dl.espressif.com/dl/package_esp32_index.json
 * ********************************************************************************/
//---------------------------------------------------------------------------
#include "FastLED.h"
#define NUM_LEDS 60
CRGB leds[NUM_LEDS];
#define PIN 6
int count = 1;
//---------------------------------------------------------------------------
#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);
//---------------------------------------------------------------------------
#include <SoftwareSerial.h>
#define rxPin 2
#define txPin 3 //unused
SoftwareSerial neogps(rxPin,txPin);
//---------------------------------------------------------------------------
#include <TinyGPS++.h> //1.0.3
TinyGPSPlus gps;
//---------------------------------------------------------------------------
int x_max    = 128;       //OLED display width, in pixels
int y_max    = 62;        //OLED display width, in pixels
int x_center = x_max/2;
int y_center = y_max/2+10;
int arc      = y_max/2;
int angle    = 0;
//---------------------------------------------------------------------------
int needle_pos = 0;
u8g_uint_t xx  = 0;
//---------------------------------------------------------------------------
//satellite logo
#define sat_logo_width 20
#define sat_logo_height 20

const unsigned char sat_logo[] = {
  0x00, 0x01, 0x00, 0x80, 0x07, 0x00, 0xc0, 0x06, 0x00, 0x60, 0x30, 0x00,
  0x60, 0x78, 0x00, 0xc0, 0xfc, 0x00, 0x00, 0xfe, 0x01, 0x00, 0xff, 0x01,
  0x80, 0xff, 0x00, 0xc0, 0x7f, 0x06, 0xc0, 0x3f, 0x06, 0x80, 0x1f, 0x0c,
  0x80, 0x4f, 0x06, 0x19, 0xc6, 0x03, 0x1b, 0x80, 0x01, 0x73, 0x00, 0x00,
  0x66, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x70, 0x00, 0x00
};
//---------------------------------------------------------------------------

//Program variables
double lat;
double lng;
//int day, month, year;
String hour, minute;
int second;

int num_sat, speed;
String heading;


/*******************************************************************************************
 * gauge function
 * dispay gauge and other gps data on oled
*******************************************************************************************/
void gauge(uint8_t angle) {
  //fonts: u8g_font_chikita - u8g_font_04b_03br
  // u8g_font_orgv01 - u8g_font_freedoomr10r
  u8g.setFont(u8g_font_chikita);
  //---------------------------------------------------------------------------
  // draw border of the gauge
  u8g.drawCircle(x_center,y_center,arc+6, U8G_DRAW_UPPER_RIGHT);
  u8g.drawCircle(x_center,y_center,arc+4, U8G_DRAW_UPPER_RIGHT);
  u8g.drawCircle(x_center,y_center,arc+6, U8G_DRAW_UPPER_LEFT);
  u8g.drawCircle(x_center,y_center,arc+4, U8G_DRAW_UPPER_LEFT);
  //---------------------------------------------------------------------------
  // show gauge values
  u8g.drawStr(20,  42, "0");   
  u8g.drawStr(18,  29, "25");
  u8g.drawStr(28,  14, "50");
  u8g.drawStr(60,  14, "100");
  u8g.drawStr(91,  14, "150");
  u8g.drawStr(101, 29, "175");
  u8g.drawStr(105, 42, "200"); 
  //---------------------------------------------------------------------------
  // show gauge label
  u8g.setPrintPos(54,25);
  u8g.print("km/h");
  u8g.setPrintPos(50,32);
  u8g.print("SPEED");
  //---------------------------------------------------------------------------
  // draw the needle
  float x1=sin(2*angle*2*3.14/360);           
  float y1=cos(2*angle*2*3.14/360); 
  u8g.drawLine(x_center, y_center, x_center+arc*x1, y_center-arc*y1);
  u8g.drawDisc(x_center, y_center, 5, U8G_DRAW_UPPER_LEFT);
  u8g.drawDisc(x_center, y_center, 5, U8G_DRAW_UPPER_RIGHT);
  //---------------------------------------------------------------------------
  if (gps.location.isValid() == 1) {
  //TOP LEFT: draw satellite logo and number of satellites
    u8g.drawXBM(0, 0, sat_logo_width, sat_logo_height, sat_logo);
    u8g.setPrintPos(18, 5);
    u8g.print(num_sat, 5);
  }
  //---------------------------------------------------------------------------
  //TOP RIGHT: Display direction
  u8g.setPrintPos(110, 5);
  u8g.print(heading);
  //---------------------------------------------------------------------------
  //Display latitude and longitude
  u8g.setPrintPos(0, 55);
  u8g.print(lat, 4);
  u8g.setPrintPos(0, 62);
  u8g.print(lng, 4);
  //---------------------------------------------------------------------------
  //Display time
  u8g.setFont(u8g_font_freedoomr10r);
  u8g.setPrintPos(90, 65);
  u8g.print(hour);
  
  if(second%2 == 0) 
    {u8g.drawStr(104, 65, ":");}
  else
    {u8g.drawStr(104, 65, " ");}
    
  u8g.setPrintPos(111, 65);
  u8g.print(minute);
  
  //u8g.drawStr(90, 65, "00:00");
  //---------------------------------------------------------------------------
  // Show Speed and align its position
  u8g.setFont(u8g_font_profont22);
  u8g.setPrintPos(54,60);
  if (speed<10){                              
    u8g.print("0");
  }
  if (speed>99) {                                  
    u8g.setPrintPos(47,60);
  }
  u8g.print(speed);
  //---------------------------------------------------------------------------
}

/*******************************************************************************************
 * gauge function
 * dispay gauge and other gps data on oled
*******************************************************************************************/
int i = 200;
void setup(void) {
  Serial.begin(9600);

  neogps.begin(9600);
  
  u8g.setFont(u8g_font_chikita);
  u8g.setColorIndex(1);

  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
}

/*******************************************************************************************
 * gauge function
 * dispay gauge and other gps data on oled
*******************************************************************************************/
int speedFac = 0;
void loop(void){

  //----------------------------------------------------------
  Read_GPS();
  //----------------------------------------------------------
  needle_pos = map(speed,0,200,0,90); //SET NEEDLE
  // show needle and dial
  xx = needle_pos;                                    
  if (xx<45)
    {xx=xx+135;}
  else
    {xx=xx-45;} 
  //----------------------------------------------------------
  //Display Data on Oled
  {
    u8g.firstPage(); 
    do {             
      gauge(xx);
    }
    while( u8g.nextPage() );
  }
  //----------------------------------------------------------
  
  theaterChaseRainbow(100-speedFac);
  count++;
  if (count > 90)
    count = 1;

  //------------hard coded values-----------------------------
  /*
  if(count > 5)
    speedFac = 20;
  if(count > 10)
    speedFac = 40;
  if(count > 15)
    speedFac = 50;
  if(count > 20)
    speedFac = 60;
  if(count > 25)
    speedFac = 70;
  */
  Serial.println(speedFac);
  //---------------------------------------------------------------------
  
  //----------------------------implemented vals--------------------------
  ///*
  if(speed > 5)
    speedFac = 20;
  if(speed > 10)
    speedFac = 40;
  if(speed > 15)
    speedFac = 50;
  if(speed > 20)
    speedFac = 60;
  if(speed > 25)
    speedFac = 70;
  //*/
}




void Read_GPS(){
  //------------------------------------------------------------------
  boolean newData = false;
  //for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (neogps.available())
    {
      if (gps.encode(neogps.read()))
      {
        newData = true;
        break;
      }
    }
  }
  //------------------------------------------------------------------
  //If newData is true
  if(newData == true){
    newData = false;
    Get_GPS();
  }
  else { 
    //no data
  }
}

void Get_GPS(){
  num_sat = gps.satellites.value();

  if (gps.location.isValid() == 1) {
    speed = gps.speed.kmph();
    //Serial.print("Speed: ");Serial.println(speed);
    lat = gps.location.lat();
    //Serial.print("lat: ");Serial.println(lat);
    lng = gps.location.lng();
    //Serial.print("lng: ");Serial.println(lng);
    heading = gps.cardinal(gps.course.value());
    //Serial.print("heading: ");Serial.println(heading);
  }
  
  if (gps.time.isValid()){
    hour = String(gps.time.hour());
    hour = (hour.length() == 1) ? "0"+hour : hour;
    
    minute = String(gps.time.minute());
    minute= (minute.length() == 1) ? "0"+minute : minute;
    
    second = gps.time.second();
  }
}
//--------------------------------------------------------------
int j = 0;
void theaterChaseRainbow(int SpeedDelay) {
  byte *c;
  
  //for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < NUM_LEDS; i=i+3) {
          c = Wheel( (i+j) % 255);
          setPixel(i+q, *c, *(c+1), *(c+2));    //turn every third pixel on
          
        }
        showStrip();
       
        delay(SpeedDelay);
       
        for (int i=0; i < NUM_LEDS; i=i+3) {
          setPixel(i+q, 0,0,0);        //turn every third pixel off
        }
        
        
    }
    j++;
    if(j>256){
      j = 0;
    }
  //}
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

