#include <DeviceManager.h>
#include <ESPAsyncWebServer.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

#include "html\html_ui.h"
#include "TareUtility.h"

AsyncWebServer server(80);

void DeviceManager::setup() {}

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
  byte buf[2];   //__attribute__((aligned(4)));
  buf[0] = 0x55; // 85
  buf[1] = 0xaa; // 170
  //set and write the magic number
  system_rtc_mem_write(RTC_BASE, buf, 2);
}

bool DeviceManager::IsColdstart()
{
  byte buf[2]; // __attribute__((aligned(4)));
  //set and write the magic number
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

OperatingStates DeviceManager::GetOperatingState()
{
  int result = digitalRead(GPIO_MAINTENANCE_PIN);
  if (result == HIGH)
  {
    return OperatingStates::Maintenance;
  }
  return OperatingStates::Operating;
}

void DeviceManager::ServeWebui()
{
  WiFi.softAP("HiveMonitor", "123");
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Origin"), "*");
  DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Methods"), "*");
  DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Headers"), "*");

  server.on("/tarestep0", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              WeightProcessor *scaledevice = WeightProcessor::getInstance(GPIO_HX711_DT, GPIO_HX711_SCK);
              if (scaledevice->DeviceReady())
              {
                float offset = scaledevice->getWeight(0);
                Serial.println("Offset");
                Serial.println(offset);
                 request->send(200);
              }
              else
              {
                Serial.println("Not available");
                request->send(500, "text"
                                   "Scale not ready or connected!");
              }
             
            });

  server.on(
      "/tarestep1", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
      {
        
        // WeightProcessor scaledevice(GPIO_HX711_DT, GPIO_HX711_SCK);
        // if (scaledevice.DeviceReady())
        // {

        //   float offset = scaledevice.getWeight(0);
        //   request->send(200);
        // }
        // else
        // {
        //   request->send(500, "text"
        //                      "Scale not ready or connected!");
        // }
      });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", PAGE_index, PAGE_index_L);
              response->addHeader(F("Cache-Control"), "no-store,max-age=0");
              // Gzip content
              response->addHeader(F("Content-Encoding"), "gzip");
              request->send(response);
            });
  server.begin();
}