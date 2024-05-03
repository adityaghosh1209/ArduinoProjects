/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     0 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#define stopbutton D4
bool activekeep = false;
void setup() {
  Serial.begin(115200);
  pinMode(stopbutton,INPUT);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
        display.clearDisplay();
  display.drawRect(0, 0, 20, 10, WHITE);
  display.fillRect(20, 2, 2, 4, WHITE);
  display.fillRect(2, 2, 6.6, 6, WHITE);

  display.fillRect(25, 0, 2, 10, WHITE);
  display.fillRect(28, 2, 2, 8, WHITE);
  display.fillRect(31, 4, 2, 6, WHITE);
  display.fillRect(34, 6, 2, 4, WHITE);

  display.drawRoundRect(40, 0, 20, 10, 5,  WHITE);
  display.fillCircle(45, 5, 4, WHITE);
 

  display.drawRoundRect(65, 0, 20, 10, 5,  WHITE);
  display.fillCircle(80, 5, 4, WHITE);
  display.display(); 
  delay(100);
  }
}
  void loop(){
    
display.clearDisplay();
  display.drawRect(0, 0, 20, 10, WHITE);
  display.fillRect(20, 2, 2, 4, WHITE);
  display.fillRect(2, 2, 6.6, 6, WHITE);

  display.fillRect(25, 0, 2, 10, WHITE);
  display.fillRect(28, 2, 2, 8, WHITE);
  display.fillRect(31, 4, 2, 6, WHITE);
  display.fillRect(34, 6, 2, 4, WHITE);

  display.drawRoundRect(40, 0, 20, 10, 5,  WHITE);
  display.fillCircle(45, 5, 4, WHITE);
 

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(62, 0);
  // Display static text
  display.println("M1");
  display.drawRoundRect(75, 0, 20, 10, 5,  WHITE);
  display.fillCircle(90, 5, 4, WHITE);
   display.display(); 
   
  delay(1000);

  display.clearDisplay();
  display.drawRect(0, 0, 20, 10, WHITE);
  display.fillRect(20, 2, 2, 4, WHITE);
  display.fillRect(2, 2, 13, 6, WHITE);

  display.fillRect(25, 0, 2, 10, WHITE);
  display.fillRect(28, 2, 2, 8, WHITE);
  display.fillRect(31, 4, 2, 6, WHITE);
  //display.fillRect(34, 6, 2, 4, WHITE);

  display.drawRoundRect(40, 0, 20, 10, 5,  WHITE);
  display.fillCircle(55, 5, 4, WHITE);
 

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(62, 0);
  // Display static text
  display.println("M1");
  display.drawRoundRect(75, 0, 20, 10, 5,  WHITE);
  display.fillCircle(90, 5, 4, WHITE);
  
   display.display(); 
  delay(1000);
  }
