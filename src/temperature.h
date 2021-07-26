/*
  Temperatures.h - Lib for gtting weights 
*/
#ifndef TEMPERATURES_h
#define TEMPERATURES_h

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>


class TemperatureProcessor
{
public:
    TemperatureProcessor(int onwWirePin, int connectedDevices);
    void setup();
    float getTemperature(int devicenumber);

private:
    OneWire oneWire;
    DallasTemperature sensors;

    int _oneWirePin;
    int _connectedDevices;
};

#endif