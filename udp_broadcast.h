#include <WiFi.h>
#include "global_vars.h"
#include "auth_settings.h"
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#ifndef UDP_BROADCAST
#define UDP_BROADCAST

WiFiUDP udp;

#define BROADCAST_IP    "192.168.1.255"
#define BROADCAST_PORT  45678

#define BUFFER_LENGTH   200
byte buffJson[BUFFER_LENGTH];


int buildMessage(unsigned long rt){
  // https://arduinojson.org/v6/example/

  DynamicJsonDocument doc(256);
  doc["node"] = "homeman";
  doc["runtime"] = rt;
  doc["battvolt"] = String(ssBatteryVolt, 2);
  doc["temp"] = String(bmTemp, 2);
  doc["humidity"] = String(bmHumidity, 2);
  doc["ssDoorMain"] = ssDoorMain;
  doc["doorMainOpenedMinutes"] = doorMainOpenedMinutes;
  doc["ssDoorToBasement"] = ssDoorToBasement;
  doc["doorToBasementOpenedMinutes"] = doorToBasementOpenedMinutes;
  doc["ssDoorBasement"] = ssDoorBasement;
  doc["doorBasementOpenedMinutes"] = doorBasementOpenedMinutes;
  doc["ssLightBasementOn"] = ssLightBasementOn;
  doc["ssEntranceMotion"] = ssEntranceMotion;
  doc["entranceMotionSeconds"] = entranceMotionSeconds;
  doc["ssWaterLeak"] = ssWaterLeak;
//  doc["acActuators"] = acActuators;

  memset(buffJson, 0x00, BUFFER_LENGTH);
  return serializeJson(doc, buffJson, BUFFER_LENGTH);
}

void sendBroadcast(unsigned long rt){
  int len = buildMessage(rt);

  udp.beginPacket(BROADCAST_IP, BROADCAST_PORT);
  udp.write(buffJson, len);
  udp.endPacket();

  Serial.println("Broadcast sent: " + String(len));
}

#endif
