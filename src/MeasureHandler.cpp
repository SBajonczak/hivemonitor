#include <MeasureHandler.h>
#include "WeightProcessor.h"

/*
 * Scale should be calibrated with a regulated input of 3.3V!
 * Every change during runtime will cause wrong readings
 */
const float LOW_BATTERY = 1.1;

MeasureHandler::MeasureHandler(ConfigurationManager config)
{
  config.ReadSettings();
  this->configurationManager = config;

  //  this->configurationManager->setup();
}

void MeasureHandler::setupHandler()
{
}

void MeasureHandler::SetLowBattery(bool isLow)
{
  this->lowBattery = isLow;
}

bool MeasureHandler::GetLowBattery()
{
  return this->lowBattery;
}

void MeasureHandler::SetTemperatureValue(int index, float temperature)
{
  switch (index)
  {
  case 0:
    this->temperature0 = temperature;
    break;
  case 1:
    this->temperature1 = temperature;
    break;
  }
}

float MeasureHandler::GetTemperaturValue(int index)
{
  switch (index)
  {
  case 0:
    return this->temperature0;
  case 1:
    return this->temperature1;
  }
  return 0;
}

void MeasureHandler::SetVoltage(float voltage)
{

  Serial.println("Got Voltage");
  this->voltage = voltage;
}

float MeasureHandler::GetWeightValue()
{
  return this->weight;
}

float MeasureHandler::GetVoltage()
{
  return this->voltage;
}

float MeasureHandler::round2two(float tmp)
{
  return ((int)(tmp * 100)) / 100.00;
}
