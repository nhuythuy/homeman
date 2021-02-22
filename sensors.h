#include "lib.h"
#include "climate.h"
#include <Adafruit_ADS1015.h>
#include "cayenne.h"


#define ADC_MAX_RAW       4095              // 12 bit ADC, 1.18 (calibration factor)
#define ADS1115_VOLT_STEP 0.125             // mV
#define MAX_SUPPLY_VOLT   16.054            // volt: 10K(9990)+39K(38610) --> 3.3*(9990+38610)/9990 = 16.054 V
#define ML35_TEMP_RATIO   ADS1115_VOLT_STEP / 10
#define BATT_VOLT_RATIO   ADS1115_VOLT_STEP * (9990+38610)/(9990 * 1000)

Adafruit_ADS1115 ads(0x49);


void setupSensors(){
  pinMode(PIN_SS_DOOR_MAIN, INPUT);
  pinMode(PIN_SS_DOOR_TO_BASEMENT, INPUT);
  pinMode(PIN_SS_DOOR_BASEMENT, INPUT);
  pinMode(PIN_SS_WATER_SMOKE_BASEMENT, INPUT);
  pinMode(PIN_SS_ENTRANCE_MOTION, INPUT);
  pinMode(PIN_SS_LIGHT_BASEMENT, INPUT);
//  attachInterrupt(digitalPinToInterrupt(PIN_SS_ENTRANCE_MOTION), detectsMovement, RISING);

  analogReadResolution(12);

  ads.setGain(GAIN_ONE); // GAIN_ONE --> 1 bit = 0.125mV
  ads.begin();
  setupClimateSensors();
}

bool updateTemp(){
  int16_t adc0 = ads.readADC_SingleEnded(0);
  float lm35Temp = ML35_TEMP_RATIO * adc0;
  Serial.println("LM35 temp.: " + String(lm35Temp, 1));
  bmTempX = lm35Temp;

  bmTemp = ds1621GetTemperature();
  Serial.println("Temperature: " + String(bmTemp, 1) + " degC");

  return true;
}

void updateBattVolt(){
  int16_t adc3 = ads.readADC_SingleEnded(3);
  ssBatteryVolt = BATT_VOLT_RATIO * adc3;
}

void updateBattVoltPS(){
  int16_t adc2 = ads.readADC_SingleEnded(2);
  ssBatteryVoltPS = BATT_VOLT_RATIO * adc2;
}

void updateSensors(){
  bool state;
  state = !digitalRead(PIN_SS_DOOR_MAIN);
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

  state = !digitalRead(PIN_SS_LIGHT_BASEMENT);
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
    ssWaterLeak = state;
  }

  ssOtherSensors = (ssEntranceMotion << 2) | (ssLightBasementOn << 1) | (ssWaterLeak << 0);
  globalState = (ssOtherSensors << 8) | ssDoorDetectors;

  if(ssDoorDetectors > 0)
    forceCamPower = 1;
  else
    forceCamPower = 0;
}
