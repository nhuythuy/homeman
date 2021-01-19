#include "global_vars.h"
#include <CayenneMQTTESP8266.h>

// Name convention: Area_SensorType_Time_Type_ID, ex.: DOOR_MAIN_MINUTES_OPENED, LR_HUMIDITY

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char dv_username[] = "3541f5b0-d9b3-11ea-883c-638d8ce4c23d";
char dv_password[] = "0573b7bfc25b7afb4042b3bb93ed8f16a6dd6fc2";
char dv_clientID[] = "d175a430-d9b4-11ea-b767-3f1a8f1211ba";


#define CH_BATT_VOLTAGE                 1
#define CH_MINUTES_DOOR_MAIN_OPENED     2
#define CH_MINUTES_DOOR_BASEMENT_OPENED 3
#define CH_BM_TEMPERATURE               4 // basement
#define CH_BM_HUMIDITY                  5
#define CH_RUNTIME                      6
#define CH_SECONDS_MOTION_DETECTED      7
#define CH_MINUTES_DOOR_BACK_OPENED     8
#define CH_RUNTIME_LIVING_ROOM          9

#define CH_LR_TEMPERATURE               101 // living room
#define CH_LR_HUMIDITY                  102

// digital states
#define CH_DOOR_MAIN            10
#define CH_DOOR_BASEMENT        11
#define CH_DOOR_BACK            12
#define CH_MOTION_ENTRANCE      15
#define CH_LIGHT_BASEMENT       16
#define CH_LIGHT_STAIR_BASEMENT 17
#define CH_LIGHT_ENTRANCE       18
#define CH_POWER_RADIO          19
#define CH_WATER_LEAK_0         20
#define CH_WATER_LEAK_1         21

#define CH_FORCE_RADIO_POWER    200


int cayenneCounter = 0;

// This function is called at intervals to send sensor data to Cayenne.
CAYENNE_OUT(CH_BATT_VOLTAGE){
  delay(1000);
  Cayenne.virtualWrite(CH_BATT_VOLTAGE, ssBatteryVolt, "batt", "V");
}

CAYENNE_OUT(CH_MINUTES_DOOR_MAIN_OPENED){
  delay(1000);
  Cayenne.virtualWrite(CH_MINUTES_DOOR_MAIN_OPENED, minutesDoorMainOpened, "counter");
}

CAYENNE_OUT(CH_MINUTES_DOOR_BACK_OPENED){
  delay(1000);
  Cayenne.virtualWrite(CH_MINUTES_DOOR_BACK_OPENED, minutesDoorBackOpened, "counter");
}

CAYENNE_OUT(CH_MINUTES_DOOR_BASEMENT_OPENED){
  delay(1000);
  Cayenne.virtualWrite(CH_MINUTES_DOOR_BASEMENT_OPENED, minutesDoorBasementOpened, "counter");
}

CAYENNE_OUT(CH_RUNTIME){
  delay(1000);
  Cayenne.virtualWrite(CH_RUNTIME, runtimeMinutes, "counter");
}

CAYENNE_OUT(CH_RUNTIME_LIVING_ROOM){
  delay(1000);
  Cayenne.virtualWrite(CH_RUNTIME_LIVING_ROOM, runtimeMinutesLivingRoom, "counter");
}

CAYENNE_OUT(CH_SECONDS_MOTION_DETECTED){
  delay(1000);
  Cayenne.virtualWrite(CH_SECONDS_MOTION_DETECTED, motionSeconds, "counter");
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
