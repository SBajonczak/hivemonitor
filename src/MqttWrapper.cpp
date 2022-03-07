#include "MqttWrapper.h"
#include "ConfigurationManager.h"
#include "message.h"
#include <ArduinoJson.h>
#include "AzureCredentialManager.h"

// Set helper method
#define MQTT_PACKET_SIZE 1024

#define DEVICE_ID "DEV-DEVICE-SBA"
#define MQTT_PACKET_SIZE 1024
#define MQTT_SERVER "sba-iot-hub.azure-devices.net"
#define MQTT_PORT 8883

// static X509List cert((const char *)ca_pem);
void MqttWrapper::onMqttConnect()
{

    MqttWrapper::messages.empty();
}




void MqttWrapper::Setup()
{
    Serial.println("Set Client");
}

bool MqttWrapper::IsConnected()
{
    return this->connected;
}
void MqttWrapper::Send(String msg)
{
    if (_client.connected())
    {
        String topic = this->_credentialManager.GetTopicName();
        Serial.print("Topic: ");
        Serial.println(topic);
        _client.publish(topic.c_str(), msg.c_str(), false);

        _client.loop();
    }
}

void MqttWrapper::Connect()
{

    // this->_client.setTrustAnchors(&cert);
    _client.setBufferSize(MQTT_PACKET_SIZE);
    _client.setServer(MQTT_SERVER, MQTT_PORT);
    yield();
    String sas_Token = this->_credentialManager.GenerateSasToken();
    Serial.print("Sas:");
    Serial.println(sas_Token);
    yield();

    if (sas_Token != "")
    {
        String username = this->_credentialManager.GetUserName();

        if (!_client.connected())
        {
            yield();
            Serial.println("Connecting Pubsub client...");
            yield();
            if (_client.connect(DEVICE_ID, username.c_str(), sas_Token.c_str()))
            {
                Serial.println("Pubsub  Connected");
            }
            else
            {

                // this->connected = false;
                Serial.print("failed, rc=");
                Serial.print(_client.state());
                Serial.println("Pubsub not Connected");
            }
        }
    }
}

MqttWrapper::MqttWrapper(char *deviceID, PubSubClient client)
{
    this->_client = client;
    this->deviceID = deviceID;
    AzureCredentialManager manager;
}

MqttWrapper::~MqttWrapper()
{
}