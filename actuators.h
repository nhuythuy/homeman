#include "lib.h"
#include "datetime.h"
#include "melody.h"


#define MOTION_DELAY      0*60*1000       // 1 mins delay


void setupActuators(){
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_AC_TONE_MELODY, OUTPUT);
  pinMode(PIN_AC_POWER_LED_ENTRANCE, OUTPUT);
  //pinMode(PIN_AC_POWER_RADIO, OUTPUT);
  pinMode(PIN_AC_SWITCH_POWER_RELAY, OUTPUT);
}

// In 3.0.0 there will be a getDay() function.
// It will return 0 - 6, from Sunday to Saturday.
void powerRadio(){
  if ((ssBatteryVolt > 13.0) // only if battery is full enough
    && (currentHour < 20)    // no later than 19:00
    && ((((currentDay == 0) || (currentDay == 6)) && (currentHour > 10)) // Sunday or Saturday
      || ((currentDay > 0) && (currentDay < 6) && (currentHour > 9)))){  // weekdays
    //digitalWrite(PIN_AC_POWER_RADIO, true);
    //Serial.println("Radio power: ON");
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_POWER_RADIO, true);
#endif
  }
  else{
    //digitalWrite(PIN_AC_POWER_RADIO, false);
    //Serial.println("Radio power: OFF");
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_POWER_RADIO, false);
#endif
  }
}

void powerSwitchToSolarPower(){
  if(ssBatteryVoltPS > 13.0){
    digitalWrite(PIN_AC_SWITCH_POWER_RELAY, HIGH);
    switchToSolarPower = true;
  }

  if(ssBatteryVoltPS < 12.5){
    digitalWrite(PIN_AC_SWITCH_POWER_RELAY, LOW);
    switchToSolarPower = false;
  }  
}

void updateActuator()
{
  timeNow = millis();
  // Turn off the LED after the number of seconds defined in the MOTION_DELAY variable
  if(startMotionTimer && (timeNow - lastTrigger > MOTION_DELAY)) {
    Serial.println(String(timeNow) + " - " + String(lastTrigger));
    Serial.println("Light stopped...");
    //digitalWrite(PIN_AC_POWER_LED_ENTRANCE, LOW);
    acEntranceLed = false;
    Serial.println("Light entrance: OFF");
#ifdef ENABLE_WIFI
    writeCayenneDigitalStates(CH_ENTRANCE_LIGHT, false);
#endif
    acActuators &= ~(1 << 0);
    startMotionTimer = false;
  }

  powerRadio();

  // play melody only twice if it happens during the sleeping time 22:00 to 8:00
  // can detect door to basement OPENED/CLOSED only if the solar charger giving power from the battery
  if((!ssDoorToBasement) && ssLightBasementOn && (ssBatteryVolt > 12.0)){
    if((currentHour > 8) && (currentHour < 22)){
      Serial.println("Playing melody...");
//      playMelody();
    }
    else{
      if (playMelodyCounter < 2){
//          playMelody();
          playMelodyCounter++;
      }
    }
  }
  else
    playMelodyCounter = 0;

}
