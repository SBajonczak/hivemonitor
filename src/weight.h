/*
  Weight.h - Lib for gtting weights 
*/
#ifndef Weight_h
#define Weight_h

#include "Arduino.h"

class Weight
{
public:
  Weight(int dtPin, int scPin);
  void setup(float kilogramDivider, float weightOffset,float calibrationTemperature, float calibrationFactor);
  float getWeight();
  float getWeight(float temperatureForCompensation);
  float toKilogram(float getWeighMeasure);

private:
  int _dtPin;
  int _scPin;
  float _kilogramDivider;
  float _weightOffset;
  float _calibrationTemperature;
  float _calibrationFactor;
};

#endif