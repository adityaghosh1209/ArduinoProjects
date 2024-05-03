

/* This example demonstrates how to take a standard 3-wire pressure transducer
 *  and read the analog signal, then convert the signal to a readable output and
 *  display it onto an LCD screen.
 *  
 *  Contact Tyler at tylerovens@me.com if you have any questions
 */

//#include "Wire.h" //allows communication over i2c devices

#include <Wire.h>
#include <Adafruit_ADS1015.h>
//==========================================extender variables==========================================
#define TdsSensorPin A0
#define VREF 5.0 // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point
int analogBuffer[SCOUNT]; // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 0;

Adafruit_ADS1115 ads(0x48); // Analog to I2C bus 1
//#include "LiquidCrystal_I2C.h" //allows interfacing with LCD screens
//==============================pressure senore variables===============================================
const int pressureInput = A0; //select the analog input pin for the pressure transducer
const int pressureZero = 102.4; //analog reading of pressure transducer at 0psi
const int pressureMax = 921.6; //analog reading of pressure transducer at 100psi
const int pressuretransducermaxPSI = 100; //psi value of transducer being used
const int baudRate = 9600; //constant integer to set the baud rate for serial monitor
const int sensorreadDelay = 250; //constant integer to set the sensor read delay in milliseconds

int16_t pressureValue = 0; //variable to store the value coming from the pressure transducer



void setup() //setup routine, runs once when system turned on or reset
{
  Serial.begin(baudRate); //initializes serial communication at set baud rate bits per second
  //lcd.begin(); //initializes the LCD screen
  ads.begin();
}

void loop() //loop routine runs over and over again forever
{
  pressureValue = ads.readADC_SingleEnded(2); //reads value from input pin and assigns to variable
  Serial.print("A2 value"); //prints value from previous line to serial
  Serial.print(pressureValue); //prints value from previous line to serial
  Serial.print("     "); //prints value from previous line to serial
  pressureValue = ((pressureValue-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero); //conversion equation to convert analog reading to psi
  Serial.print(pressureValue, 1); //prints value from previous line to serial
  Serial.println("psi"); //prints label to serial
  /*lcd.setCursor(0,0); //sets cursor to column 0, row 0
  lcd.print("Pressure:"); //prints label
  lcd.print(pressureValue, 1); //prints pressure value to lcd screen, 1 digit on float
  lcd.print("psi"); //prints label after value
  lcd.print("   "); //to clear the display after large values or negatives*/
  delay(sensorreadDelay); //delay in milliseconds between read values
}
