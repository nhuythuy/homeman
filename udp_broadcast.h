#include <WiFi.h>
#include "global_vars.h"
#include "auth_settings.h"
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#ifndef UDP_BROADCAST
#define UDP_BROADCAST

WiFiUDP udp;

#define UDP_BROADCAST_IP    "192.168.1.255"
#define UDP_BROADCAST_PORT  45678

#define UDP_BUFFER_LENGTH   200
byte buffJson[UDP_BUFFER_LENGTH];


int buildMessage(){
  // https://arduinojson.org/v6/example/

  yield();
  DynamicJsonDocument doc(256);
  doc["node"] = "homeman";
  doc["runtime"] = runtimeMinutes;
  doc["battvolt"] = ssBatteryVolt;
  doc["temp"] = bmTemp;
  yield();
  doc["humidity"] = bmHumidity;
  doc["ssDoorMain"] = ssDoorMain;
  doc["doorMainOpenedMinutes"] = doorMainOpenedMinutes;
  doc["ssDoorStairBm"] = ssDoorStairBm;
  doc["doorStairBmOpenedMinutes"] = doorStairBmOpenedMinutes;
//  doc["ssDoorBasement"] = ssDoorBasement;
//  doc["doorBasementOpenedMinutes"] = doorBasementOpenedMinutes;
//  doc["ssLightBasementOn"] = ssLightBasementOn;
//  doc["ssEntranceMotion"] = ssEntranceMotion;
//  doc["entranceMotionSeconds"] = entranceMotionSeconds;
//  doc["ssWaterLeak"] = ssWaterLeak;
//  doc["acActuators"] = acActuators;

  yield();
  memset(buffJson, 0x00, UDP_BUFFER_LENGTH);
  return serializeJson(doc, buffJson, UDP_BUFFER_LENGTH);
}

void sendBroadcast(){
  int len = buildMessage();

  yield();
  udp.beginPacket(UDP_BROADCAST_IP, UDP_BROADCAST_PORT);
  udp.write(buffJson, len);
  udp.endPacket();

  Serial.println("Broadcast sent: " + String(len));
}

#endif
