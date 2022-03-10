#include <DeviceManager.h>
#include "TareUtility.h"
#include "ConfigurationManager.h"
#include <ESP8266WiFi.h>

void DeviceManager::setup() {}
char DeviceManager::_deviceId[]; // need to define the static variable
#ifdef ESP32
void DeviceManager::generateDeviceID()
{
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  snprintf(DeviceManager::_deviceId, 15 + 1, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
#elif defined(ESP8266)
void DeviceManager::generateDeviceID()
{
  uint8_t mac[6];
  WiFi.macAddress(mac);
  snprintf(DeviceManager::_deviceId, 15, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
#endif // ESP32

char *DeviceManager::getDeviceID()
{
  this->generateDeviceID();
  return DeviceManager::_deviceId;
}

DeviceManager::DeviceManager(ConfigurationManager config)
{ 
  config.ReadSettings();
  this->_config= config;
  ReadStateFromMemory();
  this->SetMacAdress();
}

void DeviceManager::SetMacAdress()
{
  char deviceId[13]; // need to define the static variable
  uint8_t mac[6];
  WiFi.macAddress(mac);
  snprintf(deviceId, 13, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void DeviceManager::Setup()
{
}

void DeviceManager::SetSleepTime(int time)
{
  this->SleepTime = time;
}

int DeviceManager::GetSleepTime()
{
  return this->SleepTime;
}

void DeviceManager::GotToSleepForWirelesWakeUp(WakeMode mode)
{
  // Sleeping 2 seconds
  ESP.deepSleep(2 * 1000000, mode);
}

void DeviceManager::GotToSleep()
{
  int seconds = this->GetSleepTime();
  if (seconds == 0)
  {
    // Preventing to go to sleep forever.
    seconds = 5;
  }
  // Set the next state into the memory.
  this->SetStateToMemory(STATE_SLEEP_WAKE);
  Serial.print("Going to deep sleep for ");
  Serial.print(this->_config.GetSleepTime());
  Serial.println(" Minutes");

  ESP.deepSleep((this->_config.GetSleepTime()*1000000* 60)); // max deepsleep 71
}

void DeviceManager::SetStateToMemory(byte value)
{
  rtcData.state = value;
  WriteRtcSettings();
}

void DeviceManager::SetStateAndMagicNumberToMemory()
{
  WriteRtcSettings();
  ;
}

bool DeviceManager::IsColdstart()
{
  return !this->RtcReadSuccess;
}

void DeviceManager::SetState(byte state)
{
  rtcData.state = state;
}

uint32_t DeviceManager::calculateCRC32(const uint8_t *data, size_t length)
{
  uint32_t crc = 0xffffffff;
  while (length--)
  {
    uint8_t c = *data++;
    for (uint32_t i = 0x80; i > 0; i >>= 1)
    {
      bool bit = crc & 0x80000000;
      if (c & i)
      {
        bit = !bit;
      }

      crc <<= 1;
      if (bit)
      {
        crc ^= 0x04c11db7;
      }
    }
  }

  return crc;
}

void DeviceManager::WriteRtcSettings()
{

  // Generate crc32
  rtcData.crc32 = this->calculateCRC32(((uint8_t *)&rtcData) + 4, sizeof(rtcData) - 4);
  if (ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtcData, sizeof(rtcData)))
  {
    Serial.println("Wrote data to RTC");
  }
}

byte DeviceManager::ReadStateFromMemory()
{
  Serial.println("Try to read from RTC");

  if (ESP.rtcUserMemoryRead(0, (uint32_t *)&rtcData, sizeof(rtcData)))
  {
    uint32_t crc = this->calculateCRC32(((uint8_t *)&rtcData) + 4, sizeof(rtcData) - 4);
    if (crc == rtcData.crc32)
    {
      Serial.println("Read from RTC: Sucess");
      this->RtcReadSuccess = true;
    }
    else
    {
      Serial.println("Read from RTC: Failure");
      this->RtcReadSuccess = false;
    }
  }
  this->SetState(rtcData.state);
  Serial.print("GOT RTC Value = ");
  Serial.println(rtcData.state);

  return rtcData.state;
}

byte DeviceManager::GetCurrentState()
{
  return rtcData.state;
}

void DeviceManager::ConnectWifi()
{

  WiFi.mode(WIFI_STA);
  Serial.print("SSID NAME: ");
  Serial.println(this->_config.GetWifiSsid());
  Serial.print("SSID Password: ");
  Serial.println(this->_config.GetWifiPassword());
  // WiFi.config(this->staticAdress, gateway, subnet, dns);
  if (this->RtcReadSuccess)
  {
    Serial.println("Try Quick Connect");
    // The RTC data was good, make a quick connection
    WiFi.begin(this->_config.GetWifiSsid(),this->_config.GetWifiPassword(),rtcData.channel, rtcData.bssid, true);
  }
  else
  {
    Serial.println("Try NON Quick Connect");
    // The RTC data was not valid, so make a regular connection
    WiFi.begin(this->_config.GetWifiSsid(), this->_config.GetWifiPassword());
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  // Copy 6 bytes of BSSID (AP's MAC address)
  memcpy(rtcData.bssid, WiFi.BSSID(), 6);
  rtcData.channel = WiFi.channel();
  WriteRtcSettings();
  WiFi.printDiag(Serial);
}
OperatingStates DeviceManager::GetOperatingState()
{
  int result = digitalRead(GPIO_MAINTENANCE_PIN);
  if (result == HIGH)
  {
    return OperatingStates::Maintenance;
  }
  return OperatingStates::Operating;
}
