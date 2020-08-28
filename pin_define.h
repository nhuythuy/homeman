#include <ESP8266WiFi.h>

// https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
// IMPORTANT NOTE: Remove PIN_SS_DOOR_MAIN connector b4 uploading code to NodeMcu, uploading fail otherwise

// inputs D1, D2, D5, D6, D7
#define PIN_SS_SUPPLY_VOLT          A0 // battery voltage

#define PIN_SS_DHT                  D5 // DHT sensor pin
#define PIN_SS_DOOR_MAIN            D9 // = RX, try use this as DI, together w.   Serial.begin(19200, SERIAL_8N1, SERIAL_TX_ONLY);

#define PIN_SS_DOOR_BASEMENT        D6 // No. 1, door to go down to basement and door to renting area
#define PIN_SS_ENTRANCE_MOTION      D1 // No. 2, motion sensor for entrance

#define PIN_LIGHT_BASEMENT          D7
#define PIN_SS_WATER_SMOKE_BASEMENT D0 // smoke, water leak


// outputs
#define PIN_LED                     D4 // D4: same as built in LED GPIO2
#define PIN_TONE_MELODY             D8

#define PIN_AC_POWER_LED_ENTRANCE   D2 // No. 1, power for entrance led
//#define PIN_AC_POWER_RADIO          D3 // OK as output, not possible to connect to OPTO input: connected to FLASH button, boot fails if pulled LOW

// remaining pins: RX (in), D3

#define FIELD_ID_POWER_CAM  8
#define FIELD_ID_POWER_LOAD 9
