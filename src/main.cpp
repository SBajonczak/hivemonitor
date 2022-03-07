#include <Ticker.h>
#include <time.h>
#include <ESP8266WiFi.h>
#include "WeightProcessor.h"
#include "TemperatureProcessor.h"
#include "BatteryProcessor.h"
#include "MeasureHandler.h"
#include "DeviceManager.h"
// #include "ConfigurationManager.h"
#include "TareUtility.h"
#include "ConfigWebserver.h"
#include "MqttWrapper.h"
#include <PubSubClient.h>
#include <azure_ca.h>
#define NTP_SERVERS "pool.ntp.org", "time.nist.gov"



 WiFiClientSecure wifi_client;
X509List cert((const char *)ca_pem);
PubSubClient mqtt_client(wifi_client);

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

// Make thrust connection

TemperatureProcessor temperatures(GPIO_ONEWIRE_BUS);
BatteryProcessor batteryProcessor;
MeasureHandler measures;
DeviceManager devicemanager;
ConfigWebserver configServer;
MqttWrapper mqtt(devicemanager.getDeviceID(), mqtt_client);

static void initializeTime()
{
  Serial.print("Setting time using SNTP");

  configTime(-5 * 3600, 0, NTP_SERVERS);
  time_t now = time(NULL);
  while (now < 1510592825)
  {
    delay(500);
    Serial.print(".");
    now = time(NULL);
  }
  Serial.println("done!");
}

void max_run()
{
  ++runtime_s;
  if (runtime_s == RUNTIME_MAX)
  {
    // Serial.println()  << "DEBUG: Max. runtime of " << RUNTIME_MAX << "s reached, shutting down!" << endl;
    devicemanager.SetSleepTime(ConfigurationManager::getInstance()->GetSleepTime());
    // Serial.println()  << "✔ Preparing for " << devicemanager.GetSleepTime() << " seconds sleep" << endl;
    devicemanager.GotToSleep();
    // Homie.prepareToSleep();
  }
}

void setup()
{

  Serial.begin(115200);
  mqtt.Setup();
  wifi_client.setTrustAnchors(&cert);

  // if (!ConfigurationManager::getInstance()->HasValidConfiguration())
  // {

  //   //   Serial.println("No valid configuration available. Starting configuration mode");
  //   configServer.Serve();
  //   // return;
  // }

  //  ConfigurationManager::getInstance()->ReadSettings();
  // Get the settings

  switch (devicemanager.GetOperatingState())
  {
  case OperatingStates::Maintenance:
    Serial.println("Maintenance / Configuration mode");
    configServer.Serve();
    // return;
    break;
  case OperatingStates::Operating:

    if (devicemanager.IsColdstart())
    {
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
      // WiFi.mode(WIFI_STA);
      // one more sleep required to to wake with wifi on
      devicemanager.SetStateToMemory(STATE_CONNECT_WIFI);
      devicemanager.GotToSleep(WAKE_RFCAL);

      break;
    case STATE_CONNECT_WIFI:

    
      WiFi.forceSleepWake();
    //Set modem to sleep
    wifi_set_sleep_type(MODEM_SLEEP_T);

    temperatures.setup();
    devicemanager.ConnectWifi();
    Serial.println("Normal mode");
    mqtt.Connect();

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
    mqtt.Queue("/hive/battery/islow", 1);
    Serial.println("Try to send");
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

    devicemanager.GotToSleep();
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