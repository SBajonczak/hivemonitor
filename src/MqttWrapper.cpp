#include <AsyncMqttClient.h>

#include "MqttWrapper.h"
#include "ConfigurationManager.h"
#include "message.h"

AsyncMqttClient mqttClient;

std::vector<Message *> MqttWrapper::messages;

void MqttWrapper::onMqttConnect()
{
    // this->connected = true;
    Serial.println("Connected, sending message");
    for (Message *msg : MqttWrapper::messages)
    {

        Serial.println(msg->message);
        Serial.println("_____");
        mqttClient.publish(msg->Topic, 0, true, msg->message);
    }
    mqttClient.disconnect();
}

void MqttWrapper::Queue(char *Topic, char *msg)
{
    MqttWrapper::messages.push_back(new Message(Topic, msg));
}

void MqttWrapper::Send()
{
    mqttClient.setServer("192.168.178.201", ConfigurationManager::getInstance()->GetMqttPort());
    if (
        ConfigurationManager::getInstance()->GetMqttUser().length() > 0 &&
        ConfigurationManager::getInstance()->GetMqttPassword().length() > 0)
    {

        mqttClient.setCredentials(
            ConfigurationManager::getInstance()->GetMqttUser().c_str(),
            ConfigurationManager::getInstance()->GetMqttPassword().c_str());
    }
    mqttClient.onConnect(std::bind(&MqttWrapper::onMqttConnect, this));
    mqttClient.connect();

    for (Message *msg : MqttWrapper::messages)
    {

        Serial.println(msg->message);
        Serial.println("_____");
        mqttClient.publish(msg->Topic, 0, true, msg->message);
    }
}

MqttWrapper::MqttWrapper()
{
}
MqttWrapper::~MqttWrapper()
{
}