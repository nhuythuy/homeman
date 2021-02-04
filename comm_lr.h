#include <ArduinoJson.h>


IPAddress serverLivingRoom(192,168,1,5);          // the fix IP address of the server
WiFiClient clientHome;

void commServerSetup(){
  clientHome.setTimeout(3000); // msec
}

void CommServerLivingRoom(){
  if(!clientHome.connect(serverLivingRoom, 80)){
    Serial.println("Cannot connect to server (Living room)!");
    return;
  }

  digitalWrite(PIN_LED, LOW);       // to show the communication only (inverted logic)
  Serial.println("Connecting to server (Living room)");
//  clientHome.println("Hello Home server! Are you sleeping?\n");  // sends the message to the server
//  String answer = clientHome.readStringUntil('\n');   // receives the answer from the sever
//  Serial.println("from server (Living room): " + answer);

  // send client state to the server
  // https://arduinojson.org/v6/example/

  clientHome.println("livingroomstate:" + String(bmHeartbeat++) + "\n");
  String reply = clientHome.readStringUntil('\n');   // receives the answer from the sever
  Serial.println("from server (Living room): " + reply);
  if(reply.length() < 10){
    clientHome.flush();
    return;
  }
  
  DynamicJsonDocument doc(256);
  deserializeJson(doc, reply);
  String node = doc["node"];
  int heartbeat = doc["heartbeat"];
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

  Serial.println("from server (Living room): Runtime (" + String(lrRuntimeMinutes)
  + "), Temp: (" + String(lrTemp)
  + "), Humidity: (" + String(lrHumidity)
  + "), Door back: (" + String(ssDoorBack)
  + "), Door back opened: (" + String(doorBackOpenedMinutes) + ") min");

  clientHome.flush();
  digitalWrite(PIN_LED, HIGH);
}
