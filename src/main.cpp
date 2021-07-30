#include <Homie.h>
#include <Ticker.h>
#include "WeightProcessor.h"
#include "TemperatureProcessor.h"
#include "BatteryProcessor.h"
#include "MeasureHandler.h"
#include "DeviceManager.h"
#include "ConfigurationManager.h"
#include "TareUtility.h"

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

// Get weight
WeightProcessor scaledevice(GPIO_HX711_DT, GPIO_HX711_SCK);
TemperatureProcessor temperatures(GPIO_ONEWIRE_BUS);
BatteryProcessor batteryProcessor;
MeasureHandler measures;
DeviceManager devicemanager;
ConfigurationManager configurationManager;

TareUtility procs(&scaledevice);

void setupHandler()
{
  measures.setup();
}

void max_run()
{
  ++runtime_s;
  if (runtime_s == RUNTIME_MAX)
  {
    Homie.getLogger() << "DEBUG: Max. runtime of " << RUNTIME_MAX << "s reached, shutting down!" << endl;
    devicemanager.SetSleepTime(configurationManager.GetSleepTime());
    // Homie.getLogger() << "✔ Preparing for " << devicemanager.GetSleepTime() << " seconds sleep" << endl;
    Homie.prepareToSleep();
  }
}

void onHomieEvent(const HomieEvent &event)
{

  switch (event.type)
  {
  case HomieEventType::STANDALONE_MODE:
  case HomieEventType::CONFIGURATION_MODE:
  case HomieEventType::OTA_STARTED:
  case HomieEventType::OTA_PROGRESS:
  case HomieEventType::OTA_SUCCESSFUL:
  case HomieEventType::OTA_FAILED:
  case HomieEventType::ABOUT_TO_RESET:
  case HomieEventType::WIFI_CONNECTED:
  case HomieEventType::MQTT_DISCONNECTED:
  case HomieEventType::MQTT_PACKET_ACKNOWLEDGED:
  case HomieEventType::SENDING_STATISTICS:
    break;
  case HomieEventType::NORMAL_MODE:

    //timeout.attach(1.0, max_run);
    if (temperatures.getDeviceCount() > 0)
    {
      Homie.getLogger() << "DEBUG: " << temperatures.getDeviceCount() << " Temperature sensors attached" << endl;
      measures.SetTemperatureValue(0, temperatures.getTemperature(0));
    }
    else
    {
      Homie.getLogger() << "DEBUG: No Temperature sensors attached!" << endl;
    }

    if (scaledevice.DeviceReady())
    {
      Homie.getLogger() << "DEBUG: Try to get scale value!" << endl;
      float weight = scaledevice.getWeight(measures.GetTemperaturValue(1));
      measures.SetWeightValue(weight);
      Homie.getLogger() << "DEBUG: Got Scale value: " << weight << endl;
    }
    else
    {
      Homie.getLogger() << "DEBUG: Scale not ready or disconnected!" << endl;
    }

    measures.SetVoltage(batteryProcessor.getVolt());
    break;

  case HomieEventType::WIFI_DISCONNECTED:
    Homie.getLogger() << "Wifi disconnected" << endl;
    break;

  case HomieEventType::MQTT_READY:
    measures.SetLowBattery(batteryProcessor.IsLow());

    // When the battery is low, it mus sleep forever
    if (measures.GetLowBattery())
    {
      devicemanager.SetSleepTime(0);
    }
    else
    {
      devicemanager.SetSleepTime(configurationManager.GetSleepTime());
    }

    // Submitting the gathered data now.
    measures.SubmitData();

    Homie.getLogger() << "✔ Data is transmitted, waiting for package acks..." << endl;
    //max_run();
    delay(100);
    Homie.prepareToSleep();
    break;

  case HomieEventType::READY_TO_SLEEP:

    Homie.getLogger() << "DEBUG: Total runtime: " << millis() / 1000 << endl;
    devicemanager.GotToSleep();
    Homie.getLogger() << "Sleeping now!" << endl;
    break;
  }
}

void loopHandler()
{
}

void setup()
{
  Homie.disableResetTrigger();
  Homie.disableLedFeedback(); // collides with Serial on ESP07
  WiFi.forceSleepBegin();     // send wifi directly to sleep to reduce power consumption
  Serial.begin(115200);
  Homie.getLogger() << endl;
  Homie.getLogger() << "///////////////////////////////////////////" << endl;
  Homie.getLogger() << "GPIO_MAINTENANCE_PIN: " << GPIO_MAINTENANCE_PIN << endl;
  Homie.getLogger() << "GPIO_ONEWIRE_BUS: " << GPIO_ONEWIRE_BUS << endl;
  Homie.getLogger() << "GPIO_HX711_SCK: " << GPIO_HX711_SCK << endl;
  Homie.getLogger() << "GPIO_HX711_DT: " << GPIO_HX711_DT << endl;
  Homie.getLogger() << "FW_NAME: " << FW_NAME << endl;
  Homie.getLogger() << "FW_VERSION: " << FW_VERSION << endl;
  Homie.getLogger() << "Setup Maintenance pin: " << GPIO_MAINTENANCE_PIN << endl;
  Homie.getLogger() << "///////////////////////////////////////////" << endl;
  if (devicemanager.GetOperatingState() == OperatingStates::Maintenance)
  {
    Homie.getLogger() << "INFO: Device in Maintenance. Go to sleep" << endl;
    devicemanager.GotToSleep();
    return;
  }

  Homie.getLogger() << "INFO: Device in normal mode." << endl;

  if (devicemanager.IsColdstart())
  {
    Homie.getLogger() << "DEBUG: Is Coldstart set new initial State" << endl;
    devicemanager.SetStateAndMagicNumberToMemory();
    devicemanager.SetStateToMemory(STATE_COLDSTART);
  }
  devicemanager.ReadStateFromMemory();
  // // now the restart cause is clear, handle the different states
  Serial.printf("State: %d\r\n", devicemanager.GetCurrentState());
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
    Homie.onEvent(onHomieEvent);
    configurationManager.setup();

    temperatures.setup();
    scaledevice.setup(
        configurationManager.GetKilogramDivider(),
        configurationManager.GetWeightOffset(),
        configurationManager.GetCalibrationTemperatureSetting(),
        configurationManager.GetCalibrationFactorSetting());

    Homie.getLogger() << "DEBUG: Turn on WIFI: " << millis() / 1000 << endl;
    WiFi.forceSleepWake();
    delay(500);
    wifi_set_sleep_type(MODEM_SLEEP_T);
    Homie_setFirmware(FW_NAME, FW_VERSION);
    Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
    measures.AdvertiseNodes();
    Homie.getLogger() << "DEBUG: Before setup(): " << millis() / 1000 << endl;
    Homie.setup();
    break;
  }
}

void loop()
{
  Homie.loop();
}