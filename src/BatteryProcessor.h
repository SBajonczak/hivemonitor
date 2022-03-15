#ifndef BatteryProcessor_H
#define BatteryProcessor_H
#include <Arduino.h>
#define BATT_WARNING_VOLTAGE 3.2  // Voltage for Low-Bat warning
/*
  * Scale should be calibrated with a regulated input of 3.3V!
  * Every change during runtime will cause wrong readings
  */
const float LOW_BATTERY = 1.1;

class BatteryProcessor
{
public:
  BatteryProcessor();
  void setup();
  float getVolt();
  bool IsLowWarning();
  bool IsLow();

private:
  float adjust;
  float lastvoltage;
};

#endif