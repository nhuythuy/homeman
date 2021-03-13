#include <WiFi.h>
#include "global_vars.h"
#include "auth_settings.h"
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#ifndef UDP_BROADCAST
#define UDP_BROADCAST

WiFiUDP udp;

#define BROADCAST_IP    "192.168.1.255"
#define BROADCAST_PORT  5678

#define BUFFER_LENGTH   200

byte buffJson[BUFFER_LENGTH];
String sJson;

int findLength(){
  for (int i = 0; i < BUFFER_LENGTH; i++){
     if (buffJson[i] == 0x00)
       return i;
  }
}

void buildMessage(){
  // https://arduinojson.org/v6/example/

  DynamicJsonDocument doc(256);
  doc["node"] = "homeman";
  doc["runtime"] = runtimeMinutes;
  doc["battvolt"] = String(ssBatteryVolt, 2);
  doc["temp"] = String(bmTemp, 2);
  doc["humidity"] = String(bmHumidity, 2);
  doc["ssDoorMain"] = ssDoorMain;
  doc["doorMainOpenedMinutes"] = doorMainOpenedMinutes;
  doc["ssDoorBasement"] = ssDoorBasement;
  doc["doorBasementOpenedMinutes"] = doorBasementOpenedMinutes;
  doc["ssLightBasementOn"] = ssLightBasementOn;
  doc["ssEntranceMotion"] = ssEntranceMotion;
  doc["entranceMotionSeconds"] = entranceMotionSeconds;
  doc["ssWaterLeak"] = ssWaterLeak;
  doc["acActuators"] = acActuators;

  serializeJson(doc, sJson);
}

void sendBroadcast(){
  buildMessage();
  int len = sJson.length();
  memset(buffJson, 0x00, sizeof(buffJson));

  byte plain[len];
  sJson.getBytes(plain, sJson.length());

  udp.beginPacket(BROADCAST_IP,BROADCAST_PORT);
  udp.write(plain, len);
  udp.endPacket();

  Serial.println("Broadcast sent: " + String(len));
  Serial.println("Json: " + sJson);
}

void broadcastStates(){
  //udp.broadcastTo(BROADCAST_IP, BROADCAST_PORT);  
}

#endif
