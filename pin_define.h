#include <ESP8266WiFi.h>
// inputs
#define PIN_SS_ANALOG               A0

#define PIN_SS_DHT                  D5 // DHT sensor pin
#define PIN_SS_DOOR_MAIN            D6 // main door sensor

#define PIN_SS_DOOR_BASEMENT        D3 // No. 1, door to go down to basement and door to renting area
#define PIN_SS_ENTRANCE_MOTION      D2 // No. 2, motion sensor for entrance

#define PIN_SS_WATER_SMOKE_BASEMENT D7 // smoke, water leak


// outputs
#define PIN_LED                     D4 // D4: same as built in LED GPIO2
#define PIN_TONE_MELODY             D8

#define PIN_AC_POWER_LED_ENTRANCE   D0 // No. 1, power for entrance led
#define PIN_AC_POWER_CAMERA         D1 // No. 2, camera power

#define PIN_AC_BUZZER               D10

#define TRIGGER_PIN                 D9
#define ECHO_PIN                    A0
#define MAX_DISTANCE 200


#define FIELD_ID_POWER_CAM  8
#define FIELD_ID_POWER_LOAD 9
