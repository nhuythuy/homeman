
// Plot DTH11 data on thingspeak.com using an ESP8266
// May 14 2020
// Author: Thuy Nguyen, based on an examples from Jeroen Beemster reading DTH11 sensor

// Ref.:
// Website: www.arduinesp.com
// https://learn.adafruit.com/dht/using-a-dhtxx-sensor


#include <DHT.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "wifi_pw.h"
#include "pin_define.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "melody.h"


int globalError = 0;
int debugCounter = 0;

// replace with your channel's thingspeak API key, 
const char* ssid = "VNNO"; // "DNVGuest" "Thuy's iPhone"; "matsuya";
const char* password = WIFI_PW;

//unsigned long THING_SPEAK_CHANNEL_NO = 447257;
String myWriteAPIKey = "59Y4RMCCJVKVWBOQ";
// to send data, using this get req: https://api.thingspeak.com/update?api_key=QFS00KA70KNC5NX6&field8=6

#define MAX_SUPPLY_VOLT   16.157    // volt: 10K(9910)+39K(38610) --> 3.3*(9910+38610)/9910 = 16.1570131181 V 
#define DELAY_LONG        5000      // 5,0 seconds
#define DELAY_SHORT       2500      // 2,5 seconds
#define MOTION_DELAY      3*60*1000  // 5 mins delay

String myReadAPIKey = "9L9ZWCW1QLN39E09";
const char* server = "api.thingspeak.com";

DHT dht(PIN_SS_DHT, DHT11,15);
WiFiClient client;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.nist.gov");

int minutes = 0; // use this for sending to update to send to thingspeak
bool needUploadCloud = false;
bool cloudUploaded = false;

long delayMs = DELAY_LONG;
// sensors
float humidity = 0.0;
float temp = 0.0;

int ssSupplyVoltRaw = 0;
float ssSupplyVolt = 0;
bool ssDoorMain = 0;
bool ssDoorBasement = 0;
bool ssDoorBack = 0;
bool ssEntranceMotion = 0;
bool ssLightBasementOn = 0;

int ssWaterLeak = 0;

int ssDoorDetectors = 0;
int ssOtherSensors = 0;

// actuators
bool acEntranceLed = 0;
bool acBuzzer = 0;

bool forceCamPower = 0;
float camPower = 0;

unsigned long now = millis();
unsigned long lastTrigger = millis();
boolean startMotionTimer = false;


void setup() {
  pinMode(PIN_SS_DOOR_MAIN, INPUT);
  pinMode(PIN_SS_DOOR_BASEMENT, INPUT);
  pinMode(PIN_SS_WATER_SMOKE_BASEMENT, INPUT);
  pinMode(PIN_SS_ENTRANCE_MOTION, INPUT);
  pinMode(PIN_LIGHT_BASEMENT, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_SS_ENTRANCE_MOTION), detectsMovement, RISING);


  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TONE_MELODY, OUTPUT);
  pinMode(PIN_AC_POWER_LED_ENTRANCE, OUTPUT);
//  pinMode(PIN_AC_POWER_CAMERA, OUTPUT);

  //Serial.begin(19200);
  Serial.begin(19200, SERIAL_8N1, SERIAL_TX_ONLY);
  delay(1000);
  dht.begin();

  Serial.println();
  Serial.println("MAC: " + WiFi.macAddress());
  Serial.println("Connecting to " + String(ssid));
  
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

  Serial.println("WiFi connected, IP: " + WiFi.localIP());

  playMelody();

  timeClient.begin(); // Initialize a NTPClient to get time
// Set offset time in seconds to adjust for your timezone, ex.: GMT +1 = 3600, GMT +8 = 28800, GMT -1 = -3600, GMT 0 = 0
  timeClient.setTimeOffset(3600); // Norway GMT + 1

  ThingSpeak.begin(client);
}

void loop() {
  updateHumidTempe();

  getServerTime();
  blinkLed();
  updateSensors();
  updateActuator();

  if(!cloudUploaded && needUploadCloud == true)
  {
    updateCloud();
    cloudUploaded = true;
  }

  delayWithErrorCheck();
}

void updateCloud(){
  if (client.connect(server,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = myWriteAPIKey;
           postStr +="&field1=" + String(temp);
           postStr +="&field2=" + String(humidity);
           postStr +="&field3=" + String(ssDoorDetectors);
           postStr +="&field4=" + String(ssOtherSensors);
           postStr +="&field5=" + String(ssSupplyVolt);
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

     Serial.print("Temperature: " + String(temp) + " degrees Celcius, Humidity: " + String(humidity) + "% sent to Thingspeak");
  }
  client.stop();
   
  Serial.println("Waiting...");
  // thingspeak needs minimum 15 sec delay between updates
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


void getServerTime(){
  Serial.println();
  timeClient.update();
  minutes = timeClient.getMinutes();
  int seconds = timeClient.getSeconds();

  if((minutes % 20) == 0) // to send every 20 minutes
    needUploadCloud = true;
  else
  {
    cloudUploaded = false;
    needUploadCloud = false;
  }
  
  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);
  Serial.println();
}

bool updateHumidTempe(){
  humidity = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(humidity) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");

    delayWithErrorCheck();
    humidity = -100;
    temp = -100;
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

ICACHE_RAM_ATTR void detectsMovement() {
  Serial.println("MOTION DETECTED!!!");
  digitalWrite(PIN_AC_POWER_LED_ENTRANCE, HIGH);
  startMotionTimer = true;
  lastTrigger = millis();
}

void updateSensors(){
  ssSupplyVoltRaw = analogRead(PIN_SS_SUPPLY_VOLT);
  ssSupplyVolt = MAX_SUPPLY_VOLT * ssSupplyVoltRaw / 1023;

  ssDoorMain = digitalRead(PIN_SS_DOOR_MAIN);
  ssDoorBasement = digitalRead(PIN_SS_DOOR_BASEMENT);
  ssLightBasementOn = !digitalRead(PIN_LIGHT_BASEMENT);
  
  ssDoorDetectors = (ssDoorBasement << 1) | (ssDoorMain << 0);

  ssWaterLeak = digitalRead(PIN_SS_WATER_SMOKE_BASEMENT);

  ssOtherSensors =  (ssLightBasementOn << 1) | (ssWaterLeak << 0);

  int gbError = (ssOtherSensors << 8) | ssDoorDetectors;
  if(gbError != globalError) // send to cloud only if global error triggered
    needUploadCloud = true;

  globalError = gbError;

  Serial.println();
  Serial.println("Supply volt.: " + String(ssSupplyVolt) + " - " + String(ssSupplyVoltRaw));
  Serial.println("Door sensors: " + String(ssDoorDetectors, BIN));
  Serial.println("Others sensors: " + String(ssOtherSensors, BIN));
  Serial.println("Global error: " + String(globalError, BIN));
  Serial.println();
  if(ssDoorDetectors > 0)
    forceCamPower = 1;
  else
    forceCamPower = 0;
}

void updateActuator()
{
  now = millis();
  // Turn off the LED after the number of seconds defined in the MOTION_DELAY variable
  if(startMotionTimer && (now - lastTrigger > MOTION_DELAY)) {
    Serial.println(String(now) + " - " + String(lastTrigger));
    Serial.println("Light stopped...");
    digitalWrite(PIN_AC_POWER_LED_ENTRANCE, LOW);
    startMotionTimer = false;
  }

  
  if((!ssDoorBasement) && ssLightBasementOn){
    playMelody();
  }

}
