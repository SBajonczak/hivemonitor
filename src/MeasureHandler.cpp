#include <MeasureHandler.h>
#include "WeightProcessor.h"

/*
  * Scale should be calibrated with a regulated input of 3.3V!
  * Every change during runtime will cause wrong readings
  */
const float LOW_BATTERY = 1.1;

MeasureHandler::MeasureHandler()
{
  this->configurationManager = ConfigurationManager::getInstance();
  this->configurationManager->setup();
}

void MeasureHandler::setupHandler()
{
}

void MeasureHandler::SetWeightValue(float weightValue)
{
  Serial.println("DEBUG: Scale class initalized!");

  WeightProcessor scaledevice(GPIO_HX711_DT, GPIO_HX711_SCK);
  Serial.println("DEBUG: Scale class initalized!");
  scaledevice.setup(
      this->configurationManager->GetKilogramDivider(),
      this->configurationManager->GetWeightOffset(),
      this->configurationManager->GetCalibrationTemperatureSetting(),
      this->configurationManager->GetCalibrationFactorSetting());
  if (scaledevice.DeviceReady())
  {
    Serial.println("DEBUG: Try to get scale value!");
    this->weight = scaledevice.getWeight(0);
    Serial.println("Got Weight value!");
  }
  else
  {
    this->weight = -127;
    Serial.println("Scale not ready!");
  }
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
