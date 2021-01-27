// https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
// IMPORTANT NOTE: Remove PIN_SS_DOOR_MAIN connector b4 uploading code to NodeMcu, uploading fail otherwise

// inputs D1, D2, D5, D6, D7
#define PIN_SS_SUPPLY_VOLT          27 // battery voltage

#define PIN_SS_DHT                  35 // DHT sensor pin
#define PIN_SS_TEMP                 35 // temperature sensor pin
#define PIN_SS_DOOR_MAIN            34 // = RX, try use this as DI, together w.   Serial.begin(19200, SERIAL_8N1, SERIAL_TX_ONLY);

#define PIN_SS_DOOR_TO_BASEMENT     39 // No. 1, door to go down to basement and door to renting area
#define PIN_SS_DOOR_BASEMENT        36 
#define PIN_SS_ENTRANCE_MOTION      33 // No. 2, motion sensor for entrance

#define PIN_LIGHT_BASEMENT          15
#define PIN_SS_WATER_SMOKE_BASEMENT 32 // smoke, water leak


// outputs
#define PIN_LED                     2  // D4: same as built in LED GPIO2
#define PIN_TONE_MELODY             4

#define PIN_AC_POWER_LED_ENTRANCE   21 // No. 1, power for entrance led
//#define PIN_AC_POWER_RADIO          18 // OK as output, not possible to connect to OPTO input: connected to FLASH button, boot fails if pulled LOW


#define FIELD_ID_POWER_CAM  8
#define FIELD_ID_POWER_LOAD 9
