#include <DeviceManager.h>
#include "TareUtility.h"
#include "ConfigurationManager.h"
#include <ESP8266WiFi.h>

void DeviceManager::setup() {}
char DeviceManager::_deviceId[]; // need to define the static variable
#define WIFI_SSID "Ponyhof"
#define WIFI_PASSWORD "Tumalonga2411"
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

DeviceManager::DeviceManager()
{
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

void DeviceManager::GotToSleep(WakeMode mode)
{
  ESP.deepSleep(5 * 1000000, mode);
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
  Serial.print("Sleep now for ");
  // Serial.print(ESP.deepSleepMax() * 1000000);
  Serial.println(" seconds");
  ESP.deepSleep(ESP.deepSleepMax() * 1000000);
}

void DeviceManager::SetStateToMemory(byte value)
{
  this->SetState(value);
  byte buf[2]; //__attribute__((aligned(4)));
  buf[0] = value;
  system_rtc_mem_write(RTC_STATE, buf, 1);

  Serial.print("New RTC Value = ");
  Serial.println(*buf);
}

void DeviceManager::SetStateAndMagicNumberToMemory()
{
  byte buf[2];   //__attribute__((aligned(4)));
  buf[0] = 0x55; // 85
  buf[1] = 0xaa; // 170
  // set and write the magic number
  system_rtc_mem_write(RTC_BASE, buf, 2);
}

bool DeviceManager::IsColdstart()
{
  byte buf[2]; // __attribute__((aligned(4)));
  // set and write the magic number
  system_rtc_mem_read(RTC_BASE, buf, 2);
  this->SetStateAndMagicNumberToMemory();
  return (buf[0] != 0x55) || (buf[1] != 0xaa);
}

void DeviceManager::SetState(byte state)
{
  this->CurrentState = state;
}

byte DeviceManager::ReadStateFromMemory()
{
  byte buf[2]; // __attribute__((aligned(4)));
  system_rtc_mem_read(RTC_STATE, buf, 1);
  this->SetState(buf[0]);
  Serial.print("GOT RTC Value = ");
  Serial.println(*buf);

  return buf[0];
}

byte DeviceManager::GetCurrentState()
{
  return this->CurrentState;
}
void DeviceManager::ConnectWifi()
{

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Conecting to wifi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
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
