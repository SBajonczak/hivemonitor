#ifndef MqttWrapper_H
#define MqttWrapper_H
#include <AsyncMqttClient.h>
#include "message.h"

class MqttWrapper
{

public:
  MqttWrapper();
  ~MqttWrapper();
  void onMqttConnect();
  void Queue(char* Topic, char* msg);
  void Send();
  void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
private:
  char* message;
  char* Topic;
  bool connected;
    static std::vector<Message*> messages;
};

#endif