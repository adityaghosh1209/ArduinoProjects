/*
    ADXL345 Triple Axis Accelerometer. Activity & Inactivity
    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-akcelerometr-adxl345.html
    GIT: https://github.com/jarzebski/Arduino-ADXL345
    Web: http://www.jarzebski.pl
    (c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include <ADXL345.h>
#include <SPI.h>



//==========================================================================
#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_TEMPLATE_NAME "Home Security alert system"
//#define BLYNK_AUTH_TOKEN "KNuOLLX-KIBKPxgIjE-S_vz3AVYJ1JS3"
#define BLYNK_TEMPLATE_ID "TMPL33Ij55-m5"
#define BLYNK_TEMPLATE_NAME "Home Security alert system"
#define BLYNK_TEMPLATE_ID "TMPL33Ij55-m5"
#define BLYNK_TEMPLATE_NAME "Home Security alert system"
//#define BLYNK_AUTH_TOKEN "KNuOLLX-KIBKPxgIjE-S_vz3AVYJ1JS3"
#define BLYNK_DEVICE_NAME "ESP8266 D1mini"
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
//#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
#define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"
//#include <BlynkTimer.h> 



// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings



// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
WidgetLED led1(V3);

BlynkTimer timer;

// V3 LED Widget is blinking
void blinkLedWidget()
{
  if (led1.getValue()) {
    led1.off();
    Serial.println("LED on V3: off");
  } else {
    led1.on();
    Serial.println("LED on V3: on");
  }
}


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <BlynkSimpleEsp8266.h>

//char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "aditya";
//char pass[] = "55962001";
ADXL345 accelerometer;//(ADXL345_INT1);

BLYNK_CONNECTED(){
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

//====================================================================================//================================blynk stops here============================================

void myTimer() 
{
  // This function describes what will happen with each timer tick
  // e.g. writing sensor value to datastream V5
  Blynk.virtualWrite(V2, (millis() / 1000)+"sec");
}

void setup() 
{
  Serial.begin(9600);
  //==============================================================
  //Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  BlynkEdgent.begin();
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimer); 
   timer.setInterval(1000L, blinkLedWidget);
  // Initialize ADXL345
  Serial.println("Initialize ADXL345");
//========================================================================
  if (!accelerometer.begin())
  {
    Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
    delay(500);
  }

  // Values for Activity and Inactivity detection
  accelerometer.setActivityThreshold(1.2);    // Recommended 2 g
  accelerometer.setInactivityThreshold(1.2);  // Recommended 2 g
  accelerometer.setTimeInactivity(5);         // Recommended 5 s

  // Set activity detection only on X,Y,Z-Axis
  //accelerometer.setActivityXYZ(1);         // Check activity on X,Y,Z-Axis
  // or
   accelerometer.setActivityX(1);        // Check activity on X_Axis
   accelerometer.setActivityY(1);        // Check activity on Y-Axis
  // accelerometer.setActivityZ(1);        // Check activity on Z-Axis

  // Set inactivity detection only on X,Y,Z-Axis
  accelerometer.setInactivityXYZ(1);       // Check inactivity on X,Y,Z-Axis
  // or
  // accelerometer.setInactivityX(1);      // Check inactivity on X_Axis
  // accelerometer.setInactivityY(1);      // Check inactivity on Y-Axis
  // accelerometer.setInactivityZ(1);      // Check inactivity on Z-Axis

  //================================ tap setup =========================
     // Set tap detection on Z-Axis
  accelerometer.setTapDetectionX(0);       // Don't check tap on X-Axis
  accelerometer.setTapDetectionY(0);       // Don't check tap on Y-Axis
  accelerometer.setTapDetectionZ(1);       // Check tap on Z-Axis
  // or
  // accelerometer.setTapDetectionXYZ(1);  // Check tap on X,Y,Z-Axis

  accelerometer.setTapThreshold(2.5);      // Recommended 2.5 g
  accelerometer.setTapDuration(0.02);      // Recommended 0.02 s
  accelerometer.setDoubleTapLatency(0.10); // Recommended 0.10 s
  accelerometer.setDoubleTapWindow(0.30);  // Recommended 0.30 s
//===================================== end tap========================================
  // Select INT 1 for get activities
  accelerometer.useInterrupt(ADXL345_INT1);


  // Check settings
  checkSetup();
}

void checkSetup()
{
  Serial.print("Activity Threshold = "); Serial.println(accelerometer.getActivityThreshold());
  Serial.print("Inactivity Threshold = "); Serial.println(accelerometer.getInactivityThreshold());
  Serial.print("Time Inactivity = "); Serial.println(accelerometer.getTimeInactivity());

  Serial.print("Look activity on axis = "); 
  if (accelerometer.getActivityX()) { Serial.print(" X "); }
  if (accelerometer.getActivityY()) { Serial.print(" Y "); }
  if (accelerometer.getActivityZ()) { Serial.print(" Z "); }
  Serial.println();

  Serial.print("Look inactivity on axis = "); 
  if (accelerometer.getInactivityX()) { Serial.print(" X "); }
  if (accelerometer.getInactivityY()) { Serial.print(" Y "); }
  if (accelerometer.getInactivityZ()) { Serial.print(" Z "); }
  Serial.println();  


  //========================= tap ========================================
  Serial.print("Look tap on axis = "); 
  if (accelerometer.getTapDetectionX()) { Serial.print(" X "); }
  if (accelerometer.getTapDetectionY()) { Serial.print(" Y "); }
  if (accelerometer.getTapDetectionZ()) { Serial.print(" Z "); }
  Serial.println();

  Serial.print("Tap Threshold = "); Serial.println(accelerometer.getTapThreshold());
  Serial.print("Tap Duration = "); Serial.println(accelerometer.getTapDuration());
  Serial.print("Double Tap Latency = "); Serial.println(accelerometer.getDoubleTapLatency());
  Serial.print("Double Tap Window = "); Serial.println(accelerometer.getDoubleTapWindow());
  //============================= end tap ==========================
}
int count = 0;
void loop() 
{

  //Blynk.run();
  BlynkEdgent.run();
  //timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  // Read values for activities
  //delay(50);
  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0UL;
  unsigned long interval = 1000UL;
  unsigned long currentMillis1 = millis();
  unsigned long previousMillis1 = 0UL;
  unsigned long interval1 = 1000UL;
  
  if(currentMillis - previousMillis > interval){
    Vector norm = accelerometer.readNormalize();
  
    // Read activities
    Activites activ = accelerometer.readActivites();
    
    if (activ.isDoubleTap)
    {
      Serial.println("Double Tap Detected");
      Blynk.logEvent("hello", "Double Tap Detected") ;
      //Blynk.notify("Double Tap Detected");
    } else
    if (activ.isTap)
    {
      Serial.println("Tap Detected");
      Blynk.logEvent("hello", "Tap Detected") ;
      //Blynk.notify("Tap Detected");
    }
    
    if (activ.isActivity)
    {
      Serial.println("Activity Detected");
    }
  
    

    if(Serial.available()){
      String relay = "";
      relay = Serial.readStringUntil('\n');
      Blynk.virtualWrite(V1,relay);
    }
  }
}