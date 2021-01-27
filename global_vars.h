#ifndef GLOBAL_VARS
#define GLOBAL_VARS

long globalState = 0;
int debugCounter = 0;
int bmHeartbeat = 0;

// sensors
float bmHumidity = 0.0;
float bmTemp = 0.0;

float lrHumidity = 0.0;
float lrTemp = 0.0;

int ssBatteryVoltRaw = 0;
float ssBatteryVolt = 0;
bool ssDoorMain = 0;
bool ssDoorToBasement = 0;
bool ssDoorBasement = 0;
bool ssDoorBack = 0;
bool ssEntranceMotion = 0;
bool ssLightBasementOn = 0;

int ssWaterLeak = 0;

int ssDoorDetectors = 0;
int ssOtherSensors = 0;

// actuators
bool acEntranceLed = 0;
bool acBuzzer = 0;
int acActuators = 0;

bool forceCamPower = 0;
bool forceRadioPower = 0;

int bmRuntimeMinutes = 0;
int lrRuntimeMinutes = 0;
int doorMainOpenedAt = 0;
int doorMainOpenedMinutes = 0;
int doorToBasementOpenedAt = 0;
int doorToBasementOpenedMinutes = 0;
int doorBasementOpenedAt = 0;
int doorBasementOpenedMinutes = 0;
int doorBackOpenedMinutes = 0;

int playMelodyCounter = 0;
int currentHours = -1;

int entranceMotionDetectedAt = 0;
int entranceMotionSeconds = 0;

#endif
