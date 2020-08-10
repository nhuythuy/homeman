#ifndef GLOBAL_VARS
#define GLOBAL_VARS

long globalState = 0;
int debugCounter = 0;

// sensors
float humidity = 0.0;
float temp = 0.0;

int ssBatteryVoltRaw = 0;
float ssBatteryVolt = 0;
bool ssDoorMain = 0;
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
float camPower = 0;

#endif
