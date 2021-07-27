/*
  StateManager.h - Helper Libary for managing the Homie states.
*/
#ifndef ConfigurationManager_h
#define ConfigurationManager_h

#include <Homie.h>

class ConfigurationManager
{

public:
  void setup();

  long GetSleepTime();
  long GetWeightOffset();
  double GetKilogramDivider();
  double GetCalibrationTemperatureSetting();
  double GetCalibrationFactorSetting();
  double GetVccAdjustSetting();
private:
  int SleepTime;
  byte CurrentState;
};
#endif