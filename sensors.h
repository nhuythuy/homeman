#include "lib.h"
#include <DHT.h>
#include "ds1621.h"
#include <Adafruit_ADS1015.h>
#include "mydevices.h"


#define ADC_MAX_RAW       4095              // 12 bit ADC, 1.18 (calibration factor)
#define ADS1115_VOLT_STEP 0.125             // mV
#define MAX_SUPPLY_VOLT   16.054            // volt: 10K(9990)+39K(38610) --> 3.3*(9990+38610)/9990 = 16.054 V
#define BATT_VOLT_RATIO   ADS1115_VOLT_STEP * (9990+38610)/(9990 * 1000)

DHT dht(PIN_SS_DHT, DHT11, 15);
Adafruit_ADS1115 ads(0x49);


void setupSensors(){
  pinMode(PIN_SS_DOOR_MAIN, INPUT);
  pinMode(PIN_SS_DOOR_TO_BASEMENT, INPUT);
  pinMode(PIN_SS_DOOR_BASEMENT, INPUT);
  pinMode(PIN_SS_WATER_SMOKE_BASEMENT, INPUT);
  pinMode(PIN_SS_ENTRANCE_MOTION, INPUT);
  pinMode(PIN_LIGHT_BASEMENT, INPUT);
//  attachInterrupt(digitalPinToInterrupt(PIN_SS_ENTRANCE_MOTION), detectsMovement, RISING);

  analogReadResolution(12);

  dht.begin();
  ads.setGain(GAIN_ONE); // GAIN_ONE --> 1 bit = 0.125mV
  ads.begin();
  ds1621Setup();
}

bool updateTemp(){
  int16_t adc0 = ads.readADC_SingleEnded(0);
  float lm35Temp = ADS1115_VOLT_STEP * adc0 / 10;
  Serial.println("ADS1115 Raw: " + String(adc0) + " - " + String(ADS1115_VOLT_STEP*adc0, 1) + " (mV) - ESP32 temp: " + String(lm35Temp, 1));

  bmTemp = ds1621GetTemperature();
  Serial.println("Temperature: " + String(bmTemp, 1) + " - " + String(100 * volt));

  return true;
}

void delayWithErrorCheck(){
    if(globalState > 0)
    blinkLed();
  else
    delay(delayMs);
}

bool updateHumidTemp(){
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

void updateSensors(){
  bool state;

  int16_t adc3 = ads.readADC_SingleEnded(3);
  ssBatteryVolt = BATT_VOLT_RATIO * adc3;

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
  if(ssDoorDetectors > 0)
    forceCamPower = 1;
  else
    forceCamPower = 0;
}
