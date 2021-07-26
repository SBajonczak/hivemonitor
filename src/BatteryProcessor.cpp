#include <Homie.h>
#include "BatteryProcessor.h"

float adjust;

BatteryProcessor::BatteryProcessor()
{
}

float BatteryProcessor::getVolt()
{
    float aVolatageValue= analogRead(A0);
}
