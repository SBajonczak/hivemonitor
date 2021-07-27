#include <Homie.h>
#include "BatteryProcessor.h"

float adjust;

BatteryProcessor::BatteryProcessor()
{
}

float BatteryProcessor::getVolt()
{
    this->lastvoltage = analogRead(A0);
    return this->lastvoltage;
}

bool BatteryProcessor::IsLowWarning()
{
    if (this->lastvoltage < 0)
        this->getVolt();
    return this->lastvoltage < BATT_WARNING_VOLTAGE;
}
bool BatteryProcessor::IsLow()
{
    if (this->lastvoltage < 0)
        this->getVolt();
    return this->lastvoltage < LOW_BATTERY;
}