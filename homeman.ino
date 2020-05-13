// www.arduinesp.com 
//
// Plot DTH11 data on thingspeak.com using an ESP8266 
// April 11 2015
// Author: Jeroen Beemster
// Website: www.arduinesp.com
 
// https://learn.adafruit.com/dht/using-a-dhtxx-sensor
 
#include <DHT.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"

// replace with your channel's thingspeak API key, 
const char* ssid = "VNNO";
const char* password = "Pw";
//const char* ssid = "DNVGuest";
//const char* password = "";

unsigned long THING_SPEAK_CHANNEL_NO = 447257;
String myWriteAPIKey = "QFS00KA70KNC5NX6";
// to send data, using this get req: https://api.thingspeak.com/update?api_key=QFS00KA70KNC5NX6&field8=6

#define DELAY_LONG 30000    // 30 seconds
#define DELAY_SHORT 3000    // 3 seconds

int pinLed = D6;
String myReadAPIKey = "SBXS87RSOO71AE3A";

const char* server = "api.thingspeak.com";
#define DHTPIN D2 // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11

int pinTempSensor = A0;

#define PIN_WORKING_MODE    D0
#define PIN_SS_DOOR_MAIN    D3
#define PIN_SS_DOOR_BACK    D4
#define PIN_SS_DOOR_DOWN_BASEMENT   D5
#define PIN_SS_WATER_LEAK_BASEMENT  D6

#define PIN_AC_POWER_LOAD   D8
#define PIN_AC_POWER_CAM    D9
#define PIN_AC_BUZZER       D10
#define FIELD_ID_POWER_CAM  8
#define FIELD_ID_POWER_LOAD  9

DHT dht(DHTPIN, DHT11,15);
WiFiClient client;
   
 
void setup() {                
  Serial.begin(115200);
  delay(10);
  dht.begin();
  
  WiFi.begin(ssid, password);
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  pinMode(PIN_WORKING_MODE, INPUT);

  pinMode(PIN_SS_DOOR_MAIN, INPUT);
  pinMode(PIN_SS_DOOR_BACK, INPUT);
  pinMode(PIN_SS_DOOR_DOWN_BASEMENT, INPUT);
  pinMode(PIN_SS_WATER_LEAK_BASEMENT, INPUT);

  pinMode(PIN_AC_POWER_LOAD, OUTPUT);
  pinMode(PIN_AC_POWER_CAM, OUTPUT);


  ThingSpeak.begin(client);
}

long delayMs = DELAY_LONG;
// sensors
float humidity = 0;
float temp = 0;

int ssSmokeDetectors = 0;
int ssDoorDetectors = 0;
int ssWaterLeak = 0;

// actuators
bool acCamPower = 0;
bool acLoadPower = 0;
bool acBuzzer = 0;

bool forceCamPower = 0;
float camPower = 0;
float loadPower = 0;

void loop() {
   
  updateSensors();
  updateWorkingMode();
  updateActuator();
 
  if (client.connect(server,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = myWriteAPIKey;
           postStr +="&field1=";
           postStr += String(temp);
           postStr +="&field2=";
           postStr += String(humidity);
           postStr +="&field3=";
           postStr += String(ssSmokeDetectors);
           postStr +="&field4=";
           postStr += String(ssDoorDetectors);
           postStr +="&field5=";
           postStr += String(ssWaterLeak);
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n"); 
     client.print("Host: api.thingspeak.com\n"); 
     client.print("Connection: close\n"); 
     client.print("X-THINGSPEAKAPIKEY: "+myWriteAPIKey+"\n"); 
     client.print("Content-Type: application/x-www-form-urlencoded\n"); 
     client.print("Content-Length: "); 
     client.print(postStr.length()); 
     client.print("\n\n"); 
     client.print(postStr);
           
 
     Serial.print("Temperature: ");
     Serial.print(temp);
     Serial.print(" degrees Celcius Humidity: "); 
     Serial.print(humidity);
     Serial.println("% send to Thingspeak");    
  }
  client.stop();
   
  Serial.println("Waiting...");    
  // thingspeak needs minimum 15 sec delay between updates
//  updateTemperature();

  delay(delayMs);  
}

void updateTemperature(){
  humidity = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(humidity) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(delayMs);
    return;
  }
}

void updateWorkingMode(){
  bool workingMode = digitalRead(PIN_WORKING_MODE);
  if(workingMode)
    delayMs = DELAY_LONG;
  else
    delayMs = DELAY_SHORT;
}

void updateSensors(){
//  bool ssSmokeKichen = digitalRead(PIN_SS_SMOKE_KITCHEN);
//  bool ssSmokeLivingRoom = digitalRead(PIN_SS_SMOKE_LIVING);
//  bool ssSmokeFirstFloor = digitalRead(PIN_SS_SMOKE_1_FLOOR);

  bool ssDoorMain = digitalRead(PIN_SS_DOOR_MAIN);
  bool ssDoorBack = digitalRead(PIN_SS_DOOR_BACK);
  bool ssDoorDownBasement = digitalRead(PIN_SS_DOOR_DOWN_BASEMENT);

  bool ssWaterLeak = digitalRead(PIN_SS_WATER_LEAK_BASEMENT);

//  ssSmokeDetectors = (ssSmokeKichen << 3) | (ssSmokeLivingRoom << 2) | (ssSmokeFirstFloor << 1);
  ssDoorDetectors = (ssDoorDownBasement << 2) | (ssDoorBack << 1) | (ssDoorMain << 0);

  Serial.print("Door sensors: = ");
  Serial.println(ssDoorDetectors);
  Serial.print("Smoke sensors: = ");
  Serial.println(ssSmokeDetectors);
  Serial.print("Water leak. sensors: = ");
  Serial.println(ssWaterLeak);

  if((ssSmokeDetectors > 0) || (ssDoorDetectors > 0))
    forceCamPower = 1;
  else
    forceCamPower = 0;
}

void updateActuator()
{
  camPower = ThingSpeak.readFloatField(THING_SPEAK_CHANNEL_NO, FIELD_ID_POWER_CAM);
    if(camPower < 1.0)
      digitalWrite(PIN_AC_POWER_CAM, 0 || forceCamPower);
    else
      digitalWrite(PIN_AC_POWER_CAM, 1);

  Serial.print("Cam power: = ");
  Serial.println(camPower);

    loadPower = ThingSpeak.readFloatField(THING_SPEAK_CHANNEL_NO, FIELD_ID_POWER_LOAD);
    if(loadPower < 1.0)
      digitalWrite(PIN_AC_POWER_LOAD, 0 || forceCamPower);
    else
      digitalWrite(PIN_AC_POWER_LOAD, 1);
}
