#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "temperature.h"

int _oneWirePin;
int _connectedDevices;

OneWire oneWire();
DallasTemperature sensors();
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
}

int TemperatureProcessor::getDeviceCount()
{
    return this->_connectedDevices;
}

float TemperatureProcessor::getTemperature(int devicenumber)
{
    if (this->getDeviceCount() > 0)
    {
        sensors.begin();
        // call sensors.requestTemperatures() to issue a global temperature
        // request to all devices on the bus
        sensors.requestTemperatures(); // Send the command to get temperatures
        float temperature = sensors.getTempCByIndex(devicenumber);
        if (temperature == -127)
        {
            temperature = sensors.getTempCByIndex(devicenumber);
        }
        return temperature;
    }
    return 0;
}
