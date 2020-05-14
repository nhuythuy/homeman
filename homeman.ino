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
#include "wifi_pw.h"

// replace with your channel's thingspeak API key, 
//const char* ssid = "matsuya"; //"VNNO";
//const char* password = "kamikaze123"; // "Planetvegen18A"; // WIFI_PW;
const char* ssid = "DNVGuest";
const char* password = WIFI_PW;

unsigned long THING_SPEAK_CHANNEL_NO = 447257;
String myWriteAPIKey = "59Y4RMCCJVKVWBOQ";
// to send data, using this get req: https://api.thingspeak.com/update?api_key=QFS00KA70KNC5NX6&field8=6

#define DELAY_LONG 30000    // 30 seconds
#define DELAY_SHORT 3000    // 3 seconds

String myReadAPIKey = "9L9ZWCW1QLN39E09";
const char* server = "api.thingspeak.com";

#define PIN_SS_ANALOG       A0

#define PIN_LED             D4 // same as built in LED GPIO2
#define PIN_WORKING_MODE    D0
#define PIN_SS_DHT          D1 // DHT sensor pin
#define PIN_SS_DOOR_MAIN    D2
#define PIN_SS_DOOR_BACK    D3

#define PIN_SS_DOOR_DOWN_BASEMENT   D5
#define PIN_SS_WATER_SMOKE_BASEMENT D6 // smoke, water leak

#define PIN_AC_POWER_LOAD   D8
#define PIN_AC_POWER_CAM    D9
#define PIN_AC_BUZZER       D10
#define FIELD_ID_POWER_CAM  8
#define FIELD_ID_POWER_LOAD 9

DHT dht(PIN_SS_DHT, DHT11,15);
WiFiClient client;
   
 
void setup() {                
  Serial.begin(115200);
  delay(10);
  dht.begin();

  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  int dotCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");

    if(dotCounter++ > 80)
    {
      dotCounter = 0;
      Serial.println("!");
    }
  }

  Serial.println(WiFi.localIP());
  Serial.println("WiFi connected");

  pinMode(PIN_WORKING_MODE, INPUT);

  pinMode(PIN_SS_DOOR_MAIN, INPUT);
//  pinMode(PIN_SS_DOOR_BACK, INPUT);
  pinMode(PIN_SS_DOOR_DOWN_BASEMENT, INPUT);
  pinMode(PIN_SS_WATER_SMOKE_BASEMENT, INPUT);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_AC_POWER_LOAD, OUTPUT);
  pinMode(PIN_AC_POWER_CAM, OUTPUT);


  ThingSpeak.begin(client);
}

long delayMs = DELAY_LONG;
// sensors
float humidity = 0.0;
float temp = 0.0;

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
  bool ret = updateHumidTempe();
  if(!ret){
    humidity = -100.0;
    temp = -100.0;
  }

  blinkLed();

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

void blinkLed()
{
  digitalWrite(PIN_LED, true);
  delay(200);
  digitalWrite(PIN_LED, false);
}
bool updateHumidTempe(){
  humidity = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(humidity) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(delayMs);
    return false;
  }

  return true;
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
  bool ssDoorBack = 0;//digitalRead(PIN_SS_DOOR_BACK);
  bool ssDoorDownBasement = digitalRead(PIN_SS_DOOR_DOWN_BASEMENT);

  bool ssWaterLeak = digitalRead(PIN_SS_WATER_SMOKE_BASEMENT);

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