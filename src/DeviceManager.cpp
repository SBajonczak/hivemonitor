
#include <DeviceManager.h>

void DeviceManager::setup() {}
DeviceManager::DeviceManager() {}
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
  // Set the next state into the memory.
  this->SetStateToMemory(STATE_SLEEP_WAKE);
  ESP.deepSleep(this->GetSleepTime() * 1000000);
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
  byte buf[2]; //__attribute__((aligned(4)));
  buf[0] = 0x55;
  buf[1] = 0xaa;
  //set and write the magic number
  system_rtc_mem_write(RTC_BASE, buf, 1);
}

bool DeviceManager::IsColdstart()
{
  byte buf[2]; // __attribute__((aligned(4)));
  //set and write the magic number
  system_rtc_mem_write(RTC_BASE, buf, 2);
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