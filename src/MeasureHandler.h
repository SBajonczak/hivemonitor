/*
  StateManager.h - Helper Libary for managing the Homie states.
*/
#ifndef MeasureHandler_h
#define MeasureHandler_h

#include <Homie.h>

//Workaround for https://github.com/bblanchon/ArduinoJson/issues/566
#define ARDUINOJSON_USE_DOUBLE 0

class MeasureHandler
{

public:
  void setup();
  void setupHandler();
  void AdvertiseNodes();
  
  MeasureHandler();

  void SetWeightValue(float weightValue);
  void SetTemperatureValue(int index, float temperature);
  void SetVoltage(float voltage);
  float GetVoltage();
  // Get the Temperature Value for the given index.
  float GetTemperaturValue(int index);
  void SubmitData();
  void SetLowBattery(bool isLow);
  bool GetLowBattery();

private:
  int id;
  float weight;
  float voltage;
  float temperature0;
  float temperature1;
  bool lowBattery;

  float round2two(float tmp);
};
#endif