#include <AsyncMqttClient.h>

#include "MqttWarpper.h"
#include "ConfigurationManager.h"

AsyncMqttClient mqttClient;

MqttWrapper::MqttWrapper(char *Topic)
{
    if (Topic)
    {
        this->Topic = Topic;
    }
    else
    {
        this->Topic = "/";
    }
    mqttClient.setServer(ConfigurationManager::getInstance()->GetMqttServer().c_str(), ConfigurationManager::getInstance()->GetMqttPort());
    if (
        ConfigurationManager::getInstance()->GetMqttUser().length() > 0 &&
        ConfigurationManager::getInstance()->GetMqttPassword().length() > 0)
    {
        mqttClient.setCredentials(
            ConfigurationManager::getInstance()->GetMqttUser().c_str(),
            ConfigurationManager::getInstance()->GetMqttPassword().c_str());
    }
    mqttClient.connect();
}

MqttWrapper::~MqttWrapper()
{
    mqttClient.disconnect();
}

uint16_t MqttWrapper::Send(char *message)
{
    return mqttClient.publish(this->Topic, 1, true, message);
}