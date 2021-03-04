#include "wifi_cloud.h"
#include <CayenneMQTTESP32.h>


// Name convention: Area_SensorType_Time_Type, ex.: DOOR_MAIN_MINUTES_OPENED, LR_HUMIDITY

#ifndef MY_DEVICES
#define MY_DEVICES

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char dvUsername[] = CAYENNE_USERNAME;
char dvPassword[] = CAYENNE_PASSWORD;
char dvClientID[] = CAYENNE_CLIENT_ID;

#define MESSAGE_DELAY                       100 // millisec
#define MAX_CAYENNE_SIGNAL                  21 // find the last index and update here

#define CH_PS_BATT_VOLTAGE                  0
#define CH_BATT_VOLTAGE                     1
#define CH_BM_RUNTIME                       2 // basement node
#define CH_BM_TEMPERATURE                   3
#define CH_BM_HUMIDITY                      4
#define CH_ENTRANCE_MOTION_DETECTED_SECONDS 5
#define CH_DOOR_MAIN_OPENED_MINUTES         6
#define CH_DOOR_TO_BASEMENT_OPENED_MINUTES  7
#define CH_DOOR_BASEMENT_OPENED_MINUTES     8
#define CH_DOOR_BACK_OPENED_MINUTES         9

#define CH_BM_TEMPERATURE                   20
#define CH_BM_HUMIDITY                      21
#define CH_ST_TEMPERATURE                   22
#define CH_SR_TEMPERATURE                   23


// digital states
#define CH_DOOR_MAIN                51
#define CH_DOOR_TO_BASEMENT         52
#define CH_DOOR_BASEMENT            53
#define CH_DOOR_BACK                54
#define CH_ENTRANCE_MOTION          55
#define CH_LIGHT_BASEMENT           56
#define CH_LIGHT_STAIR_BASEMENT     57
#define CH_ENTRANCE_LIGHT           58
#define CH_POWER_RADIO              59
#define CH_WATER_SMOKE_BASEMENT     60
#define CH_WATER_LEAK_1             61

#define CH_FORCE_RADIO_POWER        100
#define CH_FORCE_CAMERA_POWER       101


void cayenneSetup(){
  Serial.println("Cayenne connecting...");
  Cayenne.begin(dvUsername, dvPassword, dvClientID, wifiSsid, wifiPassword);
  Serial.println("Cayenne connected!");
  delay(200);
}

void writeCayenneDigitalState(int channelId, int value){
  delay(MESSAGE_DELAY);
  Cayenne.virtualWrite(channelId, value, "digital_sensor", "d");
}

CAYENNE_OUT_DEFAULT(){
  //blynkReconnect();

    Cayenne.virtualWrite(CH_PS_BATT_VOLTAGE, ssBatteryVoltPS, "batt", "V");
    Cayenne.virtualWrite(CH_BATT_VOLTAGE, ssBatteryVolt, "batt", "V");
    Cayenne.virtualWrite(CH_DOOR_MAIN_OPENED_MINUTES, doorMainOpenedMinutes, "counter");
//    Cayenne.virtualWrite(CH_DOOR_BACK_OPENED_MINUTES, doorBackOpenedMinutes, "counter");
    Cayenne.virtualWrite(CH_DOOR_TO_BASEMENT_OPENED_MINUTES, doorToBasementOpenedMinutes, "counter");
    Cayenne.virtualWrite(CH_DOOR_BASEMENT_OPENED_MINUTES, doorBasementOpenedMinutes, "counter");
    Cayenne.virtualWrite(CH_BM_RUNTIME, bmRuntimeMinutes, "counter");
    Cayenne.virtualWrite(CH_ENTRANCE_MOTION_DETECTED_SECONDS, entranceMotionSeconds, "counter");
    Cayenne.celsiusWrite(CH_BM_TEMPERATURE, bmTemp);
    Cayenne.virtualWrite(CH_BM_HUMIDITY, bmHumidity, "rel_hum", "p");
    Cayenne.celsiusWrite(CH_ST_TEMPERATURE, stTemp);
    Cayenne.celsiusWrite(CH_SR_TEMPERATURE, srTemp);

  // digital signals
  writeCayenneDigitalState(CH_DOOR_MAIN, ssDoorMain);
  writeCayenneDigitalState(CH_DOOR_TO_BASEMENT, ssDoorToBasement);
  writeCayenneDigitalState(CH_DOOR_BASEMENT, ssDoorBasement);
//  writeCayenneDigitalState(CH_DOOR_BACK, ssDoorBack);
  writeCayenneDigitalState(CH_ENTRANCE_MOTION, ssEntranceMotion);
  writeCayenneDigitalState(CH_LIGHT_BASEMENT, ssLightBasementOn);
  writeCayenneDigitalState(CH_LIGHT_STAIR_BASEMENT, ssDoorToBasement);

}


CAYENNE_IN(CH_FORCE_RADIO_POWER)
{
  forceRadioPower = getValue.asInt(); // Get value as integer
//  Serial.println("Radio power force: " + String(forceRadioPower));
}


#endif
