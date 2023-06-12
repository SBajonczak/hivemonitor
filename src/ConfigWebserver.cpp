#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Arduino.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "TemperatureProcessor.h"
#include "WeightProcessor.h"
#include "ConfigWebserver.h"
#include "ConfigurationManager.h"
#include "html\html_ui.h"

#define AP_SSIDNAME "HiveMonitor"
#define AP_PASSWORD "B33$"
DNSServer dnsServer;
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
                                                                                    request->send(200); });

ConfigWebserver::ConfigWebserver()
{
  this->_isServing = false;
}
ConfigWebserver::~ConfigWebserver() {}

bool ConfigWebserver::IsServing()
{
  return this->_isServing;
}


void ConfigWebserver::Loop()
{
     Serial.printf("Is serving %i" , this->IsServing());

  if (this->IsServing()){
     Serial.print("process next request");
     dnsServer.processNextRequest();
  }
}

void ConfigWebserver::Serve()
{
  const IPAddress apIP = IPAddress(1, 2, 3, 4);
  static const byte DNS_PORT = 53;
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(AP_SSIDNAME);
 
  dnsServer.start(DNS_PORT, "*", apIP);

  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Origin"), "*");
  DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Methods"), "*");
  DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Headers"), "*");
  server.addHandler(storeConfigHandler);

  server.on("/getWeightValue", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              WeightProcessor *scaledevice = WeightProcessor::getInstance(GPIO_HX711_DT, GPIO_HX711_SCK);

              float offset = scaledevice->getRawWeight();
              Serial.println("Offset");
              Serial.println(offset);
              request->send(200, "text", String(offset)); });
server.onNotFound([](AsyncWebServerRequest *request)
            {
              AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", PAGE_index, PAGE_index_L);
              response->addHeader(F("Cache-Control"), "no-store,max-age=0");
              // Gzip content
              response->addHeader(F("Content-Encoding"), "gzip");
              request->send(response); });
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", PAGE_index, PAGE_index_L);
              response->addHeader(F("Cache-Control"), "no-store,max-age=0");
              // Gzip content
              response->addHeader(F("Content-Encoding"), "gzip");
              request->send(response); });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              ConfigurationManager *scaledevice = ConfigurationManager::getInstance();
              ConfigurationManager::getInstance()->ReadSettings();
              String settings = scaledevice->GetJson();
              Serial.println("Settings: ");
              Serial.println(settings);
              request->send(200, "application/json", settings); });

  server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              WeightProcessor *weightProcessor = WeightProcessor::getInstance(GPIO_HX711_DT, GPIO_HX711_SCK);
              DynamicJsonDocument doc(512);
              doc["sensors"]["scale"]["connected"]= weightProcessor->DeviceReady();
              String bodyString;
              serializeJson(doc, bodyString);
              request->send(200, "application/json", bodyString); });

  this->_isServing = true;
  server.begin();
}
