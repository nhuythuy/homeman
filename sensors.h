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

void setupAds1115(){
  analogReadResolution(12);
  ads.setGain(GAIN_ONE); // GAIN_ONE --> 1 bit = 0.125mV
  ads.begin();
  
}

void setupSensors(){
  pinMode(PIN_SS_DOOR_MAIN, INPUT);
  pinMode(PIN_SS_DOOR_STAIR_BM, INPUT);
  pinMode(PIN_SS_DOOR_BASEMENT, INPUT);
  pinMode(PIN_SS_WATER_SMOKE_BASEMENT, INPUT);
  pinMode(PIN_SS_ENTRANCE_MOTION, INPUT);
  pinMode(PIN_SS_LIGHT_BASEMENT, INPUT);
//  attachInterrupt(digitalPinToInterrupt(PIN_SS_ENTRANCE_MOTION), detectsMovement, RISING);

  setupAds1115();
  
//  setupClimateSensors();
}

bool updateTemp(){
  int16_t adc0 = ads.readADC_SingleEnded(0);
  stTemp = ML35_TEMP_RATIO * adc0;
  Serial.println("Storage temp.: " + String(stTemp, 1));

//  srTemp = ds1621GetTemperature();
//  Serial.println("Stair temp.: " + String(srTemp, 1) + " degC");

  updateBasementClimate();

  return true;
}

void updateBattVolt(){
  int16_t adc3 = ads.readADC_SingleEnded(3);
  ssBatteryVolt = round2(BATT_VOLT_RATIO * adc3);
  Serial.println("Batt. Volt.: " + String(ssBatteryVolt) + " V");
}

void updateBattVoltPS(){
  int16_t adc2 = ads.readADC_SingleEnded(2);
  ssBatteryVoltPS = BATT_VOLT_RATIO * adc2;
}


void updateSlowAnalogSensors(){
  yield();
  updateTemp();
  yield();
  updateBattVolt();
}

void updateDigitalSensors(){
  yield();
  bool state;
  state = !digitalRead(PIN_SS_DOOR_MAIN);
  if (state != ssDoorMain){
#ifdef ENABLE_CAYENNE
    writeCayenneDigitalState(CH_DOOR_MAIN, state);
#endif
    if(state)
      doorMainOpenedAt = millis();
    else
      doorMainOpenedAt = 0;

    ssDoorMain = state;
  }

  state = digitalRead(PIN_SS_DOOR_STAIR_BM);
  if (state != ssDoorStairBm){
#ifdef ENABLE_CAYENNE
    writeCayenneDigitalState(CH_DOOR_STAIR_BM, state);
    writeCayenneDigitalState(CH_LIGHT_STAIR_BASEMENT, state);
#endif
    if(state)
      doorStairBmOpenedAt = millis();
    else
      doorStairBmOpenedAt = 0;

    ssDoorStairBm = state;
  }

  state = digitalRead(PIN_SS_DOOR_BASEMENT);
  if (state != ssDoorBasement){
#ifdef ENABLE_CAYENNE
    writeCayenneDigitalState(CH_DOOR_BASEMENT, state);
    writeCayenneDigitalState(CH_LIGHT_STAIR_BASEMENT, state);
#endif
    if(state)
      doorBasementOpenedAt = millis();
    else
      doorBasementOpenedAt = 0;

    ssDoorBasement = state;
  }

  state = !digitalRead(PIN_SS_LIGHT_BASEMENT);
  if (state != ssLightBasementOn){
#ifdef ENABLE_CAYENNE
    writeCayenneDigitalState(CH_LIGHT_BASEMENT, state);
#endif
    ssLightBasementOn = state;
  }

  state = digitalRead(PIN_SS_ENTRANCE_MOTION);
  if (state != ssEntranceMotion){
#ifdef ENABLE_CAYENNE
    writeCayenneDigitalState(CH_ENTRANCE_MOTION, state);
#endif
    if(state)
      entranceMotionDetectedAt = millis();
    else
      entranceMotionDetectedAt = 0;

    ssEntranceMotion = state;
  }

  ssDoorDetectors = (ssDoorBasement << 1) | (ssDoorStairBm << 1) | (ssDoorMain << 0);

  //ssWaterLeak = 0; // digitalRead(PIN_SS_WATER_SMOKE_BASEMENT);
  state = digitalRead(PIN_SS_WATER_SMOKE_BASEMENT);
  if (state != ssWaterLeak){
#ifdef ENABLE_CAYENNE
    writeCayenneDigitalState(CH_WATER_SMOKE_BASEMENT, state);
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
