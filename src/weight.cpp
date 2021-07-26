/*
  Weight.h - Lib for gtting weights 
*/
#include "Arduino.h"
#include "weight.h"
#include "HX711.h"
#include <RunningMedian.h>

int _dtPin;
int _scPin;
float _kilogramDivider;
float _weightOffset;
float _calibrationTemperature;
float _calibrationFactor;

HX711 scale;

Weight::Weight(int dtPin, int scPin)
{
  this->_dtPin = dtPin;
  this->_scPin = scPin;
}

void Weight::setup(float kilogramDivider, float weightOffset, float calibrationTemperature, float calibrationFactor)
{
  this->_calibrationFactor = calibrationFactor;
  this->_calibrationTemperature = calibrationTemperature;
  this->_kilogramDivider = kilogramDivider;
  this->_weightOffset = weightOffset;
}

float Weight::getWeight()
{
  RunningMedian WeightSamples = RunningMedian(3);

  scale.begin(_dtPin, _scPin);
  scale.power_up();
  delay(100);
  scale.set_scale(this->_kilogramDivider); //initialize scale
  scale.set_offset(this->_weightOffset);   //initialize scale
  for (int i = 0; i < 3; i++)
  {
    float WeightRaw = scale.get_units(3);
    //yield();
    WeightSamples.add(WeightRaw);
    delay(500);
    yield();
  }
  scale.power_down();

  float weight = WeightSamples.getMedian();
  return weight;
}

float Weight::getWeight(float temperatureForCompensation)
{
  float weight = this->getWeight();
  //temperature compensation
  if (temperatureForCompensation < this->_calibrationTemperature)
    weight = weight + (fabs(temperatureForCompensation - this->_calibrationTemperature) * this->_calibrationFactor);
  if (temperatureForCompensation > this->_calibrationTemperature)
    weight = weight - (fabs(temperatureForCompensation - this->_calibrationTemperature) * this->_calibrationFactor);

  return weight;
}

float Weight::toKilogram(float getWeighMeasure)
{
  return getWeighMeasure / 1000;
}
