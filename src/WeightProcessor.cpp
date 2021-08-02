/*
  Weight.h - Lib for gtting weights 
*/
#include "Arduino.h"
#include "WeightProcessor.h"
#include "HX711.h"
#include <RunningMedian.h>
// Avoiding compile issues
WeightProcessor* WeightProcessor::instance = nullptr;

int _dtPin;
int _scPin;
float _kilogramDivider;
float _weightOffset;
float _calibrationTemperature;
float _calibrationFactor;

HX711 scale;

WeightProcessor::WeightProcessor(int dtPin, int scPin)
{
  this->_dtPin = dtPin;
  this->_scPin = scPin;
  scale.begin(_dtPin, _scPin);
}

void WeightProcessor::setup(float kilogramDivider, float weightOffset, float calibrationTemperature, float calibrationFactor)
{
  this->_calibrationFactor = calibrationFactor;
  this->_calibrationTemperature = calibrationTemperature;
  this->_kilogramDivider = kilogramDivider;
  this->_weightOffset = weightOffset;
}

float WeightProcessor::getWeight()
{
  RunningMedian WeightSamples = RunningMedian(3);
  scale.power_up();
  delay(100);
  scale.set_scale(this->_kilogramDivider); //initialize scale
  scale.set_offset(this->_weightOffset);   //initialize scale
  for (int i = 0; i < 3; i++)
  {
    float WeightRaw = scale.get_units(3);
    WeightSamples.add(WeightRaw);
    delay(500);
    yield();
  }
  scale.power_down();

  float weight = WeightSamples.getMedian();
  return weight;
}

WeightProcessor::~WeightProcessor()
{
}

bool WeightProcessor::DeviceReady()
{
  return scale.wait_ready_timeout(200);
}

float WeightProcessor::getWeight(float temperatureForCompensation)
{
  if (scale.is_ready())
  {
    float weight = this->getWeight();
    //temperature compensation
    if (temperatureForCompensation < this->_calibrationTemperature)
      weight = weight + (fabs(temperatureForCompensation - this->_calibrationTemperature) * this->_calibrationFactor);
    if (temperatureForCompensation > this->_calibrationTemperature)
      weight = weight - (fabs(temperatureForCompensation - this->_calibrationTemperature) * this->_calibrationFactor);

    return weight;
  }
  return 0;
}

float WeightProcessor::toKilogram(float getWeighMeasure)
{
  return getWeighMeasure / 1000;
}
