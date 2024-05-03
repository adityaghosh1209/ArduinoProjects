#define fullLevelLEDPin D6
#define halfLevelLEDPin D5
#define lowLevelLEDPin D4

#define pingPin D7
#define echoPin D8

#define motor D3
float fullLevelOfWater = 100;
float halflLevelofWater = 50;
float lowLevelOfWater = 5;
bool motorstatus = false;
float distance;
float duration;
void setup(){
    tankequipmentmanager()
}

void loop(){
    analiser();
    motorcontrol()
}

int checklevel(float levelofwater){
    if(levelofwater<=lowLevelOfWater){
        return 3;//red 
    }
    else if(levelofwater<=halflLevelofWater){
        return 2;//yellow
    }
    else if (levelofwater<=fullLevelOfWater)
    {
        return 1;//green
    }
    else if (levelofwater>=fullLevelOfWater)
    {
        return 1;//green
    }
    else{
        return 1;//red
    }
}

void analiser(){
    switch (checklevel(readsensordata()))
    {
    case 1:
        Serial.println("low water Alert/_!_ ");
        leddown();

        if (motorstatus == false)
        {
          Serial.println(motorstatus);
            motorstatus=true;
            digitalWrite(lowLevelLEDPin,HIGH);            
        }
        else if (motorstatus == true)
        {
          Serial.println(motorstatus);
            blinkled(lowLevelLEDPin,2);
        }
        break;
    case 2:
    Serial.println("medium water Alert/! ");
        leddown();
            digitalWrite(halfLevelLEDPin,HIGH);
            Serial.println(motorstatus);
        
        break;
    case 3:
    Serial.println("tank full ");
        leddown()

        if (motorstatus == true)
        {
            motorstatus=false;
            blinkled(lowLevelLEDPin,1);
            Serial.println(motorstatus);
        }
        else
        {
            digitalWrite(lowLevelLEDPin,HIGH);
            Serial.println(motorstatus);
        }
        break;
    default:

        blinkled(lowLevelLEDPin,1);

        leddown();
        blinkled(lowLevelLEDPin,1);
        break;
    }

   
}

float readsensordata(){
  
pinMode(pingPin, OUTPUT);
digitalWrite(pingPin, LOW);
delayMicroseconds(2);
digitalWrite(pingPin, HIGH);
delayMicroseconds(10);
digitalWrite(pingPin, LOW);
pinMode(echoPin, INPUT);
duration = pulseIn(echoPin, HIGH);
distance = duration / 29 / 2;
Serial.println(distance);
  return distance;
  
}

void blinkled(int ledtoblink, int delaytiming){
    digitalWrite(ledtoblink, LOW);
    delay(delaytiming);
    digitalWrite(ledtoblink, HIGH);
    delay(delaytiming);
}

void leddown(){
    digitalWrite(lowLevelLEDPin , LOW);
    digitalWrite(lowLevelLEDPin , LOW);
    digitalWrite(lowLevelLEDPin , LOW);
}

void motorcontrol(){
	 if (motorstatus == true){
       digitalWrite(motor, HIGH);
    }
    else{
      digitalWrite(motor, LOW);
    }
}

void tankequipmentmanager(){
	pinMode(pingPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    pinMode(fullLevelLEDPin, OUTPUT);
    pinMode(halfLevelLEDPin, OUTPUT);
    pinMode(lowLevelLEDPin, OUTPUT);
    pinMode(motor, OUTPUT);
    Serial.begin(9600);
}