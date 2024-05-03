/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/


#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1106.h>
//=======================new 
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
//#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"
#include "SH1106Wire.h"   // legacy: #include "SH1106.h"
//===================== new


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     0 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

SH1106Wire display(0x3c, SDA, SCL);     // ADDRESS, SDA, SCL ================== new ================

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#define stopbutton D4
bool activekeep = false; 
void setup() {
  Serial.begin(115200);
pinMode(stopbutton,INPUT);

  display.init(); //=================== new

  display.flipScreenVertically(); // ===================== new
 /*
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
        display.clearDisplay();
  }

*/  
}
void loop(){
  display.clear();
        display.setColor(WHITE);
        display.setFont(ArialMT_Plain_10);
        display.fillRect(2/*x start*/,3/*y start*/,126/*width*/,8/*height*/);//draws a bar on top      
        //display.setTextSize(1);//text size
        display.setColor(BLACK);//text colour (black or white)
        display.drawString(10, 0, "Aditya Inc.");
        //display.setCursor(10, 2);//place the cursor on the bar on the top
        //display.println("Aditya Inc.");//prints Aditya Inc. on the bar from the point where the cursor is placed
        display.setColor(WHITE);  
        display.fillRect(2/*x start*/,56/*y start*/,126/*width*/,62/*hieght*/);//draws a bar on bottom
        //display.setTextSize(1);
        display.setColor(BLACK);
        //display.setCursor(10, 56);//place cursor on the bottom bar
        //display.println("Support: 1800800800");
        display.drawString(10, 57, "Support: 1800800800");
              

        display.setColor(WHITE);
         //progress bar in between with border +2 on all sides
        display.drawRect(2/*x start*/,13/*y start*/,25/*width*/,8/*hieght*/);//draws the border for first rectancle(upper left side)
        display.fillRect(2/*x start*/,13/*y start*/,15/*width*/,8/*hieght*/);//draws the fill (progress bar) rectangle on the border rectangle
        //display.setTextSize(1);
        //display.setTextColor(WHITE);
        //display.setCursor(30, 13); // after three of the first rectangle((25+2)+3)
        //display.println("50%");// liquid level based on pressure value 
         display.drawString(30, 11, "50%");


        //display.setTextSize(1);
        //display.setTextColor(WHITE);
        //display.setCursor(2, 23); // set cursor below first rectangle(((2+8)+3+10)+3)
        //display.println("tank 1");// label
        display.drawString(2, 21, "Tank 1");

        display.drawRect(75/*x start*/,13/*y start*/,25/*width*/,8/*hieght*/);//draws the border for the second one(upper right)
        display.fillRect(75/*x start*/,13/*y start*/,15/*width*/,8/*hieght*/);//draws the fill for the second one
        display.drawString(75, 20,"Tank 2");
        //display.setCursor(103, 13, "50%");

        display.drawRect(2/*x start*/,34/*y start*/,25/*width*/,6/*hieght*/);//draws the border for the third one(lower bottom)
        display.fillRect(2/*x start*/,34/*y start*/,15/*width*/,6/*hieght*/);//draws the fill

        display.drawRect(2/*x start*/,34/*y start*/,25/*width*/,6/*hieght*/);//draws the border for the third one(lower bottom)
        display.fillRect(2/*x start*/,34/*y start*/,15/*width*/,6/*hieght*/);//draws the fill
        display.fillCircle(30, 36, 4);
        display.drawString(39, 31,"24 C");
        display.drawString(2, 42,"Temp");
        display.drawString(103, 11,"50 %");
        //display.setCursor(103, 13);//places cursor beside
        //display.println("50%");//label


        display.drawRect(75/*x start*/,33/*y start*/,25/*width*/,8/*hieght*/);
        display.fillRect(90/*x start*/,33/*y start*/,10/*width*/,8/*hieght*/);
        display.drawString(103, 30,"ON");
        display.drawString(75, 42,"Pump");

        
  display.display();
  delay(10);
  
  
  }
