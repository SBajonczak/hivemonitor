/*
  StateManager.h - Helper Libary for managing the Homie states.
*/
#ifndef DeviceManager_h
#define DeviceManager_h
#include "WeightProcessor.h"
#include <Arduino.h>
#include <ConfigurationManager.h>

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

  struct
  {
    uint32_t crc32;     // 4 bytes
    uint8_t channel;    // 1 byte,   5 in total
    uint8_t bssid[6];   // 6 bytes, 11 in total
    uint8_t padding;    // 1 byte,  12 in total
    byte state;         // 2 byte
  } rtcData;

public:
  void setup();
  DeviceManager(ConfigurationManager config);
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
  void GotToSleepForWirelesWakeUp(WakeMode mode);
  byte ReadStateFromMemory();
  void SetStateToMemory(byte value);
  void SetStateAndMagicNumberToMemory();
  bool IsColdstart();
  void SetState(byte state);
  byte GetCurrentState();
  // Get the actual Operating State
  OperatingStates GetOperatingState();
  void WriteRtcSettings();
  uint32_t calculateCRC32(const uint8_t *data, size_t length);
  void ConnectWifi();
  char *getDeviceID();

private:

  void SetMacAdress();

  static char _deviceId[15];
  void generateDeviceID();
  ConfigurationManager _config;
  OperatingStates operatingState;
  int SleepTime;
  byte CurrentState;
  bool RtcReadSuccess;
  // IPAddress staticAdress;
  // IPAddress gateway;
  // IPAddress subnet;
};
#endif