#include <AsyncMqttClient.h>
#include "MqttWrapper.h"
#include "ConfigurationManager.h"
#include "message.h"
#include <ArduinoJson.h>
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
    String result(value, 2);
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

void MqttWrapper::Queue(char *Topic, String msg)
{
    Message *data = new Message(Topic, msg);
    Serial.println(data->msg);
}

void MqttWrapper::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Serial.print("MQTT Disconnect Reason: ");
    Serial.println((int)reason);
}

void MqttWrapper::Send()
{
    mqttClient.setServer(ConfigurationManager::getInstance()->GetMqttServer().c_str(), ConfigurationManager::getInstance()->GetMqttPort());
    String username;

    username += ConfigurationManager::getInstance()->GetMqttServer();
    username += "/";
    username += ConfigurationManager::getInstance()->GetMqttUser();//this->deviceID;
    // username +="/";
    // username +="HIVE-SENSORS";
    username += "/?api-version=2018-06-30";
    
    Serial.print("using Username: ");
    Serial.println(username);
    mqttClient.setClientId(this->deviceID);
    mqttClient.setCredentials(ConfigurationManager::getInstance()->GetMqttUser().c_str(), "SharedAccessSignature sr=sba-iot-hub.azure-devices.net%2Fdevices%2FBETA-HIVE&sig=k1zj6j5iLqyf7dve54SlzGwRh565%2BAwvgsgy93rzXx8%3D&se=1677847336");
    mqttClient.onConnect(std::bind(&MqttWrapper::onMqttConnect, this));
    mqttClient.onDisconnect(std::bind(&MqttWrapper::onMqttDisconnect, this, std::placeholders::_1));
    mqttClient.connect();
    // Must be called to gain a connection... think this is a bug in the asyncmqtt lib.
    while (!mqttClient.connected())
    {
        delay(500);
    }
    Serial.println("Connected");
}

MqttWrapper::MqttWrapper(char *deviceID)
{
    this->deviceID = deviceID;
}

MqttWrapper::~MqttWrapper()
{
}