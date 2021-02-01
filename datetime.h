#include "global_vars.h"
#include <NTPClient.h>


#ifndef DATE_TIME
#define DATE_TIME

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.nist.gov");
char* DayOfWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setupDateTime(){
  timeClient.begin(); // Initialize a NTPClient to get time
// Set offset time in seconds to adjust for your timezone, ex.: GMT +1 = 3600, GMT +8 = 28800, GMT -1 = -3600, GMT 0 = 0
  timeClient.setTimeOffset(3600); // Norway GMT + 1
}


void getServerTime(){
  Serial.println();
  timeClient.update();
  currentDay = timeClient.getDay();
  currentHour = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  int seconds = timeClient.getSeconds();

  
  if((minutes % 1) == 0) // to send every 1 minutes
    needUploadCloud = true;
  else
  {
    cloudUploaded = false;
    needUploadCloud = false;
  }
  
  Serial.print("Epoch Time: " + String(timeClient.getEpochTime()) + " - " + timeClient.getFormattedTime());  
  Serial.println(" Today:  " + String(DayOfWeek[currentDay]) + " (" + String(currentDay) + "), hour: "
    + String(currentHour));

}

#endif
