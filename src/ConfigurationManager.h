/*
  StateManager.h - Helper Libary for managing the Homie states.
*/
#ifndef ConfigurationManager_h
#define ConfigurationManager_h

#include <Homie.h>

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