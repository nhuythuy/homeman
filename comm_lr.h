#include <ArduinoJson.h>
#include "global_vars.h"


IPAddress serverLivingRoom(192,168,1,5);          // the fix IP address of the server

void CommServerLivingRoom(){
  if(!clientHome.connect(serverLivingRoom, 80)){
    Serial.println("Connecting to server (living room) FAILED!");
    return;
  }

  flipLed();
  Serial.println("Connected to server (living room)!");
//  clientHome.println("Hello Home server! Are you sleeping?\n");  // sends the message to the server
//  String answer = clientHome.readStringUntil('\n');   // receives the answer from the sever
//  Serial.println("from server (living room): " + answer);

  // send client state to the server
  // https://arduinojson.org/v6/example/

  clientHome.println("livingroomstate:" + String(bmRuntimeMinutes) + "\n");
  String reply = clientHome.readStringUntil('\n');    // receives the answer from the sever
  Serial.println("from server (living room): " + reply);
  if(reply.length() < 10){
    clientHome.flush();
    return;
  }
  
  DynamicJsonDocument doc(256);
  deserializeJson(doc, reply);
  String node = doc["node"];
  lrRuntimeMinutes = doc["runtime"];
  String tmp = doc["temp"];
  lrTemp = tmp.toFloat();
  String humid = doc["humidity"];
  lrHumidity = humid.toFloat();

  bool state = doc["ssDoorBack"];
  if (state != ssDoorBack){
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_DOOR_BACK, state);
#endif
    ssDoorBack = state;
  }

  doorBackOpenedMinutes = doc["ssDoorBackOpenMin"];

  Serial.println("from server (living room): Runtime (" + String(lrRuntimeMinutes)
  + "), Temp: (" + String(lrTemp)
  + "), Humidity: (" + String(lrHumidity)
  + "), Door back: (" + String(ssDoorBack)
  + "), Door back opened: (" + String(doorBackOpenedMinutes) + ") min");

  clientHome.flush();
  flipLed();
}
