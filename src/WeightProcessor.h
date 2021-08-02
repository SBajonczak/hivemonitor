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
  ~WeightProcessor();
  void setup(float kilogramDivider, float weightOffset, float calibrationTemperature, float calibrationFactor);
  float getWeight();
  float getWeight(float temperatureForCompensation);
  float toKilogram(float getWeighMeasure);
  bool DeviceReady();
  // Performs an automatic tare for the first run
  void AutoTare();

  // Get an Singelton instance
  static WeightProcessor *getInstance(int dtPin, int scPin)
  {
    if (instance == nullptr)
    {
      Serial.println("Generate new instance");
      instance = new WeightProcessor(GPIO_HX711_DT, GPIO_HX711_SCK);
    }
    else
    {
      Serial.println("Reuse instance");
    }
    return instance;
  }

private:
  static WeightProcessor *instance;

  int _dtPin;
  int _scPin;
  float _kilogramDivider;
  float _weightOffset;
  float _calibrationTemperature;
  float _calibrationFactor;
};

#endif