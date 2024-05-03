#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "aditya";//put your wifi network name here
const char* password = "55962001";//put your wifi password here

IPAddress ip();
IPAddress gateway();
IPAddress subnet();

WiFiServer server(80);
WiFiClient client;

int SensorState;

#define echoPin D5 //echo Pin connected to sensor

void setup() 
{
  Serial.begin(9600);

  //Set pin
  pinMode(echoPin, INPUT);

  WiFi.begin(ssid, password);
  //WiFi.config(ip, gateway, subnet);
  Serial.println("Connecting");

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }

  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  //Start the TCP server
  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
}

void loop() 
{
  //Liten to connecting clients
  client = server.available();

  if(client)
  {
    Serial.println("Client Connected");
    while(client.connected())
    {
      SensorState = digitalRead(echoPin);

      if(SensorState == 0)
      {
        Serial.println(SensorState);
        //Send the sensor state to the client
        client.print(SensorState);
        client.print('\r');
      }
      else
      {
        Serial.println(SensorState);
        //Send the sensor state to the client
        client.print(SensorState);
        client.print('\r');
      }

      //Delay before the next reading
      delay(500);
    }
  }
}
