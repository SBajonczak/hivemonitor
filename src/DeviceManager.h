/*
  StateManager.h - Helper Libary for managing the Homie states.
*/
#ifndef DeviceManager_h
#define DeviceManager_h
#include "WeightProcessor.h"

// The Memmory Adress for the state
#define RTC_STATE 66
#define RTC_BASE 65

// state definitions for wake/sleep cycles
#define STATE_COLDSTART 0
#define STATE_SLEEP_WAKE 1
#define STATE_CONNECT_WIFI 4

enum OperatingStates
{
  Maintenance = 1,
  Operating = 2
};

class DeviceManager
{

public:
  void setup();
  DeviceManager();
  // Set the Sleeptime in Seconds
  void SetSleepTime(int time);
  // Get the sleeptime in Seconds
  int GetSleepTime();
  // Do some Setup
  void Setup();

  /* 
    Sets the ESP Device to sleep, it convert the sleeptime autmatically into ms. 
    It will also set the next state for the wakeup process.
  */
  void GotToSleep();
  void GotToSleep(WakeMode mode);
  byte ReadStateFromMemory();
  void SetStateToMemory(byte value);
  void SetStateAndMagicNumberToMemory();
  bool IsColdstart();
  void SetState(byte state);
  byte GetCurrentState();
  // Get the actual Operating State
  OperatingStates GetOperatingState();
  void ConnectWifi();
  char *getDeviceID();

private:
   static char _deviceId[15];
  void generateDeviceID();
  OperatingStates operatingState;
  int SleepTime;
  byte CurrentState;
};
#endif