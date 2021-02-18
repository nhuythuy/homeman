#include <WiFi.h>
#include "global_vars.h"
#include "auth_settings.h"

#ifndef WIFI_CLOUD
#define WIFI_CLOUD

const char* wifiSsid = WIFI_AP;
const char* wifiPassword = WIFI_PW;

int espResetCounter = 0;


void WIFI_Connect(){
  Serial.println();
  Serial.println("MAC: " + WiFi.macAddress()); Serial.println();
  Serial.println("Connecting to " + String(wifiSsid));

  WiFi.begin(wifiSsid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    flipLed();
    if(debugCounter++ > 80)
    {
      debugCounter = 0;
      espResetCounter++;
      Serial.println("!");
    }

    if(espResetCounter > 2){ // reset ESP since it cannot connect to wifi after many tries
      ESP.restart();
      espResetCounter = 0;
    }
  }

  delay(200);
  Serial.println();
  Serial.println("Connected to wifi");
  Serial.print("Status: ");   Serial.println(WiFi.status());    // Network parameters
  Serial.print("IP: ");       Serial.println(WiFi.localIP());
  Serial.print("Subnet: ");   Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");  Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: ");     Serial.println(WiFi.SSID());
  Serial.print("Signal: ");   Serial.println(WiFi.RSSI());
  Serial.println();
  delay(100);
}

#endif
