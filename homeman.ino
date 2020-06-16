// Plot DTH11 data on thingspeak.com using an ESP8266
// May 14 2020
// Author: Thuy Nguyen, based on an example from Jeroen Beemster reading DTH11 sensor

// Ref.:
// Website: www.arduinesp.com
// https://learn.adafruit.com/dht/using-a-dhtxx-sensor


#include <DHT.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "wifi_pw.h"
//#include <NTPClient.h>
#include <WiFiUdp.h>

int globalError = 0;
int debugCounter = 0;

// replace with your channel's thingspeak API key, 
const char* ssid = "VNNO"; //"Thuy's iPhone";//"matsuya"; //  // 
//const char* ssid = "DNVGuest";
const char* password = WIFI_PW;

unsigned long THING_SPEAK_CHANNEL_NO = 447257;
String myWriteAPIKey = "59Y4RMCCJVKVWBOQ";
// to send data, using this get req: https://api.thingspeak.com/update?api_key=QFS00KA70KNC5NX6&field8=6


WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "time.nist.gov");

#define DELAY_LONG 5000    // 5,0 seconds
#define DELAY_SHORT 2500   // 2,5 seconds

String myReadAPIKey = "9L9ZWCW1QLN39E09";
const char* server = "api.thingspeak.com";

// inputs
#define PIN_SS_ANALOG               A0

#define PIN_SS_DHT                  D5 // DHT sensor pin
#define PIN_SS_DOOR_MAIN            D6 // main door sensor

#define PIN_SS_DOOR_BASEMENT        D3 // No. 1, door to go down to basement and door to renting area
#define PIN_SS_ENTRANCE_MOTION      D2 // No. 2, motion sensor for entrance

#define PIN_SS_WATER_SMOKE_BASEMENT D7 // smoke, water leak


// outputs
#define PIN_LED                     D4 // D4: same as built in LED GPIO2

#define PIN_AC_POWER_LED_ENTRANCE   D0 // No. 1, power for entrance led
#define PIN_AC_POWER_CAMERA         D1 // No. 2, camera power

#define PIN_AC_BUZZER       D10
#define FIELD_ID_POWER_CAM  8
#define FIELD_ID_POWER_LOAD 9

DHT dht(PIN_SS_DHT, DHT11,15);
WiFiClient client;



void setup() {
  pinMode(PIN_SS_DOOR_MAIN, INPUT);
  pinMode(PIN_SS_DOOR_BASEMENT, INPUT);
  pinMode(PIN_SS_WATER_SMOKE_BASEMENT, INPUT);
  pinMode(PIN_SS_ENTRANCE_MOTION, INPUT);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_AC_BUZZER, OUTPUT);
  pinMode(PIN_AC_POWER_LED_ENTRANCE, OUTPUT);
  pinMode(PIN_AC_POWER_CAMERA, OUTPUT);

  Serial.begin(19200);
  delay(1000);
  dht.begin();

  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  bool ledStatus = false;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    ledStatus = !ledStatus;
    digitalWrite(PIN_LED, !ledStatus);
    if(debugCounter++ > 80)
    {
      debugCounter = 0;
      Serial.println("!");
    }
  }

  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());


// Initialize a NTPClient to get time
  timeClient.begin();
// Set offset time in seconds to adjust for your timezone, for example:
// GMT +1 = 3600, GMT +8 = 28800, GMT -1 = -3600, GMT 0 = 0
  timeClient.setTimeOffset(3600); // Norway GMT + 1


  ThingSpeak.begin(client);
}

long delayMs = DELAY_LONG;
// sensors
float humidity = 0.0;
float temp = 0.0;

bool ssDoorMain = 0;
bool ssDoorBasement = 0;
bool ssDoorBack = 0;
bool ssEntranceMotion = 0;

int ssSmokeDetectors = 0;
int ssDoorDetectors = 0;
int ssWaterLeak = 0;

// actuators
bool acCamPower = 0;
bool acBuzzer = 0;

bool forceCamPower = 0;
float camPower = 0;

void loop() {
  if(!updateHumidTempe()){
    humidity = -100.0;
    temp = -100.0;
  }

  getTime();

  blinkLed();
  updateSensors();
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
     Serial.println("% sent to Thingspeak");    
  }
  client.stop();
   
  Serial.println("Waiting...");
  // thingspeak needs minimum 15 sec delay between updates

  delayWithErrorCheck();
}

void blinkLed()
{
    Serial.print("-");
    if(debugCounter++ > 80)
    {
      debugCounter = 0;
      Serial.println("!");
    }

  digitalWrite(PIN_LED, false);
  delay(1000);
  digitalWrite(PIN_LED, true);
  delay(1000);
}


void getTime(){
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);
}

bool updateHumidTempe(){
  humidity = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(humidity) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");

    delayWithErrorCheck();
    return false;
  }

  return true;
}

void delayWithErrorCheck(){
    if(globalError > 0)
    blinkLed();
  else
    delay(delayMs);
}

void updateSensors(){
  ssDoorMain = digitalRead(PIN_SS_DOOR_MAIN);
  ssDoorBasement = digitalRead(PIN_SS_DOOR_BASEMENT);
  ssEntranceMotion = digitalRead(PIN_SS_ENTRANCE_MOTION);

  bool ssWaterLeak = digitalRead(PIN_SS_WATER_SMOKE_BASEMENT);

  ssDoorDetectors = (ssDoorBasement << 2) | (ssDoorBack << 1) | (ssDoorMain << 0);

  globalError = (ssDoorDetectors << 8) | ssDoorDetectors;
  
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
  digitalWrite(PIN_AC_POWER_LED_ENTRANCE, ssEntranceMotion);

  camPower = ThingSpeak.readFloatField(THING_SPEAK_CHANNEL_NO, FIELD_ID_POWER_CAM);
    if(camPower < 1.0)
      digitalWrite(PIN_AC_POWER_CAMERA, 0 || forceCamPower);
    else
      digitalWrite(PIN_AC_POWER_CAMERA, 1);

  Serial.print("Cam power: = ");
  Serial.println(camPower);
}
