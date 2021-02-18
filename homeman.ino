// May 14 2020
// Author: Thuy Nguyen

// Ref.:
// Website: www.arduinesp.com
// https://learn.adafruit.com/dht/using-a-dhtxx-sensor

#define ENABLE_WIFI
#define ENABLE_BLYNK
#define ENABLE_CAYENNE

#include <esp_task_wdt.h>
#include "sensors.h"
#include "actuators.h"
#include "datetime.h"
#include "wifi_cloud.h"
#include "pin_define.h"
#include "melody.h"
#include "cayenne.h"
#include "blynk.h"

#define WDT_TIMEOUT   120 // 120 sec

// =======================================================
void setup() {
  setupSensors();
  setupActuators();

  Serial.begin(19200); // ESP8266: Serial.begin(19200, SERIAL_8N1, SERIAL_TX_ONLY);
  SerialBT.begin("HomeMan");
  delay(100);

#ifdef ENABLE_WIFI
  WIFI_Connect();
  setupDateTime();

#ifdef ENABLE_CAYENNE
  cayenneSetup();
#endif
#ifdef ENABLE_BLYNK
  blynkSetup();
#endif
#endif

  esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);               // add current thread to WDT watch
}

unsigned long previousMillis = millis();
unsigned long currentMillis = millis();
// =======================================================
void loop() {
  esp_task_wdt_reset();

  currentMillis = millis();
  bmRuntimeMinutes = currentMillis / 60000;
  if(abs(currentMillis - previousMillis) > 2000){  // sampling sensors every 2 sec
    previousMillis = currentMillis;             // save the last time  

#ifdef ENABLE_WIFI
  getServerTime();
#endif

    updateTemp();
    updateBattVolt();
    updateDurations();
  }

  flipLed();
  updateSensors();
  updateActuator();

#ifdef ENABLE_WIFI
#ifdef ENABLE_CAYENNE
  Cayenne.loop();
#endif
#ifdef ENABLE_BLYNK
  blynkLoop();
#endif
#endif

#ifdef ENABLE_WIFI
  if(WiFi.status() == WL_DISCONNECTED){
    Serial.println("WiFi connection lost! Reconnecting...");
    WiFi.disconnect();
    WIFI_Connect();    
  }
#endif
}

// =======================================================
ICACHE_RAM_ATTR void detectsMovement() {
  Serial.println("MOTION DETECTED!!!");

  if(ssBatteryVolt > 12.50){
    //digitalWrite(PIN_AC_POWER_LED_ENTRANCE, HIGH);
    acEntranceLed = true;
    acActuators |= (1 << 0);

    startMotionTimer = true;
    Serial.println("Light entrance: ON");
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_ENTRANCE_LIGHT, true);
#endif
    lastTrigger = millis();
  }
}
