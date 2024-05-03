/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <PubSubClient.h>  
#include <ArduinoJson.h>

#include <Wire.h>                            //extender
#include <Adafruit_ADS1015.h>                //Adafruit works the best with documentation

// ========================================== begin tds code ==========================================

   
#define TdsSensorPin 0 
#define VREF 5.0 // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point
int analogBuffer[SCOUNT]; // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;
String temp_str;

//see last code block below use these to convert the float that you get back from DHT to a string =str
String hum_str;
char temp[50];
char hum[50];

// ========================================== end tds code ==========================================

//#include <ESP8266WiFi.h>
//#include <PubSubClient.h>

// Update these with values suitable for your network.
//flag for saving data
bool shouldSaveConfig = false;
const char* ssid = "aditya";
const char* password = "55962001";
#define mqtt_server   "raspberrypi1"
//"raspberrypi"
//"192.168.0.116"
//test.mosquitto.org
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
//long lastMsg = 0;
//char msg[50];
//int value = 0;
Adafruit_ADS1115 ads(0x48);                 //hex code for the extender 
/*
ADDR pin value	 Connection with Pin
0x48	           Connect address pin to GND
0x49	           Connect address pin to VDD
0x4A	           Connect address pin to SDA
0x4B	           Connect address pin to SCL
*/
float Voltage = 0.0;                         //store volts

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void setup_Extender(){ 
  //delay(5000);
  //Serial.begin(9600);                        //setral output setup
  ads.begin();                               //bigning of the ads code
  }
void extend_process(){
  int16_t adc0;
  int16_t adc1;
  
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  Voltage = (adc0 * 0.1875)/1000;
  
  Serial.print("AIN0: "); 
  Serial.print(adc0);
  Serial.print("\tVoltage: ");
  Serial.println(Voltage, 7); 
  Serial.println();
  
  //static unsigned long analogSampleTimepoint = millis();
  
  }
void setup_wifi() {

 

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
         // strcpy(mqtt_port, json["mqtt_port"]);
         // strcpy(mqtt_user, json["mqtt_user"]);
         // strcpy(mqtt_pass, json["mqtt_pass"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read



  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  //WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
 // WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 20);
 // WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", mqtt_pass, 20);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

// Reset Wifi settings for testing  1
  //wifiManager.resetSettings();
  
  //set config save notify callback
  // Reset Wifi settings for testing  2
  //wifiManager.setSaveConfigCallback(saveConfigCallback);
  // Reset Wifi settings for testing  3
  //saveConfigCallback();
  //set static ip
//  wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  
  //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  //wifiManager.addParameter(&custom_mqtt_port);
  //wifiManager.addParameter(&custom_mqtt_user);
  //wifiManager.addParameter(&custom_mqtt_pass);

  //reset settings - for testing
  //wifiManager.resetSettings();

  //set minimum quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    //ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
 // strcpy(mqtt_port, custom_mqtt_port.getValue());
//  strcpy(mqtt_user, custom_mqtt_user.getValue());
 // strcpy(mqtt_pass, custom_mqtt_pass.getValue());
 // strcpy(blynk_token, custom_blynk_token.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
   // json["mqtt_port"] = mqtt_port;
   // json["mqtt_user"] = mqtt_user;
    //json["mqtt_pass"] = mqtt_pass;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    Serial.println("saved to json file successfully");
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
//  client.setServer(mqtt_server, 12025);
  const uint16_t mqtt_port_x = 12025; 
 // client.setServer(mqtt_server, mqtt_port_x);
 Serial.println("local ip");
 Serial.println(mqtt_server);

 
 
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
    Serial.print("Attempting MQTT connection..raspberrypi.");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(mqtt_server)) {
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
void setup_tds()               // Setup the tds sensor
  {
    pinMode(TdsSensorPin,INPUT);
  }

float loop_tds()
  {
    static unsigned long analogSampleTimepoint = millis();
    if(millis()-analogSampleTimepoint > 40U) //every 40 milliseconds,read the analog value from the ADC
      {
        analogSampleTimepoint = millis();
        analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin); //read the analog value and store into the buffer
        analogBufferIndex++;
        if(analogBufferIndex == SCOUNT)
        analogBufferIndex = 0;
      }
    static unsigned long printTimepoint = millis();
    if(millis()-printTimepoint > 800U)
      {
        printTimepoint = millis();
        for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
        averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF/ 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
        float compensationCoefficient=1.0+0.02*(temperature-25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
        float compensationVolatge=averageVoltage/compensationCoefficient; //temperature compensation
        tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
        //Serial.print("voltage:");
        //Serial.print(averageVoltage,2);
        //Serial.print("V ");
        Serial.print("TDS Value:");
        Serial.print(tdsValue,0);
        Serial.println("ppm");
        return tdsValue;
      }
    }
int getMedianNum(int bArray[], int iFilterLen)
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

}
// ======================================================= Start And loop here ===================================
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  setup_tds();// Setup the tds sensor
}

void loop() {
  extend_process()
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //====================================================== Read TDS Values ====================================

  static unsigned long analogSampleTimepoint = millis();
    if(millis()-analogSampleTimepoint > 40U) //every 40 milliseconds,read the analog value from the ADC
      {
        analogSampleTimepoint = millis();
        analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin); //read the analog value and store into the buffer
        analogBufferIndex++;
        if(analogBufferIndex == SCOUNT)
        analogBufferIndex = 0;
      }
    static unsigned long printTimepoint = millis();
    if(millis()-printTimepoint > 800U)
      {
        printTimepoint = millis();
        for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
        averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF/ 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
        float compensationCoefficient=1.0+0.02*(temperature-25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
        float compensationVolatge=averageVoltage/compensationCoefficient; //temperature compensation
        tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
        //Serial.print("voltage:");
        //Serial.print(averageVoltage,2);
        //Serial.print("V ");
        Serial.print("TDS Value:");
        Serial.print(tdsValue,0);
        Serial.println("ppm");
       
      }
      // ================================================= end read tds values =========================================================================
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    //snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    //snprintf (msg, MSG_BUFFER_SIZE,"outTopic publish %d",tdsValue);
    //dtostrf(msg,1,2,tdsValue);
    //temp_str=String(tdsValue);
    //temp_str.toCharArray(
    Serial.print("Publish message: ");
    //Serial.println(msg);
    // Serial.print("ppm ");
    
    temp_str = String(tdsValue); //converting ftemp (the float variable above) to a string 
    temp_str.toCharArray(temp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa...
    //client.publish(topic, (char*) temp_str.c_str());
    Serial.print("Publish message: ");//all of these Serial prints are to help with debuging
    client.publish("outTopic", temp);
    
  }
}
