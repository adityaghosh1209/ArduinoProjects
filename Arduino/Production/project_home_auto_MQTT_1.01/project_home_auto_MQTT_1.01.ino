/*
change log
21-jan - abhijit added code to save pressure config in json




*/ 
//#include <LiquidCrystal_I2C.h> //LCD

#include <OneWire.h> // required for Dallas Temperature 
#include <DallasTemperature.h> // a headder library used to get readings of the temperature sensor

#include <Wire.h> //allows communication over i2c devices
#include <Adafruit_ADS1X15.h> // required for the ANALOG CONVERTER sensor | USE 1.1.0 VERSION OF THE LIBRARY
#include <Adafruit_MCP23017.h> // required for the Output entender bord

#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>            //for mqtt
#include <ESP8266WebServer.h>     //for mqtt
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <PubSubClient.h>         //mqtt
#include <ArduinoJson.h>          //file access and storage
//=========================================================================================================

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
//LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 
//==========================================begin TDS  variables 1115==========================================
#define TdsSensorPin A0 //specify where the tds is present
#define TdsSensorPin1 A1 //specify where the tds is present
#define PresiureSensorPin A2 //specify where the Pressure sensor is present
#define PresiureSensorPin1 A3 //specify where the Pressure sensor is present

#define VREF 5.0 // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point

float Voltage = 0.0;//                       Temporary
String temp_str;//                           Values for 
//                                           Sensor reading to mqtt
char temp[50];// temperory sensor readings to mqtt
char hum[50];// temperory sensor readings to mqtt
char pressure_cfg1[5]; // store pressure1


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

//========================================mqtt================================================
bool shouldSaveConfig = false;
const char* ssid = "abcd";
const char* password = "abcd";
//#define mqtt_server   "abcd"
//"raspberrypi"
//"192.168.0.116"
//test.mosquitto.org
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
 
//======================================temperature sensor====================================

#define ONE_WIRE_BUS D4 //specifies there the temperature sensor is connected ON THE D1 MINI

OneWire oneWire(ONE_WIRE_BUS); // i dont know why but crutial

DallasTemperature sensors(&oneWire); //sensor refrence?

 float Celcius=0; //variables of common use
 float Fahrenheit=0; //variables of common use


//======================================ADS1115 jumper setting for 4 analog readings====================================

//Adafruit_ADS1115 ads(0x48); // Analog to I2C bus 1
Adafruit_ADS1115 ads; // Analog to I2C bus 1


//==============================pressure senore variables===============================================
//#define pressure_config1  "100"
//#define pressure_config2  "100"
//#define mqtt_server   "abcd"
char mqtt_server[40];
char pressure_config1[6] = "1010";
char pressure_config2[6] = "2020";

const int pressureInput = A0; //select the analog input pin for the pressure transducer
const int pressureZero = 102.4; //analog reading of pressure transducer at 0psi
const int pressureMax = 921.6; //analog reading of pressure transducer at 100psi
const int pressuretransducermaxPSI = 100; //psi value of transducer being used
const int baudRate = 9600; //constant integer to set the baud rate for serial monitor
const int sensorreadDelay = 250; //constant integer to set the sensor read delay in milliseconds

int16_t pressureValue = 0; //variable to store the value coming from the pressure transducer

int16_t pressureValue1 = 0; //variable to store the value coming from the pressure transducer

//============================ end pressure sensor variables ================================
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void setup_wifi() {

  delay(5000);

  //clean FS for testing 
//  SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(pressure_config1, json["pressure_config1"]);
          strcpy(pressure_config2, json["pressure_config2"]);
          //Serial.println(mqtt_server);
          //Serial.println(pressure_config1);
          //Serial.println(pressure_config2);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read
delay(1000);


  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_user("Pressureconfig1", "Pressure Config1", pressure_config1, 4);
  WiFiManagerParameter custom_mqtt_pass("Pressureconfig2", "Pressure Config2", pressure_config2, 4);
  
//                                                        this is the parameters in a dic
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;//---------------------------------------------------mains here
//                                                        the reference of wifimanager
// Reset Wifi settings for testing  1
 // wifiManager.resetSettings();

  //set config save notify callback
  // Reset Wifi settings for testing  2
 //wifiManager.setSaveConfigCallback(saveConfigCallback);                     //why is this used?
  // Reset Wifi settings for testing  3                                          i don't know
 
  //set static ip
//  wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  
  //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server); //------------------------------parameters here
  wifiManager.addParameter(&custom_mqtt_user); //------------------------------parameters here
  wifiManager.addParameter(&custom_mqtt_pass); //------------------------------parameters here
  //reset settings - for testing
  //wifiManager.resetSettings();                                              // resets the thing Tool --> erase flash --> all to fix exceptions
  saveConfigCallback();
  //set minimum quality of signal so it ignores AP's under that quality
  //defaults to 8%                                                             ?? what is this
  //wifiManager.setMinimumSignalQuality();
  
  //sets timeout until configuration portal gets turned off                    not used
  //useful to make it all retry or go to sleep                                   now
  //in seconds
  //wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    Serial.println("failed to connect and hit timeout");
    //delay(3000);
    //reset and try again, or maybe put it to deep sleep
    //ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...wifi connection successfull");
  digitalWrite(D5, HIGH);
  //============================== LED for successfull Wifi =============================
delay(1000);


  
  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(pressure_config1, custom_mqtt_user.getValue());
  strcpy(pressure_config2, custom_mqtt_pass.getValue());
  delay(1000);


  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server; 
    json["pressure_config1"] = pressure_config1; 
    json["pressure_config2"] = pressure_config2; 
    File configFile = SPIFFS.open("/config.json", "w");//reffer the config file
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);// print to serial bus
    json.printTo(configFile); //store file data to the config file
    configFile.close();//close file
    Serial.println("saved to json file successfully");
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());//print local ip
 // const uint16_t mqtt_port_x = 12025; 
  Serial.println("The values in the file are: ");
  Serial.println("\tmqtt_server : " + String(mqtt_server));
  Serial.println("\tmqtt_port : " + String(pressure_config1));
  Serial.println("\tapi_token : " + String(pressure_config2));
  
 
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    Serial.print(mqtt_server);
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(mqtt_server)) { //"raspberrypi.Dlink"
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world raspberrypi");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//===========================================================================================

void setup() //setup routine, runs once when system turned on or reset
{
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883); //"raspberrypi.Dlink"
  //client.setCallback(callback);

  //===============================================================================
  // initialize LCD
  //lcd.init();
  // turn on LCD backlight                      
  //lcd.backlight();
  
  //===============================================================================
  
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D5, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D6, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D7, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D8, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

 

 // Serial.begin(baudRate); //initializes serial communication at set baud rate bits per second
  //lcd.begin(); //initializes the LCD screen
  if (!ads.begin())
  {
    Serial.print("failed to initialise ADS 1115");
    //
  } else {
    Serial.print("initialise ADS 1115 successful");
    //LED ON code
    digitalWrite(D7, HIGH);
  }
  //ads.begin(); // begin ADS1115
  
  mcp.digitalWrite(MCP_LEDTOG1, HIGH); //adscheckup high
  //=================================================

 if (!mcp.begin())
  {
    Serial.print("failed to initialise MCP");
    //
  } else {
    Serial.print("initialise MCP successful");
    //LED ON code
    digitalWrite(D6, HIGH);
  }

  
  //mcp.begin();      // Default device address 0

  mcp.pinMode(MCP_LEDTOG1, OUTPUT);  // Toggle LED 3
  mcp.pinMode(MCP_LEDTOG2, OUTPUT);  // Toggle LED 2

  mcp.pinMode(MCP_LED1, OUTPUT);     // LED 1 output
  

  sensors.begin(); //temperature sensor startup
  
  mcp.digitalWrite(MCP_LED1, HIGH); //led1 toggle high

  delay(1000);

  mcp.digitalWrite(MCP_LED1, LOW); //led1 toggle low
  mcp.digitalWrite(MCP_LEDTOG2, LOW); //led2 toggle low
  mcp.digitalWrite(MCP_LEDTOG1, LOW); //led3 toggle low
  digitalWrite(D5, HIGH);
}

void loop() //loop routine runs over and over again forever
{
 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

 //=========================== read json values =======================
  Serial.println("The values in the file are: ");
  Serial.println("\tmqtt_server : " + String(mqtt_server));
  Serial.println("\tmqtt_port : " + String(pressure_config1));
  Serial.println("\tapi_token : " + String(pressure_config2));
 //===================================================================

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);

    //===========================convert sensor value to send MQTT=========================
    temp_str = String(Voltage); //converting ftemp (the float variable above) to a string 
    temp_str.toCharArray(temp, temp_str.length() + 1); //packaging up the data to publish to mqtt 
    //client.publish(topic, (char*) temp_str.c_str());

    
    Serial.print("Publish message: ");//all of these Serial prints are to help with debuging
    // ======================================================== publish TDS 2 =============================
    client.publish("Voltage_value", temp);
    //=================================end convert======================================================
  }
  //========================= begin pressure sensor on A2 =================================
  pressureValue = ads.readADC_SingleEnded(2); //reads value from input pin and assigns to variable
  Serial.print("A2 value"); //prints value from previous line to serial
  Serial.print(pressureValue); //prints value from previous line to serial
  Serial.print("     "); //prints value from previous line to serial
  pressureValue = ((pressureValue-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero); //conversion equation to convert analog reading to psi
  Serial.print(pressureValue, 1); //prints value from previous line to serial
  Serial.println("psi"); //prints label to serial

    //===========================convert sensor value to send MQTT=========================
    temp_str = String(pressureValue); //converting ftemp (the float variable above) to a string 
    temp_str.toCharArray(temp, temp_str.length() + 1); //packaging up the data to publish to mqtt 
    //client.publish(topic, (char*) temp_str.c_str());
   
    Serial.print("Publish message: ");//all of these Serial prints are to help with debuging
    // ======================================================== publish TDS 2 =============================
    client.publish("PressureA2_value", temp);
    //=================================end convert======================================================
  
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
