#include <ConfigurationManager.h>
#include <EEPROM.h> // Reference for Memmory handling
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h> // Include the SPIFFS library
#define CONFIG_FILE "/config.json"

// Avoiding compile issues
ConfigurationManager *ConfigurationManager::instance = nullptr;

#define JSON_DOCSIZE 1024

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

void ConfigurationManager::setup()
{
  this->SleepTime = DEFAULT_SLEEP_TIME;
  this->WeightOffset = DEFAULT_WEIGHT_OFFSET;
  this->KilogramDivider = DEFAULT_KILOGRAM_DIVIDER;
  this->VccAdjustSetting = DEFAULT_VCC_ADJUST;
  this->CalibrationFactorSetting = DEFAULT_CALIBRATION_TEMPERATURE;
  this->CalibrationFactorSetting = DEFAULT_CALIBRATION_FACTOR_GRAM_DEGREE;
}

ConfigurationManager::ConfigurationManager()
{
  //this->ReadSettings();
}

long ConfigurationManager::GetSleepTime()
{
  return this->SleepTime;
}
long ConfigurationManager::GetWeightOffset()
{

  return this->WeightOffset;
}
double ConfigurationManager::GetKilogramDivider()
{
  return this->KilogramDivider;
}
double ConfigurationManager::GetCalibrationTemperatureSetting()
{
  return this->CalibrationTemperatureSetting;
}
double ConfigurationManager::GetCalibrationFactorSetting()
{
  return this->CalibrationFactorSetting;
}
double ConfigurationManager::GetVccAdjustSetting()
{

  return this->VccAdjustSetting;
}

String ConfigurationManager::GetWifiSsid() { return this->WifiSsid; }
String ConfigurationManager::GetWifiPassword() { return this->WifiPassword; }
String ConfigurationManager::GetMqttServer() { return this->MqttServer; }
int ConfigurationManager::GetMqttPort() { return this->MqttPort; }
String ConfigurationManager::GetMqttUser() { return this->MqttUser; }
String ConfigurationManager::GetMqttPassword() { return this->MqttPassword; }

String ConfigurationManager::GetJson()
{
  String values;
  DynamicJsonDocument doc = this->GetJsonDocument();
  serializeJson(doc, values);
  return values;
}

DynamicJsonDocument ConfigurationManager::GetJsonDocument()
{
  DynamicJsonDocument doc(JSON_DOCSIZE);
  doc["system"]["sleeptime"] = this->GetSleepTime();
  doc["system"]["vccadjustsetting"] = this->GetVccAdjustSetting();
  doc["scale"]["weightoffset"] = this->GetWeightOffset();
  doc["scale"]["kilogramdivider"] = this->GetKilogramDivider();
  doc["scale"]["calibrationtemperaturesetting"] = this->GetCalibrationTemperatureSetting();
  doc["scale"]["calibrationfactorsetting"] = this->GetCalibrationFactorSetting();
  doc["wireles"]["ssid"] = this->GetWifiSsid();
  doc["wireles"]["password"] = this->GetWifiPassword();
  doc["mqtt"]["server"] = this->GetMqttServer();
  doc["mqtt"]["port"] = this->GetMqttPort();
  doc["mqtt"]["user"] = this->GetMqttUser();
  doc["mqtt"]["password"] = this->GetMqttPassword();
  return doc;
}

void ConfigurationManager::ApplyJsonInput(String json)
{
  DynamicJsonDocument jData(2048);
  Serial.println(json);
  deserializeJson(jData, json);

  this->WeightOffset = jData["scale"]["weightoffset"];
  this->KilogramDivider = jData["scale"]["kilogramdivider"];
  this->CalibrationTemperatureSetting = jData["scale"]["calibrationtemperaturesetting"];
  this->CalibrationFactorSetting = jData["scale"]["calibrationfactorsetting"];

  this->SleepTime = jData["system"]["sleeptime"];
  this->VccAdjustSetting = jData["system"]["vccadjustsetting"];

  this->MqttServer = jData["mqtt"]["server"].as<String>();
  this->MqttUser = jData["mqtt"]["user"].as<String>();
  this->MqttPassword = jData["mqtt"]["password"].as<String>();
  this->MqttPort = jData["mqtt"]["port"];

  this->WifiSsid = jData["wireles"]["ssid"].as<String>();
  this->WifiPassword = jData["wireles"]["password"].as<String>();


  if (jData.isNull())
  {

  }

  // else
  // {
  //   Serial.println("is null");
  // }
  // this->WifiPassword = jData["wireles"]["password"].as<String>();
  // this->WifiSsid = jData["wireles"]["ssid"].as<String>();
}

void ConfigurationManager::StoreSettings()
{
  String json = this->GetJson();
  if (SPIFFS.begin())
  {
    SPIFFS.remove(CONFIG_FILE);
    File configFile = SPIFFS.open(CONFIG_FILE, "w");
    if (configFile)
    {
      Serial.println("Saving content: ");
      Serial.println(json);
      configFile.println(json);
      configFile.close();
    }
  }
}

void ConfigurationManager::ReadSettings()
{

  if (SPIFFS.begin())
  {
    if (SPIFFS.exists(CONFIG_FILE))
    {

      File configFile = SPIFFS.open(CONFIG_FILE, "r");
      if (configFile)
      {
        Serial.print("Read content:");
        String s = configFile.readString();
        this->ApplyJsonInput(s);
        Serial.println("Done!✔");
        return;
      }
    }
  }
}
