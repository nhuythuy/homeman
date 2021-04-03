// May 14 2020
// Author: Thuy Nguyen

// Ref.:
// Website: www.arduinesp.com
// https://learn.adafruit.com/dht/using-a-dhtxx-sensor


#include "sensors.h"
#include "actuators.h"
#include "datetime.h"
#include "wifi_cloud.h"
#include "pin_define.h"
#include "melody.h"
#include "cayenne.h"
#include "blynk.h"
#include "udp_broadcast.h"
#include "ota.h"
#include <esp_task_wdt.h>


#define WDT_TIMEOUT 15    // 15 seconds

// =======================================================
void setup() {
  setupSensors();
//  setupActuators();

  Serial.begin(115200); // ESP8266: Serial.begin(19200, SERIAL_8N1, SERIAL_TX_ONLY);
#ifdef ENABLE_BLUETOOTH
  SerialBT.begin("HomeMan");
#endif
  delay(100);

  setupOTA();
#ifdef ENABLE_WIFI
  WIFI_Connect();
//  setupDateTime();

#ifdef ENABLE_CAYENNE
  cayenneSetup();
#endif
#ifdef ENABLE_BLYNK
  blynkSetup();
#endif
#endif

  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
}

unsigned long previousMillis = millis();
unsigned long currentMillis = millis();
unsigned long currentSeconds = millis();

// =======================================================
void loop() {
  loopOTA();
  yield();
  esp_task_wdt_reset();

  currentMillis = millis();
  currentSeconds = currentMillis / 1000;
  runtimeMinutes = currentMillis / 60000;
  if(abs(currentMillis - previousMillis) > 2000){   // sampling sensors every 2 sec
    previousMillis = currentMillis;                 // save the last time  

  Serial.println("Runtime (min): " + String(runtimeMinutes));
#ifdef ENABLE_WIFI
#ifdef ENABLE_CAYENNE
    cayenneLoop();
#endif
#ifdef ENABLE_BLYNK
    blynkLoop();  
#endif

    wifiCheckReconnect();
//    getServerTime();
  systemHourMinute = (float)currentSeconds / 100.0;

#ifdef ENABLE_UDP_DEBUG
    if(enableUdpDebug && (currentSeconds % 5 == 0)) // every 5 min
      sendBroadcast();
#endif
#endif

    updateSlowAnalogSensors();
    updateDurations();
  }

  flipLed();
  updateDigitalSensors();
//  updateActuators();

#ifdef ENABLE_BLUETOOTH
  if(enableBluetoothDebug && (currentSeconds % 5 == 0))
    printDebugSerialBT();
#endif

}

// =======================================================
ICACHE_RAM_ATTR void detectsMovement() {
  Serial.println("MOTION DETECTED!!!");
  yield();
  if(ssBatteryVolt > 12.50){
    //digitalWrite(PIN_AC_POWER_LED_ENTRANCE, HIGH);
    acEntranceLed = true;
    acActuators |= (1 << 0);

    startMotionTimer = true;
    Serial.println("Light entrance: ON");
    lastTrigger = millis();
  }
}
