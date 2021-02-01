#ifndef GLOBAL_VARS
#define GLOBAL_VARS

#define DELAY_LONG        5000            // 5,0 seconds
#define DELAY_SHORT       2500            // 2,5 seconds

long delayMs = DELAY_SHORT;

bool PowerLedState = false;
int PowerLedDelay = 1000;

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

unsigned long timeNow = millis();
unsigned long lastTrigger = millis();
boolean startMotionTimer = false;

int playMelodyCounter = 0;
int currentHour = -1;
int currentDay = -1;

int entranceMotionDetectedAt = 0;
int entranceMotionSeconds = 0;

bool needUploadCloud = false;
bool cloudUploaded = false;

#endif
