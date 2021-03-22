#include "global_vars.h"
#include "pin_define.h"


#ifndef LIB
#define LIB


void flipLed(){
  stateLed = !stateLed;
  digitalWrite(PIN_LED, stateLed);
}

float round2(float value) {
  return (int)(value * 100 + 0.5) / 100.0;
}

void printDebugSerial(){
//  Serial.println();
//  Serial.println("0. Battery volt.:       " + String(ssBatteryVolt, 1) + " (V)");
//  Serial.println("1. Temperature:         " + String(bmTemp, 1) + " deg C");
//  Serial.println("2. Humidity:            " + String(bmHumidity, 1) + " %");
//  Serial.println("3. Door sensors:        " + String(ssDoorDetectors, BIN));
//  Serial.println("3.1. Door main:         " + String(ssDoorMain, BIN));
//  Serial.println("3.2. Door to basement:  " + String(ssDoorToBasement, BIN));
//  Serial.println("3.3. Door basement:     " + String(ssDoorBasement, BIN));
//  Serial.println("4. Others sensors:      " + String(ssOtherSensors, BIN));
//  Serial.println("4.1 Light basement:     " + String(ssLightBasementOn, BIN));
//  Serial.println("4.2. Entrance motion:   " + String(ssEntranceMotion, BIN));
//  Serial.println("4.3. Water Smoke:       " + String(ssWaterLeak, BIN));
//  Serial.println("5. Runtime:             " + String(runtimeMinutes));
//  Serial.println("--------------------------------------------");
//  Serial.println("20. Actuators:           " + String(acActuators, BIN));
//  Serial.println("Radio power force:      " + String(forceRadioPower));
//  Serial.println();
}

#ifdef ENABLE_BLUETOOTH
void printDebugSerialBT(){
  yield();
  SerialBT.println();
  SerialBT.println("0. Battery volt.:       " + String(ssBatteryVolt, 1) + " (V)");
  SerialBT.println("1. Temperature:         " + String(bmTemp, 1) + " deg C");
  SerialBT.println("2. Humidity:            " + String(bmHumidity, 1) + " %");
  yield();
  SerialBT.println("3. Door sensors:        " + String(ssDoorDetectors, BIN));
  SerialBT.println("3.1. Door main:         " + String(ssDoorMain, BIN));
  SerialBT.println("3.2. Door to basement:  " + String(ssDoorToBasement, BIN));
  SerialBT.println("3.3. Door basement:     " + String(ssDoorBasement, BIN));
  yield();
  SerialBT.println("4. Others sensors:      " + String(ssOtherSensors, BIN));
  SerialBT.println("4.1 Light basement:     " + String(ssLightBasementOn, BIN));
  SerialBT.println("4.2. Entrance motion:   " + String(ssEntranceMotion, BIN));
  SerialBT.println("4.3. Water Smoke:       " + String(ssWaterLeak, BIN));
  yield();
  SerialBT.println("5. Runtime:             " + String(runtimeMinutes));
  SerialBT.println("--------------------------------------------");
  SerialBT.println("20. Actuators:           " + String(acActuators, BIN));
  SerialBT.println("Radio power force:      " + String(forceRadioPower));
  SerialBT.println();
}
#endif

void updateDurations(){
  yield();
  if(ssDoorToBasement)
    doorToBasementOpenedMinutes = (millis() - doorToBasementOpenedAt) / 60000;
  else
    doorToBasementOpenedMinutes = 0;

  if(ssDoorBasement)
    doorBasementOpenedMinutes = (millis() - doorBasementOpenedAt) / 60000;
  else
    doorBasementOpenedMinutes = 0;

  yield();
  if(ssDoorMain)
    doorMainOpenedMinutes = (millis() - doorMainOpenedAt) / 60000;
  else
    doorMainOpenedMinutes = 0;

  if(ssEntranceMotion)
    entranceMotionSeconds = (millis() - entranceMotionDetectedAt) / 1000;
  else
    entranceMotionSeconds = 0;

  yield();
  Serial.println("Door main: " + String(doorMainOpenedMinutes) + " min, Door to BM: "
    + String(doorToBasementOpenedMinutes) + " min, Door BM " + String(doorBasementOpenedMinutes) + " min, Door back: "
    + String(doorBackOpenedMinutes) + " min, Entrance Motion: " + String(entranceMotionSeconds)  + " sec");
}

// =======================================================
// Utility function to handle heartbeat pulse generation LED and a serial message
// https://chrisramsay.co.uk/posts/2015/04/giving-an-arduino-a-heartbeat/
void heartbeat(String message) {
  pinMode(PIN_HEART_BEAT_PULSE, OUTPUT);  // Sink current to drain charge from C2
  digitalWrite(PIN_HEART_BEAT_PULSE, LOW);

  flipLed();
  delay(300);                             // Give enough time for C2 to discharge (should discharge in 50 ms)
  pinMode(PIN_HEART_BEAT_PULSE, INPUT);   // Return to high impedance
  flipLed();

  Serial.println(message);
}
#endif
