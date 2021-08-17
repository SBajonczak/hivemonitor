#include <Ticker.h>
#include "WeightProcessor.h"
#include "TemperatureProcessor.h"
#include "BatteryProcessor.h"
#include "MeasureHandler.h"
#include "DeviceManager.h"
// #include "ConfigurationManager.h"
#include "TareUtility.h"
#include "ConfigWebserver.h"
#include "MqttWrapper.h"

#define FW_NAME "Development"
#define FW_VERSION "0.10.0"
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";

// Set the Mode for the BAttery measuring
ADC_MODE(ADC_VCC);

// deep sleep infrastructure
// Include API-Headers
extern "C"
{
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "cont.h"
}

// For connection loss detection, timeout in seconds
#define RUNTIME_MAX 15
int runtime_s = 0;
Ticker timeout;

TemperatureProcessor temperatures(GPIO_ONEWIRE_BUS);
BatteryProcessor batteryProcessor;
MeasureHandler measures;
DeviceManager devicemanager;
ConfigWebserver configServer;
MqttWrapper mqtt(devicemanager.getDeviceID());

void setupHandler()
{
  measures.setup();
}

void max_run()
{
  ++runtime_s;
  if (runtime_s == RUNTIME_MAX)
  {
    // Serial.println()  << "DEBUG: Max. runtime of " << RUNTIME_MAX << "s reached, shutting down!" << endl;
    devicemanager.SetSleepTime(ConfigurationManager::getInstance()->GetSleepTime());
    //Serial.println()  << "✔ Preparing for " << devicemanager.GetSleepTime() << " seconds sleep" << endl;
    devicemanager.GotToSleep();
    // Homie.prepareToSleep();
  }
}

void setup()
{
  Serial.begin(115200);
  if (!ConfigurationManager::getInstance()->HasValidConfiguration())
  {

    Serial.println("No valid configuration available. Starting configuration mode");
    configServer.Serve();
    return;
  }
  // Get the settings
  ConfigurationManager::getInstance()->ReadSettings();
  // Homie.disableResetTrigger();
  // Homie.disableLedFeedback(); // collides with Serial on ESP07

  switch (devicemanager.GetOperatingState())
  {
  case OperatingStates::Maintenance:
    Serial.println("Maintenance / Configuration mode");
    configServer.Serve();
    return;
    break;
  case OperatingStates::Operating:
    Serial.println("Normal mode");

    if (devicemanager.IsColdstart())
    {
      // Serial.println()  << "DEBUG: Is Coldstart set new initial State" << endl;
      devicemanager.SetStateAndMagicNumberToMemory();
      devicemanager.SetStateToMemory(STATE_COLDSTART);
    }
    devicemanager.ReadStateFromMemory();
    switch (devicemanager.GetCurrentState())
    {
    default: // Catch all unknown states
    case STATE_SLEEP_WAKE:
      // first run after power on - initializes
    case STATE_COLDSTART:
      // Prepare to setup the WIFI

      WiFi.forceSleepWake();
      WiFi.mode(WIFI_STA);
      // one more sleep required to to wake with wifi on
      devicemanager.SetStateToMemory(STATE_CONNECT_WIFI);
      devicemanager.GotToSleep(WAKE_RFCAL);

      break;
    case STATE_CONNECT_WIFI:

      temperatures.setup();
      WiFi.forceSleepWake();
      delay(500);
      

      devicemanager.ConnectWifi();
      // Measure Battery
      measures.SetLowBattery(batteryProcessor.IsLow());
      measures.SetVoltage(batteryProcessor.getVolt());

      // Get temperature
      if (temperatures.getDeviceCount() > 0)
      {
        for (int i = 0; i < temperatures.getDeviceCount(); i++)
        {
          char internalTopic[155];
          strcpy(internalTopic, "/hive/Temperature/");
          strcat(internalTopic, String(i).c_str());
          strcat(internalTopic, "/value");
          mqtt.Queue(internalTopic, measures.GetTemperaturValue(0));
          measures.SetTemperatureValue(0, temperatures.getTemperature(0));
        }
        mqtt.Queue("/hive/Temperature/connected", 1);
      }
      else
      {
        mqtt.Queue("/hive/Temperature/connected", 0);
        Serial.println("No Temperature Sensors connected");
      }

      WeightProcessor scaledevice(GPIO_HX711_DT, GPIO_HX711_SCK);
      if (scaledevice.DeviceReady())
      {
        mqtt.Queue("/hive/weight/connected", 1);
        mqtt.Queue("/hive/weight/value", scaledevice.getWeight());
      }
      else
      {
        mqtt.Queue("/hive/weight/connected", 0);
        Serial.println("No Scaledevice connected");
      }
      mqtt.Queue("/hive/battery/volt", batteryProcessor.getVolt());
      mqtt.Queue("/hive/battery/islow",1 );
      mqtt.Send();

      // When the battery is low, it mus sleep forever
      if (measures.GetLowBattery())
      {
        devicemanager.SetSleepTime(0);
      }
      else
      {
        devicemanager.SetSleepTime(ConfigurationManager::getInstance()->GetSleepTime());
      }

      WiFi.forceSleepBegin(); // send wifi directly to sleep to reduce power consumption
      // Set modem to sleep
      wifi_set_sleep_type(MODEM_SLEEP_T);
      break;
    }
    break;
  }
}

void loop()
{
  if (!devicemanager.GetOperatingState() == OperatingStates::Maintenance)
  {
  }
}