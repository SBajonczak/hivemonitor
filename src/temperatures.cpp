#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "temperature.h";

int _oneWirePin;
int _connectedDevices;
OneWire oneWire();
DallasTemperature sensors();
TemperatureProcessor::TemperatureProcessor(int onwWirePin, int connectedDevices)
{
    this->_oneWirePin = onwWirePin;
    this->_connectedDevices = connectedDevices;
}

void TemperatureProcessor::setup()
{
    // Setup a OneWire instance to communicate with any OneWire devices
    // Pass our OneWire reference to Dallas Temperature.
    oneWire.begin(this->_oneWirePin);
    sensors.setOneWire(&this->oneWire);
}

float TemperatureProcessor::getTemperature(int devicenumber)
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