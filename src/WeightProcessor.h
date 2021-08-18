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
  float getRawWeight();
  float toKilogram(float getWeighMeasure);
  bool DeviceReady();
  
/*
To Autotare just following these steps

1. Call SetScale().
2 .Call Tare().
3. Place a known weight on the scale and call get_units(10).


Divide the result in step 3 to your known weight. You should get about the parameter you need to pass to set_scale().
Adjust the parameter in step 4 until you get an accurate reading.

*/
  void SetScale();
  void Tare();
  float GetUnits();

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