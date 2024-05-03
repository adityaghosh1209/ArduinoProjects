/*





*/ 
#include <LiquidCrystal_I2C.h> //LCD

#include <OneWire.h> // required for Dallas Temperature 
#include <DallasTemperature.h> // a headder library used to get readings of the temperature sensor

#include <Wire.h> //allows communication over i2c devices
#include <Adafruit_ADS1015.h> // required for the ANALOG CONVERTER sensor | USE 1.1.0 VERSION OF THE LIBRARY
#include <Adafruit_MCP23017.h> // required for the Output entender bord

//=========================================================================================================

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 
//==========================================begin TDS  variables 1115==========================================
#define TdsSensorPin A0 //specify where the tds is present
#define TdsSensorPin1 A1 //specify where the tds is present
#define PresiureSensorPin A2 //specify where the Pressure sensor is present
#define PresiureSensorPin1 A3 //specify where the Pressure sensor is present

#define VREF 5.0 // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point


// for the tds no.1
float averageVoltage = 0,tdsValue = 0,temperature = 25;
float TDSValue = 0; //variable to store TDS integer value
float ec = 0, tds = 0, ecCalibration = 1.1500;

//for the tds no.2
float averageVoltage1 = 0,tdsValue1 = 0,temperature1 = 25;
float TDSValue1 = 0; //variable to store TDS integer value
float ec1 = 0, tds1 = 0, ecCalibration1 = 1.1500;

//======================================MCP=====================================================
#define MCP_LED1 1 //led1
#define MCP_LEDTOG1 3 //led3
#define MCP_LEDTOG2 2 //led2

Adafruit_MCP23017 mcp; //mcp

//============================================================================================

#define ONE_WIRE_BUS D4 //specifies there the temperature sensor is connected ON THE D1 MINI

OneWire oneWire(ONE_WIRE_BUS); // i dont know why but crutial

DallasTemperature sensors(&oneWire); //sensor refrence?

 float Celcius=0; //variables of common use
 float Fahrenheit=0; //variables of common use
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
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  
  //===============================================================================
  
  
  Serial.begin(baudRate); //initializes serial communication at set baud rate bits per second
  //lcd.begin(); //initializes the LCD screen
  ads.begin(); // begin 1115
  
  mcp.digitalWrite(MCP_LEDTOG1, HIGH); //adscheckup high
  //=================================================
  
  mcp.begin();      // Default device address 0

  mcp.pinMode(MCP_LEDTOG1, OUTPUT);  // Toggle LED 3
  mcp.pinMode(MCP_LEDTOG2, OUTPUT);  // Toggle LED 2

  mcp.pinMode(MCP_LED1, OUTPUT);     // LED 1 output
  
  //======================================

  sensors.begin(); //temperature sensor startup
  mcp.digitalWrite(MCP_LED1, HIGH); //led1 toggle high

  delay(1000);

  mcp.digitalWrite(MCP_LED1, LOW); //led1 toggle low
  mcp.digitalWrite(MCP_LEDTOG2, LOW); //led2 toggle low
  mcp.digitalWrite(MCP_LEDTOG1, LOW); //led3 toggle low

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
mcp.digitalWrite(MCP_LEDTOG1, HIGH); //adscheckup high
delay(sensorreadDelay); //delay in milliseconds between read values
  mcp.digitalWrite(MCP_LEDTOG1, LOW); //adscheckup high
   //========================= begin pressure sensor on A3 =================================
  pressureValue1 = ads.readADC_SingleEnded(3); //reads value from input pin and assigns to variable
  Serial.print("A3 value"); //prints value from previous line to serial
  Serial.print(pressureValue1); //prints value from previous line to serial
  Serial.print("     "); //prints value from previous line to serial
  pressureValue1 = ((pressureValue1-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero); //conversion equation to convert analog reading to psi
  Serial.print(pressureValue1, 1); //prints value from previous line to serial
  Serial.println("psi"); //prints label to serial
  //========================= end pressure sensor on A3 ================================= 
    mcp.digitalWrite(MCP_LEDTOG1, HIGH); //adscheckup high
  delay(sensorreadDelay); //delay in milliseconds between read values
  mcp.digitalWrite(MCP_LEDTOG1, LOW); //adscheckup high
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
mcp.digitalWrite(MCP_LEDTOG2, HIGH); //adscheckup high
delay(sensorreadDelay); //delay in milliseconds between read values
mcp.digitalWrite(MCP_LEDTOG2, LOW); //adscheckup high
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
mcp.digitalWrite(MCP_LEDTOG2, HIGH); //adscheckup high
delay(sensorreadDelay); //delay in milliseconds between read values
mcp.digitalWrite(MCP_LEDTOG2, LOW); //adscheckup high
//========================= end TDS sensor ================================= 
  sensors.requestTemperatures(); 
  delay(1000);
  Celcius=sensors.getTempCByIndex(0);
  Fahrenheit=sensors.toFahrenheit(Celcius);
  Serial.print(" C  ");
  Serial.print(Celcius);
  Serial.print(" F  ");
  Serial.println(Fahrenheit);
  mcp.digitalWrite(MCP_LED1, HIGH); //adscheckup high
  delay(sensorreadDelay); //delay in milliseconds between read values
  mcp.digitalWrite(MCP_LED1, LOW); //adscheckup high
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
