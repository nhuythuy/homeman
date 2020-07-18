#include <ESP8266WiFi.h>
// inputs D1, D2, D5, D6, D7
//#define PIN_SS_SUPPLY_VOLT        A0

#define PIN_SS_DHT                  D5 // DHT sensor pin
#define PIN_SS_DOOR_MAIN            D10 // main door sensor

#define PIN_SS_DOOR_BASEMENT        D6 // No. 1, door to go down to basement and door to renting area
#define PIN_SS_ENTRANCE_MOTION      D2 // No. 2, motion sensor for entrance

#define PIN_LIGHT_BASEMENT          D7
#define PIN_SS_WATER_SMOKE_BASEMENT D1 // smoke, water leak
#define PIN_SS_OTHER                RX // try use this as DI, together w.   Serial.begin(19200, SERIAL_8N1, SERIAL_TX_ONLY);


// outputs
#define PIN_LED                     D4 // D4: same as built in LED GPIO2
#define PIN_TONE_MELODY             D8

#define PIN_AC_POWER_LED_ENTRANCE   D0 // No. 1, power for entrance led
//#define PIN_AC_POWER_CAMERA         D3 // not possible to connect to OPTO in: connected to FLASH button, boot fails if pulled LOW


#define FIELD_ID_POWER_CAM  8
#define FIELD_ID_POWER_LOAD 9
