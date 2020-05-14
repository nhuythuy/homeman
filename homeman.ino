// www.arduinesp.com 
//
// Plot DTH11 data on thingspeak.com using an ESP8266 
// April 11 2015
// Author: Jeroen Beemster
// Website: www.arduinesp.com
 
// https://learn.adafruit.com/dht/using-a-dhtxx-sensor


#include <DHT.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "wifi_pw.h"
#include <WiFiUdp.h>

// replace with your channel's thingspeak API key, 
//const char* ssid = "matsuya"; //"VNNO";
//const char* password = "kamikaze123"; // "Planetvegen18A"; // WIFI_PW;
const char* ssid = "DNVGuest";
const char* password = WIFI_PW;

unsigned long THING_SPEAK_CHANNEL_NO = 447257;
String myWriteAPIKey = "59Y4RMCCJVKVWBOQ";
// to send data, using this get req: https://api.thingspeak.com/update?api_key=QFS00KA70KNC5NX6&field8=6


unsigned int localPort = 2390;      // local port to listen for UDP packets
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
WiFiUDP udp; // A UDP instance to let us send and receive packets over UDP


#define DELAY_LONG 10000    // 30 seconds
#define DELAY_SHORT 3000    // 3 seconds

String myReadAPIKey = "9L9ZWCW1QLN39E09";
const char* server = "api.thingspeak.com";

#define PIN_SS_ANALOG       A0

#define PIN_LED             D4 // same as built in LED GPIO2
#define PIN_WORKING_MODE    D0
#define PIN_SS_DHT          D1 // DHT sensor pin
#define PIN_SS_DOOR_MAIN    D2
#define PIN_SS_DOOR_BACK    D3

#define PIN_SS_DOOR_DOWN_BASEMENT   D5
#define PIN_SS_WATER_SMOKE_BASEMENT D6 // smoke, water leak

#define PIN_AC_POWER_LOAD   D8
#define PIN_AC_POWER_CAM    D9
#define PIN_AC_BUZZER       D10
#define FIELD_ID_POWER_CAM  8
#define FIELD_ID_POWER_LOAD 9

DHT dht(PIN_SS_DHT, DHT11,15);
WiFiClient client;



void setup() {                
  Serial.begin(115200);
  delay(10);
  dht.begin();

  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  int dotCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");

    if(dotCounter++ > 80)
    {
      dotCounter = 0;
      Serial.println("!");
    }
  }

  Serial.println(WiFi.localIP());
  Serial.println("WiFi connected");


  // internet time
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP);

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  }
else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);


    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if ( ((epoch % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ( (epoch % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
  }


  pinMode(PIN_WORKING_MODE, INPUT);

  pinMode(PIN_SS_DOOR_MAIN, INPUT);
//  pinMode(PIN_SS_DOOR_BACK, INPUT);
  pinMode(PIN_SS_DOOR_DOWN_BASEMENT, INPUT);
  pinMode(PIN_SS_WATER_SMOKE_BASEMENT, INPUT);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_AC_POWER_LOAD, OUTPUT);
  pinMode(PIN_AC_POWER_CAM, OUTPUT);


  ThingSpeak.begin(client);
}

long delayMs = DELAY_LONG;
// sensors
float humidity = 0.0;
float temp = 0.0;

int ssSmokeDetectors = 0;
int ssDoorDetectors = 0;
int ssWaterLeak = 0;

// actuators
bool acCamPower = 0;
bool acLoadPower = 0;
bool acBuzzer = 0;

bool forceCamPower = 0;
float camPower = 0;
float loadPower = 0;

void loop() {
  bool ret = updateHumidTempe();
  if(!ret){
    humidity = -100.0;
    temp = -100.0;
  }

  blinkLed();

  updateSensors();
  updateWorkingMode();
  updateActuator();
 
  if (client.connect(server,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = myWriteAPIKey;
           postStr +="&field1=";
           postStr += String(temp);
           postStr +="&field2=";
           postStr += String(humidity);
           postStr +="&field3=";
           postStr += String(ssSmokeDetectors);
           postStr +="&field4=";
           postStr += String(ssDoorDetectors);
           postStr +="&field5=";
           postStr += String(ssWaterLeak);
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
           
 
     Serial.print("Temperature: ");
     Serial.print(temp);
     Serial.print(" degrees Celcius Humidity: "); 
     Serial.print(humidity);
     Serial.println("% send to Thingspeak");    
  }
  client.stop();
   
  Serial.println("Waiting...");    
  // thingspeak needs minimum 15 sec delay between updates
//  updateTemperature();

  delay(delayMs);  
}

void blinkLed()
{
  digitalWrite(PIN_LED, false);
  delay(200);
  digitalWrite(PIN_LED, true);
}
bool updateHumidTempe(){
  humidity = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(humidity) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(delayMs);
    return false;
  }

  return true;
}

void updateWorkingMode(){
  bool workingMode = digitalRead(PIN_WORKING_MODE);
  if(workingMode)
    delayMs = DELAY_LONG;
  else
    delayMs = DELAY_SHORT;
}

void updateSensors(){
//  bool ssSmokeKichen = digitalRead(PIN_SS_SMOKE_KITCHEN);
//  bool ssSmokeLivingRoom = digitalRead(PIN_SS_SMOKE_LIVING);
//  bool ssSmokeFirstFloor = digitalRead(PIN_SS_SMOKE_1_FLOOR);

  bool ssDoorMain = digitalRead(PIN_SS_DOOR_MAIN);
  bool ssDoorBack = 0;//digitalRead(PIN_SS_DOOR_BACK);
  bool ssDoorDownBasement = digitalRead(PIN_SS_DOOR_DOWN_BASEMENT);

  bool ssWaterLeak = digitalRead(PIN_SS_WATER_SMOKE_BASEMENT);

//  ssSmokeDetectors = (ssSmokeKichen << 3) | (ssSmokeLivingRoom << 2) | (ssSmokeFirstFloor << 1);
  ssDoorDetectors = (ssDoorDownBasement << 2) | (ssDoorBack << 1) | (ssDoorMain << 0);

  Serial.print("Door sensors: = ");
  Serial.println(ssDoorDetectors);
  Serial.print("Smoke sensors: = ");
  Serial.println(ssSmokeDetectors);
  Serial.print("Water leak. sensors: = ");
  Serial.println(ssWaterLeak);

  if((ssSmokeDetectors > 0) || (ssDoorDetectors > 0))
    forceCamPower = 1;
  else
    forceCamPower = 0;
}

void updateActuator()
{
  camPower = ThingSpeak.readFloatField(THING_SPEAK_CHANNEL_NO, FIELD_ID_POWER_CAM);
    if(camPower < 1.0)
      digitalWrite(PIN_AC_POWER_CAM, 0 || forceCamPower);
    else
      digitalWrite(PIN_AC_POWER_CAM, 1);

  Serial.print("Cam power: = ");
  Serial.println(camPower);

    loadPower = ThingSpeak.readFloatField(THING_SPEAK_CHANNEL_NO, FIELD_ID_POWER_LOAD);
    if(loadPower < 1.0)
      digitalWrite(PIN_AC_POWER_LOAD, 0 || forceCamPower);
    else
      digitalWrite(PIN_AC_POWER_LOAD, 1);
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
