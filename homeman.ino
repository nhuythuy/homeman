
// Plot DTH11 data on thingspeak.com using an ESP8266
// May 14 2020
// Author: Thuy Nguyen, based on an examples from Jeroen Beemster reading DTH11 sensor

// Ref.:
// Website: www.arduinesp.com
// https://learn.adafruit.com/dht/using-a-dhtxx-sensor
// https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/


#include <DHT.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "wifi_pw.h"
#include "pin_define.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "melody.h"
#include <NewPing.h>



int globalError = 0;
int debugCounter = 0;

// replace with your channel's thingspeak API key, 
const char* ssid = "VNNO"; // "DNVGuest" "Thuy's iPhone"; "matsuya";
const char* password = WIFI_PW;

unsigned long THING_SPEAK_CHANNEL_NO = 447257;
String myWriteAPIKey = "59Y4RMCCJVKVWBOQ";
// to send data, using this get req: https://api.thingspeak.com/update?api_key=QFS00KA70KNC5NX6&field8=6

#define DELAY_LONG 5000    // 5,0 seconds
#define DELAY_SHORT 2500   // 2,5 seconds

String myReadAPIKey = "9L9ZWCW1QLN39E09";
const char* server = "api.thingspeak.com";

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

DHT dht(PIN_SS_DHT, DHT11,15);
WiFiClient client;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.nist.gov");

int minutes = 0; // use this for sending to update to send to thingspeak
bool needUpdateCloud = false;

long delayMs = DELAY_LONG;
// sensors
float humidity = 0.0;
float temp = 0.0;

bool ssDoorMain = 0;
bool ssDoorBasement = 0;
bool ssDoorBack = 0;
bool ssEntranceMotion = 0;

int ssWaterLeak = 0;

int ssDoorDetectors = 0;
int ssOtherSensors = 0;

// actuators
bool acCamPower = 0;
bool acBuzzer = 0;

bool forceCamPower = 0;
float camPower = 0;


void setup() {
  pinMode(PIN_SS_DOOR_MAIN, INPUT);
  pinMode(PIN_SS_DOOR_BASEMENT, INPUT);
  pinMode(PIN_SS_WATER_SMOKE_BASEMENT, INPUT);
  pinMode(PIN_SS_ENTRANCE_MOTION, INPUT);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_AC_BUZZER, OUTPUT);
  pinMode(PIN_AC_POWER_LED_ENTRANCE, OUTPUT);
  pinMode(PIN_AC_POWER_CAMERA, OUTPUT);

  Serial.begin(19200);
  delay(1000);
  dht.begin();

  Serial.println();
  Serial.println("MAC: " + WiFi.macAddress());
  Serial.println("Connecting to " + String(ssid));
  
  WiFi.begin(ssid, password);

  bool ledStatus = false;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    ledStatus = !ledStatus;
    digitalWrite(PIN_LED, !ledStatus);
    if(debugCounter++ > 80)
    {
      debugCounter = 0;
      Serial.println("!");
    }
  }

  Serial.println("WiFi connected, IP: " + WiFi.localIP());

  playMelody();

  timeClient.begin(); // Initialize a NTPClient to get time
// Set offset time in seconds to adjust for your timezone, ex.: GMT +1 = 3600, GMT +8 = 28800, GMT -1 = -3600, GMT 0 = 0
  timeClient.setTimeOffset(3600); // Norway GMT + 1

  ThingSpeak.begin(client);
}

void loop() {
  updateHumidTempe();

  getTime();
  blinkLed();
  updateSensors();
  updateActuator();

  if(needUpdateCloud == true)
    updateCloud();

  delayWithErrorCheck();
}

void updateDistanceSensor(){
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  Serial.println("Ping: " + String(sonar.convert_cm(uS)) + " cm");
}

void updateCloud(){
  if (client.connect(server,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = myWriteAPIKey;
           postStr +="&field1=" + String(temp);
           postStr +="&field2=" + String(humidity);
           postStr +="&field3=" + String(ssDoorDetectors);
           postStr +="&field4=" + String(ssOtherSensors);
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n"); 
     client.print("Host: api.thingspeak.com\n"); 
     client.print("Connection: close\n"); 
     client.print("X-THINGSPEAKAPIKEY: "+myWriteAPIKey+"\n"); 
     client.print("Content-Type: application/x-www-form-urlencoded\n"); 
     client.print("Content-Length: "); 
     client.print(postStr.length()); 
     client.print("\n\n"); 
     client.print(postStr);

     Serial.print("Temperature: " + String(temp) + " degrees Celcius, Humidity: " + String(humidity) + "% sent to Thingspeak");
  }
  client.stop();
   
  Serial.println("Waiting...");
  // thingspeak needs minimum 15 sec delay between updates
}

void blinkLed()
{
    Serial.print("-");
    if(debugCounter++ > 80)
    {
      debugCounter = 0;
      Serial.println("!");
    }

  digitalWrite(PIN_LED, false);
  delay(100);
  digitalWrite(PIN_LED, true);
  delay(100);
}


void getTime(){
  Serial.println();
  timeClient.update();
  minutes = timeClient.getMinutes();

  if((minutes % 20) == 0) // to send every 10 minutes
    needUpdateCloud = true;
  else
    needUpdateCloud = false;
  
  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);
  Serial.println();
}

bool updateHumidTempe(){
  humidity = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(humidity) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");

    delayWithErrorCheck();
    humidity = -100;
    temp = -100;
    return false;
  }

  return true;
}

void delayWithErrorCheck(){
    if(globalError > 0)
    blinkLed();
  else
    delay(delayMs);
}

void updateSensors(){
  ssDoorMain = digitalRead(PIN_SS_DOOR_MAIN);
  ssDoorBasement = digitalRead(PIN_SS_DOOR_BASEMENT);
  ssEntranceMotion = digitalRead(PIN_SS_ENTRANCE_MOTION);
  ssDoorDetectors = (ssEntranceMotion << 2) | (ssDoorBasement << 1) | (ssDoorMain << 0);

  ssWaterLeak = digitalRead(PIN_SS_WATER_SMOKE_BASEMENT);

  ssOtherSensors =  ssWaterLeak;

  int gbError = (ssOtherSensors << 8) | ssDoorDetectors;
  if(gbError != globalError) // send to cloud only if global error triggered
    needUpdateCloud = true;

  globalError = gbError;

  Serial.println();
  Serial.println("Door sensors: " + String(ssDoorDetectors, BIN));
  Serial.println("Others sensors: " + String(ssOtherSensors, BIN));
  Serial.println("Global error: " + String(globalError, BIN));
  Serial.println();

  if(ssDoorDetectors > 0)
    forceCamPower = 1;
  else
    forceCamPower = 0;
}

void updateActuator()
{
  digitalWrite(PIN_AC_POWER_LED_ENTRANCE, ssEntranceMotion);

  camPower = ThingSpeak.readFloatField(THING_SPEAK_CHANNEL_NO, FIELD_ID_POWER_CAM);
    if(camPower < 1.0)
      digitalWrite(PIN_AC_POWER_CAMERA, 0 || forceCamPower);
    else
      digitalWrite(PIN_AC_POWER_CAMERA, 1);

  Serial.println("Cloud CAM power: " + String(camPower) + " - Force CAM power: " + String(forceCamPower));
}
