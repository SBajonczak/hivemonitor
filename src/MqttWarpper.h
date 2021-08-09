#ifndef MqttWrapper_H
#define MqttWrapper_H
#include <AsyncMqttClient.h>

class MqttWrapper
{

  const int delayBetweenSamplesInSeconds = 3;
  const int amountWeightSamples = 10;

public:
  MqttWrapper(char *Topic);
  ~MqttWrapper();
  uint16_t Send(char *message);
private:
  char *Topic;
};

#endif