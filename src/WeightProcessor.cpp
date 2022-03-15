/*
  Weight.h - Lib for gtting weights 
*/
#include "Arduino.h"
#include "WeightProcessor.h"
#include "HX711.h"
#include <RunningMedian.h>
// Avoiding compile issues
// WeightProcessor *WeightProcessor::instance = nullptr;

int _dtPin;
int _scPin;
float _kilogramDivider;
float _weightOffset;
float _calibrationTemperature;
float _calibrationFactor;

HX711 scale;

WeightProcessor::WeightProcessor(int dtPin, int scPin,ConfigurationManager config)
{
  this->_config= config;
  this->_config.ReadSettings();

  this->_calibrationFactor = this->_config.GetCalibrationFactorSetting();
  this->_calibrationTemperature = this->_config.GetCalibrationTemperatureSetting();
  this->_kilogramDivider = this->_config.GetKilogramDivider();
  this->_weightOffset = this->_config.GetWeightOffset();
  
  this->_dtPin = dtPin;
  this->_scPin = scPin;
  scale.begin(_dtPin, _scPin);
}

void WeightProcessor::SetScale()
{
  scale.set_scale();
}

void WeightProcessor::Tare()
{
  scale.tare();
}

float WeightProcessor::GetUnits()
{
  return scale.get_units(10);
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
  Serial.print("Kilogram Divider");
  Serial.println(this->_kilogramDivider);
  scale.set_scale(this->_kilogramDivider); //initialize scale
  Serial.print("Weight offset");
  Serial.println(this->_weightOffset);
  scale.set_offset(this->_weightOffset);   //initialize scale
  for (int i = 0; i < 3; i++)
  {

    scale.power_up();
    float WeightRaw = scale.get_units(3);
    scale.power_down();
    Serial.print("Weight RAW Value:");
    Serial.println(WeightRaw);
    WeightSamples.add(WeightRaw);
    delay(250);
  }
  float median = WeightSamples.getMedian();
  Serial.print("Median:");
  Serial.println(median);
  return median;
}

WeightProcessor::~WeightProcessor()
{
}

bool WeightProcessor::DeviceReady()
{

  return scale.wait_ready_timeout(1000);
}




float WeightProcessor::getRawWeight()
{

  RunningMedian WeightSamples = RunningMedian(3);
  for (int i = 0; i < 3; i++)
  {
    scale.power_up();
    float WeightRaw = scale.read_average(3);
    scale.power_down();
    Serial.print("Weight RAW Value:");
    Serial.println(WeightRaw);
    WeightSamples.add(WeightRaw);
    delay(50);
  }
  float median = WeightSamples.getMedian();
  Serial.print("Median:");
  Serial.println(median);
  return median;
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
  return -127;
}

float WeightProcessor::toKilogram(float getWeighMeasure)
{
  return getWeighMeasure / 1000;
}
