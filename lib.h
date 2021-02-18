#include "global_vars.h"
#include "pin_define.h"


#ifndef LIB
#define LIB


void flipLed(){
  stateLed = !stateLed;
  digitalWrite(PIN_LED, stateLed);
}

void blinkLed(){
  flipLed();
  flipLed();
}

void delayWithErrorCheck(){
  if(globalState > 0)
    blinkLed();
}


void printDebugSerial(){
  Serial.println();
  Serial.println("0. Battery volt.:       " + String(ssBatteryVolt, 1) + " (V)");
  Serial.println("1. Temperature:         " + String(bmTemp, 1) + " deg C");
  Serial.println("2. Humidity:            " + String(bmHumidity, 1) + " %");
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
}

void printDebugSerialBT(){
  SerialBT.println();
  SerialBT.println("0. Battery volt.:       " + String(ssBatteryVolt, 1) + " (V)");
  SerialBT.println("1. Temperature:         " + String(bmTemp, 1) + " deg C");
  SerialBT.println("2. Humidity:            " + String(bmHumidity, 1) + " %");
  SerialBT.println("3. Door sensors:        " + String(ssDoorDetectors, BIN));
  SerialBT.println("3.1. Door main:         " + String(ssDoorMain, BIN));
  SerialBT.println("3.2. Door to basement:  " + String(ssDoorToBasement, BIN));
  SerialBT.println("3.3. Door basement:     " + String(ssDoorBasement, BIN));
  SerialBT.println("4. Others sensors:      " + String(ssOtherSensors, BIN));
  SerialBT.println("4.1 Light basement:     " + String(ssLightBasementOn, BIN));
  SerialBT.println("4.2. Entrance motion:   " + String(ssEntranceMotion, BIN));
  SerialBT.println("4.3. Water Smoke:       " + String(ssWaterLeak, BIN));
  SerialBT.println("-- Global state:        " + String(globalState, BIN));
  SerialBT.println("5. Actuators:           " + String(acActuators, BIN));
  SerialBT.println("Radio power force:      " + String(forceRadioPower));
  SerialBT.println();
}

void updateDurations(){
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

  Serial.println("Door main: " + String(doorMainOpenedMinutes) + " min, Door to BM: "
    + String(doorToBasementOpenedMinutes) + " min, Door BM" + String(doorBasementOpenedMinutes) + " min, Door back: "
    + String(doorBackOpenedMinutes) + " min, Entrance Motion: " + String(entranceMotionSeconds)  + " sec");
}

#endif
