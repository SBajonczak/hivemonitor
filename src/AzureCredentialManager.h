#ifndef AzureCredentialManager_h
#define AzureCredentialManager_h
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h> // Include the SPIFFS library
class AzureCredentialManager
{

public:
    AzureCredentialManager();
    void setup();
    String getClientID();
    String GenerateSasToken();
    String GetUserName();
    String GetTopicName();

private:
};
#endif