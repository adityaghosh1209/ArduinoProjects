#include <DNSServer.h>            //for mqtt
#include <ESP8266WebServer.h>     //for mqtt
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <PubSubClient.h>         //mqtt
#include <ArduinoJson.h>          //file access and storage

#ifdef ESP32
  #include <SPIFFS.h>
#endif
int tempreset= 0; // store reset pin info
bool shouldSaveConfig = false;
const char* ssid = "abcd";
const char* password = "abcd";
//#define mqtt_server   "abcd"
//"raspberrypi"
//"192.168.0.116"
//test.mosquitto.org
WiFiClient espClient;
//PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

char mqtt_server[40];
char tank_low[6] = "8080";
char tank_half[6] = "8080";
char tank_full[6] = "8080";

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void setupwifi(){
//read configuration from FS json


  //clean FS, for testing
  //SPIFFS.format();

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

//#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get());
        serializeJson(json, Serial);
        if ( ! deserializeError ) {
//#else
       // DynamicJsonBuffer jsonBuffer;
       // JsonObject& json = jsonBuffer.parseObject(buf.get());
       // json.printTo(Serial);
       // if (json.success()) {

          Serial.println("\nparsed json");
          const char* error = json["mqtt_server"]; 
          if (!error) {
              Serial.println("mqtt_server doesnot exist in json");
              Serial.println(error);
            } else {
                    strcpy(mqtt_server, json["mqtt_server"]);
                   }        
          const char* error1 = json["tank_low"]; 
          if (!error1) {
              Serial.println("tank_low doesnot exist in json");
              Serial.println(error1);
            } else {
                    strcpy(tank_low, json["tank_low"]);
                   }    
          const char* error2 = json["tank_half"]; 
          if (!error2) {
              Serial.println("tank_half doesnot exist in json");
              Serial.println(error2);
            } else {
                    strcpy(tank_half, json["tank_half"]);
                   }  
          const char* error3 = json["tank_full"]; 
          if (!error3) {
              Serial.println("tank_full doesnot exist in json");
              Serial.println(error3);
            } else {
                    strcpy(tank_full, json["tank_full"]);
                   } 
          
         
          
       // } else {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
//#endif   
  } else {
    Serial.println("failed to mount FS");
  }
  //end read
  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_tank_low("tank_low", "tank_low", tank_low, 6);
  WiFiManagerParameter custom_tank_half("tank_half", "tank_half", tank_half, 6);
  WiFiManagerParameter custom_tank_full("tank_full", "tank_full", tank_full, 6);

 //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //set static ip
  //wifiManager.setSTAStaticIPConfig(IPAddress(10, 0, 1, 99), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));

  
   //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_tank_low);
  wifiManager.addParameter(&custom_tank_half);
  wifiManager.addParameter(&custom_tank_full);
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
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
    ESP.restart();
    delay(5000);
  }
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(tank_low, custom_tank_low.getValue());
  strcpy(tank_half, custom_tank_half.getValue());
  strcpy(tank_full, custom_tank_full.getValue());
  Serial.println("The values in the file are: ");
  Serial.println("\tmqtt_server : " + String(mqtt_server));
  Serial.println("\tank_low: " + String(tank_low));
  Serial.println("\tank_half : " + String(tank_half));
  Serial.println("\tank_full : " + String(tank_full));
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
//#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
    DynamicJsonDocument json(1024);
//#else
  //  DynamicJsonBuffer jsonBuffer;
   // JsonObject& json = jsonBuffer.createObject();
//#endif
    json["mqtt_server"] = mqtt_server;
    json["tank_low"] = tank_low;
    json["tank_half"] = tank_half;
    json["tank_full"] = tank_full;
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

//#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
    serializeJson(json, Serial);
    serializeJson(json, configFile);
//#else
  ////  json.printTo(Serial);
  //  json.printTo(configFile);
//#endif
    configFile.close();
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP()); 

}

void resetwifi(){
//read configuration from FS json


  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
 
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

//#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get());
        serializeJson(json, Serial);
        if ( ! deserializeError ) {
//#else
       // DynamicJsonBuffer jsonBuffer;
       // JsonObject& json = jsonBuffer.parseObject(buf.get());
       // json.printTo(Serial);
       // if (json.success()) {

          Serial.println("\nparsed json");
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(tank_low, json["tank_low"]);
          strcpy(tank_half, json["tank_half"]);
          strcpy(tank_full, json["tank_full"]);
          
       // } else {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
//#endif   
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_tank_low("tank_low", "tank_low", tank_low, 6);
  WiFiManagerParameter custom_tank_half("tank_half", "tank_half", tank_half, 6);
  WiFiManagerParameter custom_tank_full("tank_full", "tank_full", tank_full, 6);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //set static ip
  //wifiManager.setSTAStaticIPConfig(IPAddress(10, 0, 1, 99), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));

  //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_tank_low);
  wifiManager.addParameter(&custom_tank_half);
  wifiManager.addParameter(&custom_tank_full);
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
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
    ESP.restart();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(tank_low, custom_tank_low.getValue());
  strcpy(tank_half, custom_tank_half.getValue());
  strcpy(tank_full, custom_tank_full.getValue());
  Serial.println("The values in the file are: ");
  Serial.println("\tmqtt_server : " + String(mqtt_server));
  Serial.println("\ttank_low: " + String(tank_low));
  Serial.println("\ttank_half : " + String(tank_half));
  Serial.println("\ttank_full : " + String(tank_full));
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
//#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
    DynamicJsonDocument json(1024);
//#else
  //  DynamicJsonBuffer jsonBuffer;
   // JsonObject& json = jsonBuffer.createObject();
//#endif
    json["mqtt_server"] = mqtt_server;
    json["tank_low"] = tank_low;
    json["tank_half"] = tank_half;
    json["tank_full"] = tank_full;
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

//#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
    serializeJson(json, Serial);
    serializeJson(json, configFile);
//#else
  ////  json.printTo(Serial);
  //  json.printTo(configFile);
//#endif
    configFile.close();
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP()); 

}


void setup() //setup routine, runs once when system turned on or reset
{
  ESP.wdtFeed(); // feeds the dog
  delay(0);
  pinMode(D8, INPUT);
  setupwifi();
  //ESP.wdtDisable();
  Serial.begin(115200);
   yield();//
 
}
void loop()
{

//================================= check for reset pin ===========================================
 tempreset = digitalRead(D8);
     
     if (tempreset == HIGH) {
        resetwifi();
        Serial.println("LED Turned ON");
        delay(1000);
       }
     else {
        
        Serial.println("LED Turned OFF");
        delay(1000);
       }

//================================== end check for reset pin ===================================
 //=========================== read json values =======================
  Serial.println("The values in the file are: ");
  Serial.println("\tmqtt_server : " + String(mqtt_server));
  Serial.println("\tank_low: " + String(tank_low).toInt());
  Serial.println("\tank_half : " + String(tank_half));
  Serial.println("\tank_full: " + String(tank_full));
  delay(2000);
 //===================================================================

}
