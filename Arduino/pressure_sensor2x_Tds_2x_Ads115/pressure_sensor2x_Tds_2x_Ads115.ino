

/* This example demonstrates how to take a standard 3-wire pressure transducer
 *  and read the analog signal, then convert the signal to a readable output and
 *  display it onto an LCD screen.
 *  
 *  Contact Tyler at tylerovens@me.com if you have any questions
 */

//#include "Wire.h" //allows communication over i2c devices

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <Adafruit_MCP23017.h>
//==========================================begin TDS  variables 1115==========================================
#define TdsSensorPin A0
#define VREF 5.0 // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point

//float analogBuffer[SCOUNT]; // store the analog value in the array, read from ADC
//float analogBufferTemp[SCOUNT];
//int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;
float TDSValue = 0; //variable to store TDS integer value
float ec = 0, tds = 0, ecCalibration = 1.1500;


float averageVoltage1 = 0,tdsValue1 = 0,temperature1 = 25;
float TDSValue1 = 0; //variable to store TDS integer value
float ec1 = 0, tds1 = 0, ecCalibration1 = 1.1500;

//===========================================================================================
#define MCP_LED1 1
#define MCP_INPUTPIN 4
#define MCP_LEDTOG1 3
#define MCP_LEDTOG2 2

Adafruit_MCP23017 mcp;

//============================================================================================

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D4

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

 float Celcius=0;
 float Fahrenheit=0;
/* /==========================================begin tds variables di mini
#define TdsSensorPin A0
#define VREF1 5.0 // analog reference voltage(Volt) of the ADC
#define SCOUNT1 30 // sum of sample point
int analogBuffer1[SCOUNT1]; // store the analog value in the array, read from ADC
int analogBufferTemp1[SCOUNT1];
int analogBufferIndex1 = 0,copyIndex1 = 0;
int averageVoltage1 = 0,tdsValue1 = 0,temperature1 = 25;
int TDSValue1 = 0; //variable to store TDS integer value
//=====================================end TDS variables ==================================*/

Adafruit_ADS1115 ads(0x48); // Analog to I2C bus 1


//==============================pressure senore variables===============================================
const int pressureInput = A0; //select the analog input pin for the pressure transducer
const int pressureZero = 102.4; //analog reading of pressure transducer at 0psi
const int pressureMax = 921.6; //analog reading of pressure transducer at 100psi
const int pressuretransducermaxPSI = 100; //psi value of transducer being used
const int baudRate = 9600; //constant integer to set the baud rate for serial monitor
const int sensorreadDelay = 250; //constant integer to set the sensor read delay in milliseconds

int16_t pressureValue = 0; //variable to store the value coming from the pressure transducer

int16_t pressureValue1 = 0; //variable to store the value coming from the pressure transducer

//============================ end pressure sensor variables ================================

void setup() //setup routine, runs once when system turned on or reset
{
  Serial.begin(baudRate); //initializes serial communication at set baud rate bits per second
  //lcd.begin(); //initializes the LCD screen
  ads.begin(); // begin 1115
  //=================================================
  
  mcp.begin();      // Default device address 0

  mcp.pinMode(MCP_LEDTOG1, OUTPUT);  // Toggle LED 1
  mcp.pinMode(MCP_LEDTOG2, OUTPUT);  // Toggle LED 2

  mcp.pinMode(MCP_LED1, OUTPUT);     // LED output
  mcp.digitalWrite(MCP_LED1, HIGH);

  //======================================

  Serial.begin(9600);
  sensors.begin();
}

void loop() //loop routine runs over and over again forever
{
  //========================= begin pressure sensor on A2 =================================
  pressureValue = ads.readADC_SingleEnded(2); //reads value from input pin and assigns to variable
  Serial.print("A2 value"); //prints value from previous line to serial
  Serial.print(pressureValue); //prints value from previous line to serial
  Serial.print("     "); //prints value from previous line to serial
  pressureValue = ((pressureValue-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero); //conversion equation to convert analog reading to psi
  Serial.print(pressureValue, 1); //prints value from previous line to serial
  Serial.println("psi"); //prints label to serial
  //========================= end pressure sensor on A2 =================================
Serial.println(".......");
delay(sensorreadDelay); //delay in milliseconds between read values
   //========================= begin pressure sensor on A3 =================================
  pressureValue1 = ads.readADC_SingleEnded(3); //reads value from input pin and assigns to variable
  Serial.print("A3 value"); //prints value from previous line to serial
  Serial.print(pressureValue1); //prints value from previous line to serial
  Serial.print("     "); //prints value from previous line to serial
  pressureValue1 = ((pressureValue1-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero); //conversion equation to convert analog reading to psi
  Serial.print(pressureValue1, 1); //prints value from previous line to serial
  Serial.println("psi"); //prints label to serial
  //========================= end pressure sensor on A3 ================================= 
  
 
  delay(sensorreadDelay); //delay in milliseconds between read values

//========================= begin TDS sensor for 1115================================= 
TDSValue = ads.readADC_SingleEnded(1)* 0.000125;; //reads value from input pin and assigns to variable

float tempCoefficient = 1.0 + 0.02 * (temperature - 25.0);          // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
float voltageComp = (TDSValue / tempCoefficient) * ecCalibration;                     // temperature and calibration compensation
tdsValue = (133.42 * pow(voltageComp, 3) - 255.86 * pow(voltageComp, 2) + 857.39 * voltageComp) * 0.5; // convert voltage value to tds value


Serial.print("A1 TDS Value:");
Serial.print(TDSValue);
Serial.print("           TDS Value:");
Serial.print(tdsValue,0);
Serial.println("ppm");

//========================= end TDS sensor ================================= 

//========================= begin TDS sensor1 for 1115================================= 
TDSValue1 = ads.readADC_SingleEnded(0)* 0.000125;; //reads value from input pin and assigns to variable

float tempCoefficient1 = 1.0 + 0.02 * (temperature1 - 25.0);          // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
float voltageComp1 = (TDSValue1 / tempCoefficient1) * ecCalibration1;                     // temperature and calibration compensation
tdsValue1 = (133.42 * pow(voltageComp1, 3) - 255.86 * pow(voltageComp1, 2) + 857.39 * voltageComp1) * 0.5; // convert voltage value to tds value


Serial.print("A0 TDS Value:");
Serial.print(TDSValue1);
Serial.print("           TDS Value:");
Serial.print(tdsValue1,0);
Serial.println("ppm");

//========================= end TDS sensor ================================= 
delay(300);

  mcp.digitalWrite(MCP_LEDTOG1, HIGH);
  mcp.digitalWrite(MCP_LEDTOG2, LOW);

  delay(300);

  mcp.digitalWrite(MCP_LEDTOG1, LOW);
  mcp.digitalWrite(MCP_LEDTOG2, HIGH);

  // Transfer input pin state to LED1
  if (mcp.digitalRead(MCP_INPUTPIN)) {
     mcp.digitalWrite(MCP_LED1,HIGH);
  } else {
     mcp.digitalWrite(MCP_LED1,LOW);
  }
  //========================================================================
  sensors.requestTemperatures(); 
  delay(1000);
  Celcius=sensors.getTempCByIndex(0);
  Fahrenheit=sensors.toFahrenheit(Celcius);
  Serial.print(" C  ");
  Serial.print(Celcius);
  Serial.print(" F  ");
  Serial.println(Fahrenheit);
}
/*/========================= end TDS sensor ================================= 
}
float getMedianNum(float bArray[], int iFilterLen)
{
float bTab[iFilterLen];
for (byte i = 0; i<iFilterLen; i++)
bTab[i] = bArray[i];
int i, j, bTemp;
for (j = 0; j < iFilterLen - 1; j++)
{
for (i = 0; i < iFilterLen - j - 1; i++)
{
if (bTab[i] > bTab[i + 1])
{
bTemp = bTab[i];
bTab[i] = bTab[i + 1];
bTab[i + 1] = bTemp;
}
}
}
if ((iFilterLen & 1) > 0)
bTemp = bTab[(iFilterLen - 1) / 2];
else
bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
return bTemp;
}

int getMedianNum1(int bArray[], int iFilterLen)
{
int bTab[iFilterLen];
for (byte i = 0; i<iFilterLen; i++)
bTab[i] = bArray[i];
int i, j, bTemp;
for (j = 0; j < iFilterLen - 1; j++)
{
for (i = 0; i < iFilterLen - j - 1; i++)
{
if (bTab[i] > bTab[i + 1])
{
bTemp = bTab[i];
bTab[i] = bTab[i + 1];
bTab[i + 1] = bTemp;
}
}
}
if ((iFilterLen & 1) > 0)
bTemp = bTab[(iFilterLen - 1) / 2];
else
bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
return bTemp;
}*/


/*
 //========================= begin TDS sensor di mini ================================= 
TDSValue1 = analogRead(TdsSensorPin);  //reads value from input pin and assigns to variable
static unsigned long analogSampleTimepoint1 = millis();
if(millis()-analogSampleTimepoint1 > 40U) //every 40 milliseconds,read the analog value from the ADC
{
analogSampleTimepoint1 = millis();
analogBuffer1[analogBufferIndex1] = analogRead(TdsSensorPin); //read the analog value and store into the buffer
analogBufferIndex1++;
if(analogBufferIndex1 == SCOUNT1)
analogBufferIndex1 = 0;
}
static unsigned long printTimepoint1 = millis();
if(millis()-printTimepoint1 > 800U)
{
printTimepoint1 = millis();
for(copyIndex1=0;copyIndex1<SCOUNT1;copyIndex1++)
analogBufferTemp1[copyIndex1]= analogBuffer1[copyIndex1];
averageVoltage1 = getMedianNum1(analogBufferTemp1,SCOUNT1) * (float)VREF1/ 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
float compensationCoefficient1=1.0+0.02*(temperature1-25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
float compensationVolatge1=averageVoltage1/compensationCoefficient1; //temperature compensation
tdsValue1=(133.42*compensationVolatge1*compensationVolatge1*compensationVolatge1 - 255.86*compensationVolatge1*compensationVolatge1 + 857.39*compensationVolatge1)*0.5; //convert voltage value to tds value
//Serial.print("voltage:");
//Serial.print(averageVoltage,2);
//Serial.print("V ");
Serial.print("d1 mini TDS Value:");
Serial.print(TDSValue1);
Serial.print("           TDS Value:");
Serial.print(tdsValue1,0);
Serial.println("ppm");


 */
