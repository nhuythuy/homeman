#include "global_vars.h"
#include "pin_define.h"


#ifndef LIB
#define LIB


void flipLed(){
  stateLed = !stateLed;
  digitalWrite(PIN_LED, stateLed);
}

void blinkLed(){
  flipLed();
  flipLed();
}

void blinkPowerLed(){
  PowerLedDelay += 500;
  if(PowerLedDelay > 5000)
    PowerLedDelay = 1000;

  for(int i = 0; i < 20; i++){
    PowerLedState = !PowerLedState;
    digitalWrite(PIN_AC_POWER_LED_ENTRANCE, PowerLedState);
    delay(PowerLedDelay);
  }
}

#endif
