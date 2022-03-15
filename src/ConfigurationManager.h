#ifndef ConfigurationManager_h
#define ConfigurationManager_h
#include <Arduino.h>
#include <ArduinoJson.h>
#define USE_SPIFFS 0
#include <FS.h> // Include the SPIFFS library
#if USE_SPIFFS
#include <spiffs/spiffs.h>
#else
#include <LittleFS.h>
#endif
class ConfigurationManager
{

public:
  ConfigurationManager();
  void setup();
  long GetSleepTime();
  long GetWeightOffset();
  double GetKilogramDivider();
  double GetCalibrationTemperatureSetting();
  double GetCalibrationFactorSetting();
  double GetVccAdjustSetting();
  void ApplyJsonInput(String json);
  bool HasValidConfiguration();

  String GetWifiSsid();
  String GetWifiPassword();
  String GetMqttServer();
  int GetMqttPort();
  String GetMqttUser();
  String GetMqttPassword();

  String GetJson();
  void StoreSettings();
  void ReadSettings();

  // Get an Singelton instance
  static ConfigurationManager *getInstance()
  {
    if (instance == nullptr)
    {
      instance = new ConfigurationManager();
    }
    return instance;
  }

private:
  static ConfigurationManager *instance;
  float VccAdjustSetting;
  int SleepTime;
  String WifiSsid;
  String WifiPassword;

  String MqttServer;
  String MqttUser;
  int MqttPort;
  String MqttPassword;

  // weight
  float WeightOffset;
  float KilogramDivider;
  float CalibrationTemperatureSetting;
  float CalibrationFactorSetting;
  byte CurrentState;

  DynamicJsonDocument GetJsonDocument();
};
#endif