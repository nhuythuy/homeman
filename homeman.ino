// May 14 2020
// Author: Thuy Nguyen

// Ref.:
// Website: www.arduinesp.com
// https://learn.adafruit.com/dht/using-a-dhtxx-sensor

#define ENABLE_WIFI
#define ENABLE_BLYNK
#define ENABLE_CAYENNE

#include "sensors.h"
#include "actuators.h"
#include "mydevices.h"
#include "datetime.h"
#include "wifi_cloud.h"
#include "pin_define.h"
#include <WiFiUdp.h>
#include "melody.h"
#include "blynk.h"
#include <ArduinoJson.h>



void setup() {
  setupSensors();

  setupActuators();

  Serial.begin(19200);
//--  Serial.begin(19200, SERIAL_8N1, SERIAL_TX_ONLY);
  delay(1000);

#ifdef ENABLE_WIFI
  WIFI_Connect();
  cayenneSetup();
  blynkSetup();


  setupDateTime();
#endif
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
