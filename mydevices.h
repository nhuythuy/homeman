#include "global_vars.h"
#include <CayenneMQTTESP8266.h>

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char dv_username[] = "3541f5b0-d9b3-11ea-883c-638d8ce4c23d";
char dv_password[] = "0573b7bfc25b7afb4042b3bb93ed8f16a6dd6fc2";
char dv_clientID[] = "d175a430-d9b4-11ea-b767-3f1a8f1211ba";


#define CH_BATT_VOLTAGE   0
#define CH_DOORS          1
#define CH_OTHER_SENSORS  2
#define CH_ACTUATORS      3
#define CH_TEMPERATURE    4
#define CH_HUMIDITY       5

#define CH_DOOR_MAIN        10
#define CH_DOOR_BASEMENT    11
#define CH_DOOR_BACK        12
#define CH_MOTION_ENTRANCE  15
#define CH_LIGHT_BASEMENT   16
#define CH_WATER_LEAK_0     17
#define CH_WATER_LEAK_1     18


// This function is called at intervals to send sensor data to Cayenne.
CAYENNE_OUT(CH_BATT_VOLTAGE){
  Cayenne.virtualWrite(CH_BATT_VOLTAGE, ssBatteryVolt, "batt", "V");
}

CAYENNE_OUT(CH_DOORS){
  Cayenne.virtualWrite(CH_DOORS, ssDoorDetectors, "counter");
}

CAYENNE_OUT(CH_OTHER_SENSORS){
  Cayenne.virtualWrite(CH_OTHER_SENSORS, ssOtherSensors, "counter");
}

CAYENNE_OUT(CH_ACTUATORS){
  Cayenne.virtualWrite(CH_ACTUATORS, acActuators, "counter");
}

CAYENNE_OUT(CH_TEMPERATURE){
  Cayenne.celsiusWrite(CH_TEMPERATURE, temp);
}

CAYENNE_OUT(CH_HUMIDITY){
  Cayenne.virtualWrite(CH_HUMIDITY, humidity, "rel_hum", "p");
}

void writeCayenneDigitalStates(int channelId, int value){
  Cayenne.virtualWrite(channelId, value, "digital_sensor", "d");
}
