#include <ArduinoJson.h>
#include "global_vars.h"


IPAddress serverPowerStation(192,168,1,6);          // the fix IP address of the server


void CommServerPowerStation(){
  if(!clientHome.connect(serverPowerStation, 80)){
    Serial.println("Connecting to server (power station) FAILED!");
    return;
  }

  flipLed();
  Serial.println("Connected to server (power station)!");

  clientHome.println("powerstationstate:" + String(bmRuntimeMinutes) + "\n");
  String reply = clientHome.readStringUntil('\n');   // receives the answer from the sever
  Serial.println("from server (power station): " + reply);
  if(reply.length() < 10){
    clientHome.flush();
    return;
  }
  
  DynamicJsonDocument doc(256);
  deserializeJson(doc, reply);
  String node = doc["node"];
  psRuntimeMinutes = doc["runtime"];
  String battvolt = doc["battvolt"];
  psBatteryVolt = battvolt.toFloat();
  String tmp = doc["temp"];
  psTemp = tmp.toFloat();
  String humid = doc["humidity"];
  lrHumidity = humid.toFloat();

  Serial.println("from server (power station): Runtime (" + String(psRuntimeMinutes)
  + "), Temp: (" + String(psTemp)
  + "), Humidity: (" + String(psHumidity)
  + "), Batt. volt.: (" + String(psBatteryVolt) + ")");

  clientHome.flush();
  flipLed();
}
