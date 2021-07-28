#include <ConfigurationManager.h>
const int DEFAULT_SLEEP_TIME = 20;
/* 
    Use sketch BeeScale-Calibration.ino to determine these calibration values.
    Set them here or use HomieSetting via config.json or WebApp/MQTT
  */
const long DEFAULT_WEIGHT_OFFSET = 666;                   // Load cell zero offset.
const float DEFAULT_KILOGRAM_DIVIDER = 21.59;             // Load cell value per kilogram.
const float DEFAULT_CALIBRATION_TEMPERATURE = 23.44;      // Temperature at which the scale has been calibrated for Temperature compensation
const float DEFAULT_CALIBRATION_FACTOR_GRAM_DEGREE = 0.0; // Calibration factor in gram per degree

/*
  * *Some* ESPs tend to have a too low reading on ESP.getVcc()
  * To get the maximum battery runtime you should check what ADC is reading and compare
  * it with a multimeter between VCC and GND at the ESP. Change the following value to e.g. 0.13
  * or -0.02 in case of a too high reading. Or set it using HomieSetting via config.json or WebApp/MQTT
  */
const float DEFAULT_VCC_ADJUST = 0.23;

HomieSetting<long> sleepTimeSetting("sleepTime", "SleepTime in seconds (max. 3600s!), default is 60s");
HomieSetting<long> weightOffsetSetting("weightOffset", "Offset value to zero.");
HomieSetting<double> kilogramDividerSetting("kilogramDivider", "Scale value per kilogram.");
HomieSetting<double> calibrationTemperatureSetting("calibrationTemperature", "Outside Temperature at which the scale has been calibrated");
HomieSetting<double> calibrationFactorSetting("calibrationFactor", "Calibration Factor in gram per degree. 0.0 to disable adjustment");
HomieSetting<double> vccAdjustSetting("vccAdjust", "Calibration value for input voltage. See sketch for details.");

void ConfigurationManager::setup()
{

  sleepTimeSetting.setDefaultValue(DEFAULT_SLEEP_TIME);
  /*
      * Can't set more than one value to default, see issue #323
      * https://github.com/marvinroger/homie-esp8266/issues/323
      */
  weightOffsetSetting.setDefaultValue(DEFAULT_WEIGHT_OFFSET);
  kilogramDividerSetting.setDefaultValue(DEFAULT_KILOGRAM_DIVIDER);
  vccAdjustSetting.setDefaultValue(DEFAULT_VCC_ADJUST);
  calibrationTemperatureSetting.setDefaultValue(DEFAULT_CALIBRATION_TEMPERATURE);
  calibrationFactorSetting.setDefaultValue(DEFAULT_CALIBRATION_FACTOR_GRAM_DEGREE);
}

long ConfigurationManager::GetSleepTime()
{
  return sleepTimeSetting.get();
}
long ConfigurationManager::GetWeightOffset()
{

  return weightOffsetSetting.get();
}
double ConfigurationManager::GetKilogramDivider()
{
  return kilogramDividerSetting.get();
}
double ConfigurationManager::GetCalibrationTemperatureSetting()
{
  return calibrationTemperatureSetting.get();
}
double ConfigurationManager::GetCalibrationFactorSetting()
{
  return calibrationFactorSetting.get();
}
double ConfigurationManager::GetVccAdjustSetting()
{

  return vccAdjustSetting.get();
}