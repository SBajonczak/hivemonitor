/*
  Temperatures.h - Lib for gtting weights 
*/
#ifndef BatteryProcessor_H
#define BatteryProcessor_H
class BatteryProcessor
{
public:
    BatteryProcessor();
    void setup();
    float getVolt();

private:
    float adjust;
};

#endif