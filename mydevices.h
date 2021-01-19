#include "global_vars.h"
#include <CayenneMQTTESP8266.h>

// Name convention: Area_SensorType_Time_Type, ex.: DOOR_MAIN_MINUTES_OPENED, LR_HUMIDITY

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char dvUsername[] = "3541f5b0-d9b3-11ea-883c-638d8ce4c23d";
char dvPassword[] = "0573b7bfc25b7afb4042b3bb93ed8f16a6dd6fc2";
char dvClientID[] = "10df0a00-5a78-11eb-883c-638d8ce4c23d";


#define CH_BATT_VOLTAGE                     1
#define CH_BM_RUNTIME                       2   // basement node
#define CH_BM_TEMPERATURE                   3
#define CH_BM_HUMIDITY                      4
#define CH_MOTION_ENTRANCE_DETECTED_SECONDS 5
#define CH_DOOR_MAIN_OPENED_MINUTES         6
#define CH_DOOR_BASEMENT_OPENED_MINUTES     7
#define CH_DOOR_BACK_OPENED_MINUTES         8

#define CH_LR_RUNTIME                       11   // living room node
#define CH_LR_TEMPERATURE                   12
#define CH_LR_HUMIDITY                      13

// digital states
#define CH_DOOR_MAIN                51
#define CH_DOOR_BASEMENT            52
#define CH_DOOR_BACK                53
#define CH_MOTION_ENTRANCE          54
#define CH_LIGHT_BASEMENT           55
#define CH_LIGHT_STAIR_BASEMENT     56
#define CH_LIGHT_ENTRANCE           57
#define CH_POWER_RADIO              58
#define CH_WATER_LEAK_0             59
#define CH_WATER_LEAK_1             60

#define CH_FORCE_RADIO_POWER        100


int cayenneCounter = 0;

// This function is called at intervals to send sensor data to Cayenne.
CAYENNE_OUT(CH_BATT_VOLTAGE){
  delay(1000);
  Cayenne.virtualWrite(CH_BATT_VOLTAGE, ssBatteryVolt, "batt", "V");
}

CAYENNE_OUT(CH_DOOR_MAIN_OPENED_MINUTES){
  delay(1000);
  Cayenne.virtualWrite(CH_DOOR_MAIN_OPENED_MINUTES, doorMainOpenedMinutes, "counter");
}

CAYENNE_OUT(CH_DOOR_BACK_OPENED_MINUTES){
  delay(1000);
  Cayenne.virtualWrite(CH_DOOR_BACK_OPENED_MINUTES, doorBackOpenedMinutes, "counter");
}

CAYENNE_OUT(CH_DOOR_BASEMENT_OPENED_MINUTES){
  delay(1000);
  Cayenne.virtualWrite(CH_DOOR_BASEMENT_OPENED_MINUTES, doorBasementOpenedMinutes, "counter");
}

CAYENNE_OUT(CH_BM_RUNTIME){
  delay(1000);
  Cayenne.virtualWrite(CH_BM_RUNTIME, bmRuntimeMinutes, "counter");
}

CAYENNE_OUT(CH_LR_RUNTIME){
  delay(1000);
  Cayenne.virtualWrite(CH_LR_RUNTIME, lrRuntimeMinutes, "counter");
}

CAYENNE_OUT(CH_MOTION_ENTRANCE_DETECTED_SECONDS){
  delay(1000);
  Cayenne.virtualWrite(CH_MOTION_ENTRANCE_DETECTED_SECONDS, entranceMotionSeconds, "counter");
}

CAYENNE_OUT(CH_BM_TEMPERATURE){
  delay(1000);
  Cayenne.celsiusWrite(CH_BM_TEMPERATURE, bmTemp);
}

CAYENNE_OUT(CH_BM_HUMIDITY){
  delay(1000);
  Cayenne.virtualWrite(CH_BM_HUMIDITY, bmHumidity, "rel_hum", "p");
}

CAYENNE_OUT(CH_LR_TEMPERATURE){
  delay(1000);
  Cayenne.celsiusWrite(CH_LR_TEMPERATURE, lrTemp);
}

CAYENNE_OUT(CH_LR_HUMIDITY){
  delay(1000);
  Cayenne.virtualWrite(CH_LR_HUMIDITY, lrHumidity, "rel_hum", "p");
}

void writeCayenneDigitalStates(int channelId, int value){
  delay(1000);
  Cayenne.virtualWrite(channelId, value, "digital_sensor", "d");
}

CAYENNE_IN(CH_FORCE_RADIO_POWER)
{
  forceRadioPower = getValue.asInt(); // Get value as integer
  Serial.println("Radio power force: " + String(forceRadioPower));
}
