#ifndef MqttWrapper_H
#define MqttWrapper_H
// #include <AsyncMqttClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "message.h"
#include "AzureCredentialManager.h"

class MqttWrapper
{

public:
  MqttWrapper(char *deviceID, PubSubClient client);
  ~MqttWrapper();
  void onMqttConnect();
  void Queue(String msg);
  void Send(String msg);
  bool IsConnected();
  void Connect();
  // void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
  void Setup();

private:
  AzureCredentialManager _credentialManager;
  PubSubClient _client;
  char *deviceID;
  char *message;
  char *Topic;
  bool connected;
  static std::vector<Message *> messages;
  // static az_iot_hub_client client;
};

#endif