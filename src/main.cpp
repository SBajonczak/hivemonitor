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
ConfigurationManager config;
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
MeasureHandler measures(config);
DeviceManager devicemanager(config);
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

void setup()
{

  Serial.begin(115200);
  Serial.println("Trigger Setup");
  mqtt.Setup();
  wifi_client.setTrustAnchors(&cert);

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
      Serial.println("Coldstart Initiated");
      devicemanager.SetStateAndMagicNumberToMemory();
      Serial.println("Set state to STATE_COLDSTART");

      devicemanager.SetStateToMemory(STATE_COLDSTART);
    }
    devicemanager.ReadStateFromMemory();

    switch (devicemanager.GetCurrentState())
    {
    default: // Catch all unknown states
    case STATE_SLEEP_WAKE:
      // first run after power on - initializes
    case STATE_COLDSTART:
      Serial.println("Now in state to STATE_COLDSTART");
      // Prepare to setup the WIFI

      WiFi.forceSleepWake();
      // WiFi.mode(WIFI_STA);
      // one more sleep required to to wake with wifi on
      Serial.println("Set state to STATE_CONNECT_WIFI");
      devicemanager.SetStateToMemory(STATE_CONNECT_WIFI);
      devicemanager.GotToSleepForWirelesWakeUp(WAKE_RFCAL);

      break;
    case STATE_CONNECT_WIFI:
      Serial.println("Now in state to STATE_CONNECT_WIFI");
      WiFi.forceSleepWake();
      // Set modem to sleep
      wifi_set_sleep_type(MODEM_SLEEP_T);

      temperatures.setup();
     
      devicemanager.ConnectWifi();
      // Connect to the server externally, because pubsubclient has some major problems when do it internal
      wifi_client.connect(MQTT_SERVER, MQTT_PORT);
      //Connecting to the mqtt server
      mqtt.Connect();
      // sync Time
      initializeTime();
      // Measure Battery
      measures.SetLowBattery(batteryProcessor.IsLow());
      measures.SetVoltage(batteryProcessor.getVolt());

      DynamicJsonDocument doc(MQTT_PACKET_SIZE);

      // Get temperature
      if (temperatures.getDeviceCount() > 0)
      {
        for (int i = 0; i < temperatures.getDeviceCount(); i++)
        {
          doc["Temperatures"][i]["Celsius"] = measures.GetTemperaturValue(0);
          doc["Temperatures"][i]["Index"] = i;
        }
        doc["Temperatures"]["Connected"] = true;
      }
      else
      {
        doc["Temperatures"]["Connected"] = false;
        Serial.println("No Temperature Sensors connected");
      }

      WeightProcessor scaledevice(GPIO_HX711_DT, GPIO_HX711_SCK);
      if (scaledevice.DeviceReady())
      {
        doc["weight"]["Connected"] = true;
        doc["weight"]["value"] = scaledevice.getWeight();
      }
      else
      {
        doc["weight"]["Connected"] = false;
        Serial.println("No Scaledevice connected");
      }
      doc["System"]["Battery"]["Volts"] = batteryProcessor.getVolt();
      doc["System"]["Battery"]["islow"] = measures.GetLowBattery();

      time_t now = time(NULL);
      doc["System"]["Time"] = ctime(&now);
      doc["System"]["Sleeptime"] = ESP.deepSleepMax();
      Serial.println("Try to send");
      String jsonData;
      serializeJson(doc, jsonData);

      mqtt.Send(jsonData);

      // When the battery is low, it mus sleep forever
      if (measures.GetLowBattery())
      {
        devicemanager.SetSleepTime(0);
      }
      else
      {
        devicemanager.SetSleepTime(config.GetSleepTime());
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