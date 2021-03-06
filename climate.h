#include <Wire.h>            // include Arduino Wire library (required for I2C devices)
#include <DHT.h>


#ifndef CLIMATE
#define CLIMATE

// A2, A1 & A0 connected to GND --> 1001000 = 0x48
#define DS1621_ADDRESS  0x48

DHT dht(PIN_SS_DHT, DHT11, 15);


void ds1621Setup(){
  Wire.begin();           // join i2c bus
  // initialize DS1621 sensor
  Wire.beginTransmission(DS1621_ADDRESS); // connect to DS1621 (send DS1621 address)
  Wire.write(0xAC);                       // send configuration register address (Access Config)
  Wire.write(0);                          // perform continuous conversion
  Wire.beginTransmission(DS1621_ADDRESS); // send repeated start condition
  Wire.write(0xEE);                       // send start temperature conversion command
  Wire.endTransmission();                 // stop transmission and release the I2C bus
}

void setupClimateSensors(){
  dht.begin();

//  ds1621Setup();
}

float ds1621GetTemperature() {
  Wire.beginTransmission(DS1621_ADDRESS); // connect to DS1621 (send DS1621 address)
  Wire.write(0xAA);                       // read temperature command
  Wire.endTransmission(false);            // send repeated start condition
  Wire.requestFrom(DS1621_ADDRESS, 2);    // request 2 bytes from DS1621 and release I2C bus at end of reading
  uint8_t t_msb = Wire.read();            // read temperature MSB register
  uint8_t t_lsb = Wire.read();            // read temperature LSB register
    
  // calculate full temperature (raw value)
  int16_t raw_t = (int8_t)t_msb << 1 | t_lsb >> 7;
  // convert raw temperature value to tenths °C then to temp.
  return (raw_t * 10 / 2.0) / 10;
}

bool updateBasementClimate(){
  bmTemp = dht.readTemperature();
  bmHumidity = dht.readHumidity();
  if (isnan(bmHumidity) || isnan(bmTemp)) {
    Serial.println("Failed to read from DHT sensor!");

    bmTemp = -100;
    bmHumidity = -100;
    return false;
  }

  Serial.println("Basement Temp.: " + String(bmTemp, 1) + " degC, Humidity.: " + String(bmHumidity, 1) + " %");
  return true;
}


#endif
