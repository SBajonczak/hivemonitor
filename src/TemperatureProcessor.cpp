#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "TemperatureProcessor.h"

int _oneWirePin;
int _connectedDevices;

OneWire oneWire;
DallasTemperature sensors;

TemperatureProcessor::TemperatureProcessor(int onwWirePin)
{
    this->_oneWirePin = onwWirePin;
    this->setup();
    this->_connectedDevices = this->getDeviceCount();
}

void TemperatureProcessor::setup()
{
    // Setup a OneWire instance to communicate with any OneWire devices
    // Pass our OneWire reference to Dallas Temperature.
    oneWire.begin(this->_oneWirePin);
    sensors.setOneWire(&this->oneWire);
    
    sensors.begin();
}

int TemperatureProcessor::getDeviceCount()
{
    return this->_connectedDevices;
}

float TemperatureProcessor::getTemperature(int devicenumber)
{
    if (this->getDeviceCount() > 0)
    {
        sensors.requestTemperatures();                             // Send the command to get temperatures from all devices
        float temperature = sensors.getTempCByIndex(0); // Read the temp from the specific device
        if (temperature == DEVICE_DISCONNECTED_C)
        {

            return -1; // Device not connected.
        }
        return temperature;
    }
    return 0;
}
