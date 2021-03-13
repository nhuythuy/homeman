#include <WiFi.h>
#include "global_vars.h"
#include "auth_settings.h"
#include <WiFiUdp.h>

#ifndef UDP_BROADCAST
#define UDP_BROADCAST

WiFiUDP udp;





void sendBroadcast()
  if(connected){
    //Send a packet
    udp.beginPacket(udpAddress,udpPort);
    udp.write(command, 27);
    udp.endPacket();
  }


#endif
