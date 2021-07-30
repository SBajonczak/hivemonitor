/*
  Weight.h - Lib for gtting weights 
*/
#ifndef WeightProcessor_h
#define WeightProcessor_h

#include "Arduino.h"

class WeightProcessor
{
public:
  WeightProcessor(int dtPin, int scPin);
  void setup(float kilogramDivider, float weightOffset,float calibrationTemperature, float calibrationFactor);
  float getWeight();
  float getWeight(float temperatureForCompensation);
  float toKilogram(float getWeighMeasure);
  bool DeviceReady();
  // Performs an automatic tare for the first run
  void AutoTare();
private:
  int _dtPin;
  int _scPin;
  float _kilogramDivider;
  float _weightOffset;
  float _calibrationTemperature;
  float _calibrationFactor;
};

#endif