#include <Homie.h>
#include <MeasureHandler.h>

/*
  * Scale should be calibrated with a regulated input of 3.3V!
  * Every change during runtime will cause wrong readings
  */
const float LOW_BATTERY = 1.1;

HomieNode Nodeweight("weight",  "switch");
HomieNode Nodetemperature0("temperature0",  "switch");
HomieNode Nodetemperature1("temperature1",  "switch");
HomieNode Nodebattery("battery",  "switch");
HomieNode NodebatAlarm("battery",  "switch");
HomieNode Nodejson("data",  "switch"); //Hiveeyes.org compatibility format


MeasureHandler::MeasureHandler()
{
}

void MeasureHandler::setupHandler()
{
}

void MeasureHandler::SetWeightValue(float weightValue)
{
  this->weight = weightValue;
}

void MeasureHandler::SetLowBattery(bool isLow)
{
  this->lowBattery = isLow;
}

bool MeasureHandler::GetLowBattery()
{
  return this->lowBattery;
}

void MeasureHandler::SetTemperatureValue(int index, float temperature)
{
  switch (index)
  {
  case 0:
    this->temperature0 = temperature;
    break;
  case 1:
    this->temperature1 = temperature;
    break;
  }
}

float MeasureHandler::GetTemperaturValue(int index)
{
  switch (index)
  {
  case 0:
    return this->temperature0;
  case 1:
    return this->temperature1;
  }
  return 0;
}

void MeasureHandler::setup()
{

  Nodeweight.setProperty("unit").send("kg");
  Nodetemperature0.setProperty("unit").send("C");
  Nodetemperature1.setProperty("unit").send("C");
  Nodebattery.setProperty("unit").send("V");
  NodebatAlarm.setProperty("bool");
}
void MeasureHandler::AdvertiseNodes()
{
  Nodeweight.advertise("unit");
  Nodeweight.advertise("kilogram");
  Nodetemperature0.advertise("unit");
  Nodetemperature0.advertise("degrees");

  Nodetemperature1.advertise("unit");
  Nodetemperature1.advertise("degrees");

  Nodebattery.advertise("unit");
  Nodebattery.advertise("volt");
}

void MeasureHandler::SetVoltage(float voltage)
{

  Serial.println("Got Voltage");
  this->voltage = voltage;
}

void MeasureHandler::SubmitData()
{

  String values;

  //debug MTU
  delay(100);
  // Homie.getLogger() << "Weight: " << weight << " kg" << endl;
  Nodeweight.setProperty("kilogram").setRetained(false).send(String(weight));

  Homie.getLogger() << "Temperature0: " << temperature0 << " °C" << endl;
  Nodetemperature0.setProperty("degrees").setRetained(false).send(String(temperature0));

  Homie.getLogger() << "Temperature1: " << temperature1 << " °C" << endl;
  Nodetemperature1.setProperty("degrees").setRetained(false).send(String(temperature1));

  Homie.getLogger() << "Corrected Input Voltage: " << voltage << " V" << endl;
  Nodebattery.setProperty("volt").setRetained(true).send(String(voltage));
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject &root = jsonBuffer.createObject();
  root["Weight"] = round2two(this->weight);
  root["Temp1"] = round2two(temperature0);
  root["Temp2"] = round2two(temperature1);
  root["VCC"] = round2two(voltage);
  root.printTo(values);
  //root.printTo(values);
  // Homie.getLogger() << "Json data:" << values << endl;
  Nodejson.setProperty("__json__").setRetained(false).send(values);

  if (this->voltage <= LOW_BATTERY)
  {
    NodebatAlarm.setProperty("alarm").setRetained(true).send("true");
    // SLEEP_TIME = 0;
    // Homie.getLogger() << F("✖✖✖ Battery critically low, will sleep forever ✖✖✖") << endl;
  }
  else
  {
    NodebatAlarm.setProperty("alarm").setRetained(true).send("false");
    // SLEEP_TIME = sleepTimeSetting.get();
    // Homie.getLogger() << "✔ Preparing for " << SLEEP_TIME << " seconds sleep" << endl;
  }
}

float MeasureHandler::GetVoltage()
{
  return this->voltage;
}

float MeasureHandler::round2two(float tmp)
{
  return ((int)(tmp * 100)) / 100.00;
}
