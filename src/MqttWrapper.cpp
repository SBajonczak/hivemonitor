#include <AsyncMqttClient.h>

#include "MqttWrapper.h"
#include "ConfigurationManager.h"
#include "message.h"
AsyncMqttClient mqttClient;

std::vector<Message *> MqttWrapper::messages;

void MqttWrapper::onMqttConnect()
{
    Serial.println("Connected, sending message");
    for (Message *msg : MqttWrapper::messages)
    {
        char internalTopic[255];
        strcpy(internalTopic, "devices/");
        strcat(internalTopic, this->deviceID);
        strcat(internalTopic, msg->Topic);
        Serial.print("Topic:");
        Serial.println(internalTopic);
        Serial.print("Message:");
        Serial.println(msg->msg);
        Serial.println("_____");
        mqttClient.publish(internalTopic, 0, true, msg->msg.c_str());
    }
    MqttWrapper::messages.empty();
}

void MqttWrapper::Queue(char *Topic, float value)
{
    Serial.print("Queue value:");
    String result(value,2);
    Message *m = new Message(Topic, result);
    Serial.println(m->msg);
    MqttWrapper::messages.push_back(m);
}

void MqttWrapper::Queue(char *Topic, int value)
{
    Serial.print("Queue value:");
    String result(value);
    Message *m = new Message(Topic, result);
    Serial.println(m->msg);
    MqttWrapper::messages.push_back(m);
}

void MqttWrapper::Queue(char *Topic, char *msg)
{
    Message *data = new Message(Topic, msg);
    Serial.println(data->msg);
    MqttWrapper::messages.push_back(data);
}

void MqttWrapper::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Serial.print("MQTT Disconnect Reason: ");
    Serial.println((int)reason);
}

void MqttWrapper::Send()
{
    Serial.println("Setting up mqtt connection");
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
    mqttClient.onDisconnect(std::bind(&MqttWrapper::onMqttDisconnect, this, std::placeholders::_1));
    mqttClient.connect();
    // Must be called to gain a connection... think this is a bug in the asyncmqtt lib.
    while (!mqttClient.connected())
    {
        delay(500);
    }
}

MqttWrapper::MqttWrapper(char *deviceID)
{
    this->deviceID = deviceID;
}

MqttWrapper::~MqttWrapper()
{
}