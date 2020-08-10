
// Plot DTH11 data on thingspeak.com using an ESP8266
// May 14 2020
// Author: Thuy Nguyen, based on an examples from Jeroen Beemster reading DTH11 sensor

// Ref.:
// Website: www.arduinesp.com
// https://learn.adafruit.com/dht/using-a-dhtxx-sensor


#include <DHT.h>
#include "global_vars.h"
#include "mydevices.h"
#include <ESP8266WiFi.h>
#include "wifi_pw.h"
#include "pin_define.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "melody.h"

#define CAYENNE_PRINT Serial

// replace with your channel's thingspeak API key, 
const char* ssid = "VNNO"; // "DNVGuest" "Thuy's iPhone"; "matsuya";
const char* password = WIFI_PW;

#define MAX_SUPPLY_VOLT   16.157    // volt: 10K(9910)+39K(38610) --> 3.3*(9910+38610)/9910 = 16.1570131181 V 
#define DELAY_LONG        5000      // 5,0 seconds
#define DELAY_SHORT       2500      // 2,5 seconds
#define MOTION_DELAY      1*60*1000  // 1 mins delay

DHT dht(PIN_SS_DHT, DHT11,15);
WiFiClient client;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.nist.gov");

int minutes = 0; // use this for sending to update to send to thingspeak
bool needUploadCloud = false;
bool cloudUploaded = false;

long delayMs = DELAY_LONG;

unsigned long now = millis();
unsigned long lastTrigger = millis();
boolean startMotionTimer = false;

void WIFI_Connect(){
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

  delay(500);
  Serial.println("... WiFi connected, IP: " + WiFi.localIP());
  Serial.println();
}

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

  WIFI_Connect();

  playMelody();

  timeClient.begin(); // Initialize a NTPClient to get time
// Set offset time in seconds to adjust for your timezone, ex.: GMT +1 = 3600, GMT +8 = 28800, GMT -1 = -3600, GMT 0 = 0
  timeClient.setTimeOffset(3600); // Norway GMT + 1

  Cayenne.begin(dv_username, dv_password, dv_clientID, ssid, password);
}

void loop() {
  updateHumidTempe();

  getServerTime();
  blinkLed();
  updateSensors();
  updateActuator();

  if(!cloudUploaded && needUploadCloud == true)
  {
    Cayenne.loop();
    cloudUploaded = true;
  }

  if(WiFi.status() == WL_DISCONNECTED){
    Serial.println("WiFi connection lost! Reconnecting...");
    WiFi.disconnect();
    WIFI_Connect();
  }

  delayWithErrorCheck();
}


void blinkLed(){
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

  if((minutes % 1) == 0) // to send every 1 minutes
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
    if(globalState > 0)
    blinkLed();
  else
    delay(delayMs);
}

ICACHE_RAM_ATTR void detectsMovement() {
  Serial.println("MOTION DETECTED!!!");
  digitalWrite(PIN_AC_POWER_LED_ENTRANCE, HIGH);
  acEntranceLed = true;
  acActuators |= (1 << 0);

  startMotionTimer = true;
  writeCayenneDigitalStates(CH_LIGHT_ENTRANCE, false);
  lastTrigger = millis();
}

void updateSensors(){
  bool state;
  
  ssBatteryVoltRaw = analogRead(PIN_SS_SUPPLY_VOLT);
  ssBatteryVolt = MAX_SUPPLY_VOLT * ssBatteryVoltRaw / 1023;

  state = digitalRead(PIN_SS_DOOR_MAIN);
  if (state != ssDoorMain){
    writeCayenneDigitalStates(CH_DOOR_MAIN, state);
    ssDoorMain = state;
  }
  
  state = digitalRead(PIN_SS_DOOR_BASEMENT);
  if (state != ssDoorBasement){
    writeCayenneDigitalStates(CH_DOOR_BASEMENT, state);
    writeCayenneDigitalStates(CH_LIGHT_STAIR_BASEMENT, state);
    ssDoorBasement = state;
  }

  state = !digitalRead(PIN_LIGHT_BASEMENT);
  if (state != ssLightBasementOn){
    writeCayenneDigitalStates(CH_LIGHT_BASEMENT, state);
    ssLightBasementOn = state;
  }

  state = digitalRead(PIN_SS_ENTRANCE_MOTION);
  if (state != ssEntranceMotion){
    writeCayenneDigitalStates(CH_MOTION_ENTRANCE, state);
    ssEntranceMotion = state;
  }

  ssDoorDetectors = (ssDoorBasement << 1) | (ssDoorMain << 0);

  ssWaterLeak = 0; // digitalRead(PIN_SS_WATER_SMOKE_BASEMENT);

  ssOtherSensors = (ssEntranceMotion << 2) | (ssLightBasementOn << 1) | (ssWaterLeak << 0);

  long gbState = (acActuators << 16) | (ssOtherSensors << 8) | ssDoorDetectors;
  if(gbState != globalState) // send to cloud only if global error triggered
    needUploadCloud = true;

  globalState = gbState;

  Serial.println();
  Serial.println("0. Battery volt.:  " + String(ssBatteryVolt) + " - " + String(ssBatteryVoltRaw));
  Serial.println("1. Door sensors:   " + String(ssDoorDetectors, BIN));
  Serial.println("2. Others sensors: " + String(ssOtherSensors, BIN));
  Serial.println("3. Actuators:      " + String(acActuators, BIN));
  Serial.println("4. Temperature:    " + String(temp) + " deg C");
  Serial.println("5. Humidity:       " + String(humidity) + " %");
  Serial.println("-- Global state:   " + String(globalState, BIN));
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
    acEntranceLed = false;
    writeCayenneDigitalStates(CH_LIGHT_ENTRANCE, false);

    acActuators &= ~(1 << 0);
    startMotionTimer = false;
  }

  
  if((!ssDoorBasement) && ssLightBasementOn){
    playMelody();
  }

}
