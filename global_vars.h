#ifndef GLOBAL_VARS
#define GLOBAL_VARS

#include "BluetoothSerial.h"

#define DELAY_LONG        5000            // 5,0 seconds
#define DELAY_SHORT       1000            // 2,5 seconds


// Check if Bluetooth configs are enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

long delayMs = DELAY_SHORT;
bool stateLed = 0;

bool enableBluetoothDebug = false;

bool PowerLedState = false;
long PowerLedStartAt = 0;

long globalState = 0;
int debugCounter = 0;

// sensors
float srTemp = 0.0;     // stair
float stTemp = 0.0;     // storage temp. sensor using LM35

float bmHumidity = 0.0; // basement (rent out) humidity
float bmTemp = 0.0;     // basement temp.

bool switchToSolarPower = false;

float ssBatteryVoltPS = 0;
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
