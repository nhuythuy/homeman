// May 14 2020
// Author: Thuy Nguyen
// NOTE: Disconnect Door main sensor cable to upload new code, otherwise not possible

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
#include <ArduinoJson.h>


const char* ssid = "VNNO"; // "Thuy's iPhone";
const char* password = WIFI_PW;

#define MAX_SUPPLY_VOLT   16.157    // volt: 10K(9910)+39K(38610) --> 3.3*(9910+38610)/9910 = 16.1570131181 V 
#define DELAY_LONG        5000      // 5,0 seconds
#define DELAY_SHORT       2500      // 2,5 seconds
#define MOTION_DELAY      0*60*1000 // 1 mins delay

DHT dht(PIN_SS_DHT, DHT11, 15);
WiFiClient client;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.nist.gov");

bool needUploadCloud = false;
bool cloudUploaded = false;

long delayMs = DELAY_LONG;

unsigned long timeNow = millis();
unsigned long lastTrigger = millis();
boolean startMotionTimer = false;

IPAddress serverHome(192,168,1,5);          // the fix IP address of the server
WiFiClient clientHome;


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
  Serial.println("Connected to wifi");
  Serial.print("Status: ");   Serial.println(WiFi.status());    // Network parameters
  Serial.print("IP: ");       Serial.println(WiFi.localIP());
  Serial.print("Subnet: ");   Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");  Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: ");     Serial.println(WiFi.SSID());
  Serial.print("Signal: ");   Serial.println(WiFi.RSSI());
  Serial.println();
  delay(1000);

  Serial.println("Cayenne connecting...");
  Cayenne.begin(dv_username, dv_password, dv_clientID, ssid, password);
  Serial.println("Cayenne connected!");
  delay(1000); 
}

void setup() {
  //pinMode(PIN_SS_DOOR_MAIN, INPUT);
  pinMode(PIN_SS_DOOR_BASEMENT, INPUT);
  pinMode(PIN_SS_WATER_SMOKE_BASEMENT, INPUT);
  pinMode(PIN_SS_ENTRANCE_MOTION, INPUT);
  pinMode(PIN_LIGHT_BASEMENT, INPUT);
//  attachInterrupt(digitalPinToInterrupt(PIN_SS_ENTRANCE_MOTION), detectsMovement, RISING);


  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TONE_MELODY, OUTPUT);
  pinMode(PIN_AC_POWER_LED_ENTRANCE, OUTPUT);
  //pinMode(PIN_AC_POWER_RADIO, OUTPUT);

  //Serial.begin(19200);
  Serial.begin(19200, SERIAL_8N1, SERIAL_TX_ONLY);
  delay(1000);
  dht.begin();

  WIFI_Connect();

  timeClient.begin(); // Initialize a NTPClient to get time
// Set offset time in seconds to adjust for your timezone, ex.: GMT +1 = 3600, GMT +8 = 28800, GMT -1 = -3600, GMT 0 = 0
  timeClient.setTimeOffset(3600); // Norway GMT + 1
}

bool PowerLedState = false;
int PowerLedDelay = 1000;

void blinkPowerLed(){
  PowerLedDelay += 500;
  if(PowerLedDelay > 5000)
    PowerLedDelay = 1000;

  for(int i = 0; i < 20; i++){
    PowerLedState = !PowerLedState;
    digitalWrite(PIN_AC_POWER_LED_ENTRANCE, PowerLedState);
    delay(PowerLedDelay);
  }
}

void loop() {
  MainServerComm();

  blinkPowerLed();
  updateHumidTempe();

  getServerTime();
  blinkLed();
  updateSensors();
  updateActuator();

  runtimeMinutes = millis() / 60000;
  
  if(ssDoorBasement)
    minutesDoorBasementOpened = (millis() - timeDoorBasementOpened) / 60000;
  else
    minutesDoorBasementOpened = 0;

  if(ssDoorMain)
    minutesDoorMainOpened = (millis() - timeDoorMainOpened) / 60000;
  else
    minutesDoorMainOpened = 0;

  if(ssEntranceMotion)
    motionSeconds = (millis() - timeMotionDetected) / 1000;
  else
    motionSeconds = 0;

  Serial.println("Sensors detected: " + String(minutesDoorMainOpened) + " min - " + String(minutesDoorBasementOpened) + " min - " + String(motionSeconds)  + " sec");

  Cayenne.loop();
  if(!cloudUploaded && needUploadCloud == true)
  {
    if(cayenneCounter++ > CH_HUMIDITY) // last channel
      cayenneCounter = 0;
    cloudUploaded = true;
  }

  if(WiFi.status() == WL_DISCONNECTED){
    Serial.println("WiFi connection lost! Reconnecting...");
    WiFi.disconnect();
    WIFI_Connect();    
  }

  delayWithErrorCheck();

  if((currentHours > 8) && (currentHours < 22))
    playMelody();
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
  currentHours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  int seconds = timeClient.getSeconds();
  
  if((minutes % 1) == 0) // to send every 1 minutes
    needUploadCloud = true;
  else
  {
    cloudUploaded = false;
    needUploadCloud = false;
  }
  
  Serial.print("Epoch Time: " + String(timeClient.getEpochTime()));  
  Serial.print("Formatted Time: " + timeClient.getFormattedTime());
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

  if(ssBatteryVolt > 12.50){
    //digitalWrite(PIN_AC_POWER_LED_ENTRANCE, HIGH);
    acEntranceLed = true;
    acActuators |= (1 << 0);

    startMotionTimer = true;
    Serial.println("Light entrance: ON");
    writeCayenneDigitalStates(CH_LIGHT_ENTRANCE, true);
    lastTrigger = millis();
  }
}

void updateSensors(){
  bool state;
  
  ssBatteryVoltRaw = analogRead(PIN_SS_SUPPLY_VOLT);
  ssBatteryVolt = MAX_SUPPLY_VOLT * ssBatteryVoltRaw / 1023;

  state = digitalRead(PIN_SS_DOOR_MAIN);
  if (state != ssDoorMain){
    writeCayenneDigitalStates(CH_DOOR_MAIN, state);
    if(state)
      timeDoorMainOpened = millis();
    else
      timeDoorMainOpened = 0;

    ssDoorMain = state;
  }

  state = digitalRead(PIN_SS_DOOR_BASEMENT);
  if (state != ssDoorBasement){
    writeCayenneDigitalStates(CH_DOOR_BASEMENT, state);
    writeCayenneDigitalStates(CH_LIGHT_STAIR_BASEMENT, state);
    if(state)
      timeDoorBasementOpened = millis();
    else
      timeDoorBasementOpened = 0;

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

    if(state)
      timeMotionDetected = millis();
    else
      timeMotionDetected = 0;

    ssEntranceMotion = state;
  }

  ssDoorDetectors = (ssDoorBasement << 1) | (ssDoorMain << 0);

  ssWaterLeak = 0; // digitalRead(PIN_SS_WATER_SMOKE_BASEMENT);

  ssOtherSensors = (ssEntranceMotion << 2) | (ssLightBasementOn << 1) | (ssWaterLeak << 0);

  int gbSensorState = (ssOtherSensors << 8) | ssDoorDetectors;
  if(gbSensorState != globalState) // send to cloud only if global error triggered
    needUploadCloud = true;

  globalState = gbSensorState;

  Serial.println();
  Serial.println("0. Battery volt.:     " + String(ssBatteryVolt) + " - " + String(ssBatteryVoltRaw));
  Serial.println("1. Temperature:       " + String(temp) + " deg C");
  Serial.println("2. Humidity:          " + String(humidity) + " %");
  Serial.println("3. Door sensors:      " + String(ssDoorDetectors, BIN));
  Serial.println("3.1. Door main:       " + String(ssDoorMain, BIN));
  Serial.println("3.2. Door basement:   " + String(ssDoorBasement, BIN));
  Serial.println("4. Others sensors:    " + String(ssOtherSensors, BIN));
  Serial.println("4.1 Light basement:   " + String(ssLightBasementOn, BIN));
  Serial.println("4.2. Entrance motion: " + String(ssEntranceMotion, BIN));
  Serial.println("4.3. Water Smoke:     " + String(ssWaterLeak, BIN));
  Serial.println("-- Global state:      " + String(globalState, BIN));
  Serial.println("5. Actuators:         " + String(acActuators, BIN));
  Serial.println("Radio power force:    " + String(forceRadioPower));
  Serial.println();
  if(ssDoorDetectors > 0)
    forceCamPower = 1;
  else
    forceCamPower = 0;
}

void MainServerComm(){
  clientHomeHeartbeat++;

  clientHome.connect(serverHome, 80);   // Connection to the server
  digitalWrite(PIN_LED, LOW);       // to show the communication only (inverted logic)
  Serial.println("Connecting to server (Living room)");
//  clientHome.println("Hello Home server! Are you sleeping?\r");  // sends the message to the server
//  String answer = clientHome.readStringUntil('\r');   // receives the answer from the sever
//  Serial.println("from server (Living room): " + answer);

  // send client state to the server
  // https://arduinojson.org/v6/example/

  clientHome.println("livingroomstate\r");
  String reply = clientHome.readStringUntil('\r');   // receives the answer from the sever
  Serial.println("from server (Living room): " + reply);

  DynamicJsonDocument doc(256);
  deserializeJson(doc, reply);
  String node = doc["node"];
  int heartbeat = doc["heartbeat"];
  runtimeMinutesLivingRoom = doc["runtime"];
//  float ssBatteryVolt = String(doc["battvolt"]).toFloat();
//  String tmp = doc["temp"];
//  temp = tmp.toFloat();
//  String humid = doc["humidity"];
//  humidity = humid.toFloat();

  ssDoorBack = doc["ssDoorBack"];
  minutesDoorBackOpened = doc["ssDoorBackOpenMin"];

  Serial.println("from server (Living room): " + String(runtimeMinutesLivingRoom) + " - " + String(ssDoorBack) + " - " + String(minutesDoorBackOpened));

  clientHome.flush();
  digitalWrite(PIN_LED, HIGH);

}

// In 3.0.0 there will be a getDay() function.
// It will return 0 - 6, from Sunday to Saturday.
void powerRadio(){
  int currentDay = timeClient.getDay();
  int currentHour = timeClient.getHours();
  Serial.println("Current day:  " + String(currentDay) + ", hour: " + String(currentHour));

  if ((ssBatteryVolt > 13.0) // only if battery is full enough
    && (currentHour < 20)    // no later than 19:00
    && ((((currentDay == 0) || (currentDay == 6)) && (currentHour > 10)) // Sunday or Saturday
      || ((currentDay > 0) && (currentDay < 6) && (currentHour > 9)))){  // weekdays
    //digitalWrite(PIN_AC_POWER_RADIO, true);
    //Serial.println("Radio power: ON");
    writeCayenneDigitalStates(CH_POWER_RADIO, true);
  }
  else{
    //digitalWrite(PIN_AC_POWER_RADIO, false);
    //Serial.println("Radio power: OFF");
    writeCayenneDigitalStates(CH_POWER_RADIO, false);
  }
}

void updateActuator()
{
  timeNow = millis();
  // Turn off the LED after the number of seconds defined in the MOTION_DELAY variable
  if(startMotionTimer && (timeNow - lastTrigger > MOTION_DELAY)) {
    Serial.println(String(timeNow) + " - " + String(lastTrigger));
    Serial.println("Light stopped...");
    //digitalWrite(PIN_AC_POWER_LED_ENTRANCE, LOW);
    acEntranceLed = false;
    Serial.println("Light entrance: OFF");
    writeCayenneDigitalStates(CH_LIGHT_ENTRANCE, false);

    acActuators &= ~(1 << 0);
    startMotionTimer = false;
  }

  powerRadio();

  // play melody only twice if it happens during the sleeping time 22:00 to 8:00
  // can detect door to basement OPENED/CLOSED only if the solar charger giving power from the battery
  if((!ssDoorBasement) && ssLightBasementOn && (ssBatteryVolt > 12.0)){
    if((currentHours > 8) && (currentHours < 22)){
      Serial.println("Playing melody...");
      playMelody();
    }
    else{
      if (playMelodyCounter < 2){
          playMelody();
          playMelodyCounter++;
      }
    }
  }
  else
    playMelodyCounter = 0;

}
