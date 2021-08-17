#ifndef MqttWrapper_H
#define MqttWrapper_H
#include <AsyncMqttClient.h>
#include "message.h"
#include <ESP8266WiFi.h>
class MqttWrapper
{

public:
  MqttWrapper(char* deviceID);
  ~MqttWrapper();
  void onMqttConnect();
  void Queue(char* Topic, char* msg);
  void Queue(char* Topic, float value);
  void Queue(char *Topic, int value);
  void Send();
  void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
private:
  char* deviceID;
  char* message;
  char* Topic;
  bool connected;
    static std::vector<Message*> messages;
};

#endif