#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Arduino.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "ConfigWebserver.h"
#include "ConfigurationManager.h"
#include "html\html_ui.h"

#define AP_SSIDNAME "HiveMonitor"
#define AP_PASSWORD "B33$"

AsyncWebServer server(80);
AsyncCallbackJsonWebHandler *storeConfigHandler = new AsyncCallbackJsonWebHandler("/store", [](AsyncWebServerRequest *request, JsonVariant &json)
                                                                                  {
                                                                                    Serial.print("Got Setting:");

                                                                                    String values;
                                                                                    serializeJson(json, values);
                                                                                    Serial.println(values);
                                                                                    ConfigurationManager::getInstance()->ApplyJsonInput(values);
                                                                                    ConfigurationManager::getInstance()->StoreSettings();
                                                                                    ConfigurationManager::getInstance()->ReadSettings();
                                                                                    request->send(200);
                                                                                  });

ConfigWebserver::ConfigWebserver() {}
ConfigWebserver::~ConfigWebserver() {}

void ConfigWebserver::Serve()
{
  WiFi.softAP(AP_SSIDNAME, AP_PASSWORD);
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
  server.addHandler(storeConfigHandler);

  // server.on("/tarestep0", HTTP_POST, [](AsyncWebServerRequest *request)
  //           {
  //             WeightProcessor *scaledevice = WeightProcessor::getInstance(GPIO_HX711_DT, GPIO_HX711_SCK);
  //             if (scaledevice->getWeight())
  //             {
  //               float offset = scaledevice->getWeight(0);
  //               Serial.println("Offset");
  //               Serial.println(offset);
  //               request->send(200);
  //             }
  //             else
  //             {
  //               Serial.println("Not available");
  //               request->send(500, "text"
  //                                  "Scale not ready or connected!");
  //             }
  //           });

  // server.on(
  //     "/tarestep1", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
  //     [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

  //     });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", PAGE_index, PAGE_index_L);
              response->addHeader(F("Cache-Control"), "no-store,max-age=0");
              // Gzip content
              response->addHeader(F("Content-Encoding"), "gzip");
              request->send(response);
            });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              ConfigurationManager *scaledevice = ConfigurationManager::getInstance();
              ConfigurationManager::getInstance()->ReadSettings();
              String settings = scaledevice->GetJson();
              Serial.println("Settings: ");
              Serial.println(settings);
              request->send(200, "application/json", settings);
            });

  server.begin();
}
