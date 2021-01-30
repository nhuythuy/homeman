// May 14 2020
// Author: Thuy Nguyen

// Ref.:
// Website: www.arduinesp.com
// https://learn.adafruit.com/dht/using-a-dhtxx-sensor

#define ENABLE_WIFI
#define ENABLE_BLYNK
#define ENABLE_CAYENNE

#include <DHT.h>
#include "global_vars.h"
#include "mydevices.h"
#include <WiFi.h>
#include "wifi_pw.h"
#include "pin_define.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "melody.h"
#include "blynk.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include "ds1621.h"
#include <Adafruit_ADS1015.h>

const char* wifiSsid = "THUY"; // "VNNO"; // "Thuy";
const char* wifiPassword = "thuy2105";//WIFI_PW;

#define ADS1115_VOLT_STEP 0.125
#define MAX_SUPPLY_VOLT   1.22*16.054          // volt: 10K(9990)+39K(38610) --> 3.3*(9990+38610)/9990 = 16.054 V 
#define SUPPLY_VOLT_RATIO 1.22*16.054/1023.0 // 10 bit ADC, 1.18 (calibration factor) 
#define DELAY_LONG        5000            // 5,0 seconds
#define DELAY_SHORT       2500            // 2,5 seconds
#define MOTION_DELAY      0*60*1000       // 1 mins delay

DHT dht(PIN_SS_DHT, DHT11, 15);
Adafruit_ADS1115 ads(0x49);

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
  Serial.println("MAC: " + WiFi.macAddress()); Serial.println();
  Serial.println("Connecting to " + String(wifiSsid));

  WiFi.begin(wifiSsid, wifiPassword);

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
  Cayenne.begin(dvUsername, dvPassword, dvClientID, wifiSsid, wifiPassword);
  Serial.println("Cayenne connected!");
  delay(500);
  blynkSetup();
  delay(500);
}

void setup() {
  pinMode(PIN_SS_DOOR_MAIN, INPUT);
  pinMode(PIN_SS_DOOR_TO_BASEMENT, INPUT);
  pinMode(PIN_SS_DOOR_BASEMENT, INPUT);
  pinMode(PIN_SS_WATER_SMOKE_BASEMENT, INPUT);
  pinMode(PIN_SS_ENTRANCE_MOTION, INPUT);
  pinMode(PIN_LIGHT_BASEMENT, INPUT);
//  attachInterrupt(digitalPinToInterrupt(PIN_SS_ENTRANCE_MOTION), detectsMovement, RISING);

  analogReadResolution(10);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TONE_MELODY, OUTPUT);
  pinMode(PIN_AC_POWER_LED_ENTRANCE, OUTPUT);
  //pinMode(PIN_AC_POWER_RADIO, OUTPUT);

  Serial.begin(19200);
//--  Serial.begin(19200, SERIAL_8N1, SERIAL_TX_ONLY);
  delay(1000);
  dht.begin();
  ads.setGain(GAIN_ONE);
  ads.begin();
  ds1621Setup();

#ifdef ENABLE_WIFI
  WIFI_Connect();
#endif
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
#ifdef ENABLE_WIFI
  MainServerComm();
  getServerTime();
#endif
  blinkPowerLed();
  updateTemp();

  blinkLed();
  updateSensors();
  updateActuator();

  bmRuntimeMinutes = millis() / 60000;
  
  if(ssDoorToBasement)
    doorToBasementOpenedMinutes = (millis() - doorToBasementOpenedAt) / 60000;
  else
    doorToBasementOpenedMinutes = 0;

  if(ssDoorBasement)
    doorBasementOpenedMinutes = (millis() - doorBasementOpenedAt) / 60000;
  else
    doorBasementOpenedMinutes = 0;

  if(ssDoorMain)
    doorMainOpenedMinutes = (millis() - doorMainOpenedAt) / 60000;
  else
    doorMainOpenedMinutes = 0;

  if(ssEntranceMotion)
    entranceMotionSeconds = (millis() - entranceMotionDetectedAt) / 1000;
  else
    entranceMotionSeconds = 0;

  Serial.println("Sensors detected: " + String(doorMainOpenedMinutes) + " min - "
  + String(doorToBasementOpenedMinutes) + " min - "
  + String(doorBasementOpenedMinutes) + " min - "
  + String(entranceMotionSeconds)  + " sec");

#ifdef ENABLE_WIFI
#ifdef ENABLE_CAYENNE
  Cayenne.loop();
#endif
#ifdef ENABLE_BLYNK
  blynkLoop();
#endif
#endif
  if(!cloudUploaded && needUploadCloud == true)
  {
    if(cayenneCounter++ > CH_BM_HUMIDITY) // last channel
      cayenneCounter = 0;
    cloudUploaded = true;
  }

#ifdef ENABLE_WIFI
  if(WiFi.status() == WL_DISCONNECTED){
    Serial.println("WiFi connection lost! Reconnecting...");
    WiFi.disconnect();
    WIFI_Connect();    
  }
#endif
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
  Serial.println(" - " + timeClient.getFormattedTime());
}

bool updateTemp(){
//  int valRaw = analogRead(PIN_SS_TEMP);
//  float volt = (valRaw / 1023.0) * 3.3;
//  bmTemp = 100* volt;
  bmTemp = ds1621GetTemperature();
  Serial.println("Temperature: " + String(bmTemp, 1));

  return true;
}

bool updateHumidTempe(){
  bmHumidity = dht.readHumidity();
  bmTemp = dht.readTemperature();
  if (isnan(bmHumidity) || isnan(bmTemp)) {
    Serial.println("Failed to read from DHT sensor!");

    delayWithErrorCheck();
    bmHumidity = -100;
    bmTemp = -100;
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
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_ENTRANCE_LIGHT, true);
#endif
    lastTrigger = millis();
  }
}

void updateSensors(){
  bool state;

  ssBatteryVoltRaw = analogRead(PIN_SS_SUPPLY_VOLT);
  ssBatteryVolt = SUPPLY_VOLT_RATIO * ssBatteryVoltRaw;

  int valRaw = analogRead(35);
  float Voltage = (valRaw / 1023.0) * 3.3;
  Serial.println("RAW: " + String(valRaw) + " - " + String(Voltage));

  int16_t adc0, adc1, adc2, adc3;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  Serial.println("AIN0: " + String(adc0) + " - " + String(4.096*adc0/65535) + " - " + String(ADS1115_VOLT_STEP*adc0));
  Serial.println("AIN1: " + String(adc1) + " - " + String(4.096*adc1/65535) + " - " + String(ADS1115_VOLT_STEP*adc1));
  Serial.println("AIN2: " + String(adc2) + " - " + String(4.096*adc2/65535) + " - " + String(ADS1115_VOLT_STEP*adc2));
  Serial.println("AIN3: " + String(adc3) + " - " + String(4.096*adc3/65535) + " - " + String(ADS1115_VOLT_STEP*adc3));
  
  Serial.println(" ");
  
  state = digitalRead(PIN_SS_DOOR_MAIN);
  if (state != ssDoorMain){
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_DOOR_MAIN, state);
#endif
    if(state)
      doorMainOpenedAt = millis();
    else
      doorMainOpenedAt = 0;

    ssDoorMain = state;
  }

  state = digitalRead(PIN_SS_DOOR_TO_BASEMENT);
  if (state != ssDoorToBasement){
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_DOOR_TO_BASEMENT, state);
    writeCayenneDigitalStates(CH_LIGHT_STAIR_BASEMENT, state);
#endif
    if(state)
      doorToBasementOpenedAt = millis();
    else
      doorToBasementOpenedAt = 0;

    ssDoorToBasement = state;
  }

  state = digitalRead(PIN_SS_DOOR_BASEMENT);
  if (state != ssDoorBasement){
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_DOOR_BASEMENT, state);
    writeCayenneDigitalStates(CH_LIGHT_STAIR_BASEMENT, state);
#endif
    if(state)
      doorBasementOpenedAt = millis();
    else
      doorBasementOpenedAt = 0;

    ssDoorBasement = state;
  }

  state = !digitalRead(PIN_LIGHT_BASEMENT);
  if (state != ssLightBasementOn){
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_LIGHT_BASEMENT, state);
#endif
    ssLightBasementOn = state;
  }

  state = digitalRead(PIN_SS_ENTRANCE_MOTION);
  if (state != ssEntranceMotion){
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_ENTRANCE_MOTION, state);
#endif
    if(state)
      entranceMotionDetectedAt = millis();
    else
      entranceMotionDetectedAt = 0;

    ssEntranceMotion = state;
  }

  ssDoorDetectors = (ssDoorBasement << 1) | (ssDoorToBasement << 1) | (ssDoorMain << 0);

  //ssWaterLeak = 0; // digitalRead(PIN_SS_WATER_SMOKE_BASEMENT);
  state = digitalRead(PIN_SS_WATER_SMOKE_BASEMENT);
  if (state != ssWaterLeak){
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_WATER_SMOKE_BASEMENT, state);
#endif
//    if(state)
//      entranceMotionDetectedAt = millis();
//    else
//      entranceMotionDetectedAt = 0;

    ssWaterLeak = state;
  }

  ssOtherSensors = (ssEntranceMotion << 2) | (ssLightBasementOn << 1) | (ssWaterLeak << 0);

  int gbSensorState = (ssOtherSensors << 8) | ssDoorDetectors;
  if(gbSensorState != globalState) // send to cloud only if global error triggered
    needUploadCloud = true;

  globalState = gbSensorState;

  Serial.println();
  Serial.println("0. Battery volt.:       " + String(ssBatteryVolt) + " - " + String(ssBatteryVoltRaw));
  Serial.println("1. Temperature:         " + String(bmTemp) + " deg C");
  Serial.println("2. Humidity:            " + String(bmHumidity) + " %");
  Serial.println("3. Door sensors:        " + String(ssDoorDetectors, BIN));
  Serial.println("3.1. Door main:         " + String(ssDoorMain, BIN));
  Serial.println("3.2. Door to basement:  " + String(ssDoorToBasement, BIN));
  Serial.println("3.3. Door basement:     " + String(ssDoorBasement, BIN));
  Serial.println("4. Others sensors:      " + String(ssOtherSensors, BIN));
  Serial.println("4.1 Light basement:     " + String(ssLightBasementOn, BIN));
  Serial.println("4.2. Entrance motion:   " + String(ssEntranceMotion, BIN));
  Serial.println("4.3. Water Smoke:       " + String(ssWaterLeak, BIN));
  Serial.println("-- Global state:        " + String(globalState, BIN));
  Serial.println("5. Actuators:           " + String(acActuators, BIN));
  Serial.println("Radio power force:      " + String(forceRadioPower));
  Serial.println();
  if(ssDoorDetectors > 0)
    forceCamPower = 1;
  else
    forceCamPower = 0;
}

void MainServerComm(){
  if(!clientHome.connect(serverHome, 80)){
    Serial.println("Cannot connect to server (Living room)!");
    return;
  }

  digitalWrite(PIN_LED, LOW);       // to show the communication only (inverted logic)
  Serial.println("Connecting to server (Living room)");
//  clientHome.println("Hello Home server! Are you sleeping?\r");  // sends the message to the server
//  String answer = clientHome.readStringUntil('\r');   // receives the answer from the sever
//  Serial.println("from server (Living room): " + answer);

  // send client state to the server
  // https://arduinojson.org/v6/example/

  clientHome.println("livingroomstate:" + String(bmHeartbeat++) + "\r");
  String reply = clientHome.readStringUntil('\r');   // receives the answer from the sever
  Serial.println("from server (Living room): " + reply);

  DynamicJsonDocument doc(256);
  deserializeJson(doc, reply);
  String node = doc["node"];
  int heartbeat = doc["heartbeat"];
  lrRuntimeMinutes = doc["runtime"];
  String tmp = doc["temp"];
  lrTemp = tmp.toFloat();
  String humid = doc["humidity"];
  lrHumidity = humid.toFloat();

  bool state = doc["ssDoorBack"];
  if (state != ssDoorBack){
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_DOOR_BACK, state);
#endif
    ssDoorBack = state;
  }

  doorBackOpenedMinutes = doc["ssDoorBackOpenMin"];

  Serial.println("from server (Living room): Runtime (" + String(lrRuntimeMinutes)
  + "), Temp: (" + String(lrTemp)
  + "), Humidity: (" + String(lrHumidity)
  + "), Door back: (" + String(ssDoorBack)
  + "), Door back opened: (" + String(doorBackOpenedMinutes) + ") min");

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
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_POWER_RADIO, true);
#endif
  }
  else{
    //digitalWrite(PIN_AC_POWER_RADIO, false);
    //Serial.println("Radio power: OFF");
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_POWER_RADIO, false);
#endif
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
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_ENTRANCE_LIGHT, false);
#endif
    acActuators &= ~(1 << 0);
    startMotionTimer = false;
  }

  powerRadio();

  // play melody only twice if it happens during the sleeping time 22:00 to 8:00
  // can detect door to basement OPENED/CLOSED only if the solar charger giving power from the battery
  if((!ssDoorToBasement) && ssLightBasementOn && (ssBatteryVolt > 12.0)){
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
