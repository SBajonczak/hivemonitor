  #include <Homie.h>
  #include <OneWire.h>
  #include <DallasTemperature.h>
  #include <Wire.h>
  #include "HX711.h"
  #include <RunningMedian.h>
  #include <Ticker.h>

  #define FW_NAME "node-wifi-mqtt-homie-battery";
  #define FW_VERSION "0.10.0"
  const int DEFAULT_SLEEP_TIME = 20;
  #define USE_SCALE true

  //Workaround for https://github.com/bblanchon/ArduinoJson/issues/566
  #define ARDUINOJSON_USE_DOUBLE 0
  //function to round to two decimals
  float round2two(float tmp)
  {
    return ((int)(tmp * 100)) / 100.00;
  }

  // PINs
  #define ONE_WIRE_BUS 14   // D5
  #define DOUT 13           // D7
  #define PD_SCK 12         // D6
  #define TANSISTORSWITCH 4 // D2

  /* 
    Use sketch BeeScale-Calibration.ino to determine these calibration values.
    Set them here or use HomieSetting via config.json or WebApp/MQTT
  */
  const long DEFAULT_WEIGHT_OFFSET = 666;                   // Load cell zero offset.
  const float DEFAULT_KILOGRAM_DIVIDER = 21.59;             // Load cell value per kilogram.
  const float DEFAULT_CALIBRATION_TEMPERATURE = 23.44;      // Temperature at which the scale has been calibrated for Temperature compensation
  const float DEFAULT_CALIBRATION_FACTOR_GRAM_DEGREE = 0.0; // Calibration factor in gram per degree

  /*
  * Scale should be calibrated with a regulated input of 3.3V!
  * Every change during runtime will cause wrong readings
  */
  const float LOW_BATTERY = 1.1;

  /*
  * *Some* ESPs tend to have a too low reading on ESP.getVcc()
  * To get the maximum battery runtime you should check what ADC is reading and compare
  * it with a multimeter between VCC and GND at the ESP. Change the following value to e.g. 0.13
  * or -0.02 in case of a too high reading. Or set it using HomieSetting via config.json or WebApp/MQTT
  */
  const float DEFAULT_VCC_ADJUST = 0.23;

  HomieSetting<long> sleepTimeSetting("sleepTime", "SleepTime in seconds (max. 3600s!), default is 60s");
  HomieSetting<long> weightOffsetSetting("weightOffset", "Offset value to zero.");
  HomieSetting<double> kilogramDividerSetting("kilogramDivider", "Scale value per kilogram.");
  HomieSetting<double> calibrationTemperatureSetting("calibrationTemperature", "Outside Temperature at which the scale has been calibrated");
  HomieSetting<double> calibrationFactorSetting("calibrationFactor", "Calibration Factor in gram per degree. 0.0 to disable adjustment");
  HomieSetting<double> vccAdjustSetting("vccAdjust", "Calibration value for input voltage. See sketch for details.");

  HX711 scale;

  ADC_MODE(ADC_VCC);

  // deep sleep infrastructure
  // Include API-Headers
  extern "C"
  {
  #include "ets_sys.h"
  #include "os_type.h"
  #include "osapi.h"
  #include "mem.h"
  #include "user_interface.h"
  #include "cont.h"
  }

  // state definitions for wake/sleep cycles

  #define STATE_COLDSTART 0
  #define STATE_SLEEP_WAKE 1
  #define STATE_CONNECT_WIFI 4

  // For connection loss detection, timeout in seconds
  #define RUNTIME_MAX 15
  int runtime_s = 0;
  Ticker timeout;

  #define BATT_WARNING_VOLTAGE 3.2 // Voltage for Low-Bat warning

  // RTC-MEM Adresses
  #define RTC_BASE 65
  #define RTC_STATE 66

  // global variables
  byte buf[10] __attribute__((aligned(4)));
  byte state; // state variable
  uint32_t sleepCount;
  uint32_t time1, time2;

  // Temporary buffer
  uint32_t b = 0;
  int i;

  //end deep sleep infrastructure

  RunningMedian WeightSamples = RunningMedian(3);

  float weight;
  float temperature0;
  float temperature1;
  float voltage;
  float raw_voltage;
  float vcc_adjust;
  int SLEEP_TIME;

  HomieNode weightNode("weight", "weight","switch");
  HomieNode temperatureNode0("temperature0", "temperature","switch");
  HomieNode temperatureNode1("temperature1", "temperature","switch");
  HomieNode batteryNode("battery", "volt","switch");
  HomieNode batAlarmNode("battery", "alarm","switch");
  HomieNode jsonNode("data", "__json__","switch"); //Hiveeyes.org compatibility format

  void setupHandler()
  {
    delay(500);
    weightNode.setProperty("unit").send("kg");
    temperatureNode0.setProperty("unit").send("C");
    temperatureNode1.setProperty("unit").send("C");
    batteryNode.setProperty("unit").send("V");
    batAlarmNode.setProperty("bool");
  }

  // Setup a OneWire instance to communicate with any OneWire devices
  OneWire OneWire(ONE_WIRE_BUS);
  // Pass our OneWire reference to Dallas Temperature.
  DallasTemperature sensors(&OneWire);

  void getTemperatures()
  {
    digitalWrite(TANSISTORSWITCH,HIGH);

    sensors.begin();
    // call sensors.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    sensors.requestTemperatures(); // Send the command to get temperatures

    temperature0 = sensors.getTempCByIndex(0);
    if (temperature0 == -127)
    { //ignore wrong reading
      delay(100);
      temperature0 = sensors.getTempCByIndex(0);
    }
    temperature1 = sensors.getTempCByIndex(1);
    if (temperature1 == -127)
    { //ignore wrong reading
      delay(100);
    }
    temperature1 = sensors.getTempCByIndex(1);
  digitalWrite(TANSISTORSWITCH,LOW);
  }

  void getWeight()
  {
    digitalWrite(TANSISTORSWITCH,HIGH);
    scale.begin(DOUT, PD_SCK);
    scale.power_up();
delay(300);
    scale.set_scale(kilogramDividerSetting.get()); //initialize scale
    Homie.getLogger() << "DEBUG: Got kilogramDivider" << kilogramDividerSetting.get() << endl;
    scale.set_offset(weightOffsetSetting.get()); //initialize scale
    Homie.getLogger() << "DEBUG: Got weightOffset: " << weightOffsetSetting.get() << endl;
    Homie.getLogger() << endl
                      << endl;
    Homie.getLogger() << "Re  ading scale, hold on" << endl;
    for (int i = 0; i < 3; i++)
    {
      float WeightRaw = scale.get_units(3);
      //yield();
      Homie.getLogger() << "✔ Raw measurements: " << WeightRaw << " g" << endl;
      WeightSamples.add(WeightRaw);
      delay(500);
      yield();
    }
      scale.power_down();

    digitalWrite(TANSISTORSWITCH,LOW);

    weight = WeightSamples.getMedian();

    //temperature compensation
    Homie.getLogger() << "✔ uncompensated median Weight: " << weight << " g" << endl;
    float calibrationTemperature = calibrationTemperatureSetting.get();
    float calibrationFactor = calibrationFactorSetting.get();
    if (temperature1 < calibrationTemperature)
      weight = weight + (fabs(temperature1 - calibrationTemperature) * calibrationFactor);
    if (temperature1 > calibrationTemperature)
      weight = weight - (fabs(temperature1 - calibrationTemperature) * calibrationFactor);
    Homie.getLogger() << "✔ compensated median Weight: " << weight << "g" << endl;

    weight = weight / 1000; // we want kilogram

  }

  void getVolt()
  {
    raw_voltage = ESP.getVcc();
    vcc_adjust = vccAdjustSetting.get();
    Homie.getLogger() << "Voltage adjust value is: " << vcc_adjust << "V" << endl;
    voltage = raw_voltage / 1000 + vcc_adjust;
  }

  void transmit()
  {
    //debug MTU
    delay(100);
    Homie.getLogger() << "Weight: " << weight << " kg" << endl;
    weightNode.setProperty("kilogram").setRetained(false).send(String(weight));

    Homie.getLogger() << "Temperature0: " << temperature0 << " °C" << endl;
    temperatureNode0.setProperty("degrees").setRetained(false).send(String(temperature0));

    Homie.getLogger() << "Temperature1: " << temperature1 << " °C" << endl;
    temperatureNode1.setProperty("degrees").setRetained(false).send(String(temperature1));

    Homie.getLogger() << "Raw Input Voltage: " << raw_voltage / 1000 << " V" << endl;
    Homie.getLogger() << "Corrected Input Voltage: " << voltage << " V" << endl;
    batteryNode.setProperty("volt").setRetained(true).send(String(voltage));
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject(); 
    root["Weight"] = round2two(weight);
    root["Temp1"] = round2two(temperature0);    
    root["Temp2"] = round2two(temperature1);
    root["VCC"] = round2two(voltage);
    String values;
    root.printTo(values);
    Homie.getLogger() << "Json data:" << values << endl;
    jsonNode.setProperty("__json__").setRetained(false).send(values);

    /*
      * determine low battery, sleep forever to save lithium cell.
      * we could go lower, but scale is calibrated at 3.3V so we
      * would get wrong readings.
      */
    if (voltage <= LOW_BATTERY)
    {
      batAlarmNode.setProperty("alarm").setRetained(true).send("true");
      SLEEP_TIME = 0;
      Homie.getLogger() << F("✖✖✖ Battery critically low, will sleep forever ✖✖✖") << endl;
    }
    else
    {
      batAlarmNode.setProperty("alarm").setRetained(true).send("false");
      SLEEP_TIME = sleepTimeSetting.get();
      Homie.getLogger() << "✔ Preparing for " << SLEEP_TIME << " seconds sleep" << endl;
    }
  }

  void max_run()
  {
    ++runtime_s;
    if (runtime_s == RUNTIME_MAX)
    {
      Homie.getLogger() << "DEBUG: Max. runtime of " << RUNTIME_MAX << "s reached, shutting down!" << endl;
      /*
      buf[0] = STATE_SLEEP_WAKE;
      system_rtc_mem_write(RTC_STATE, buf, 1); // set state for next wakeUp
      ESP.deepSleep(SLEEP_TIME*1000000, WAKE_RF_DISABLED);
      */
      SLEEP_TIME = sleepTimeSetting.get();
      Homie.getLogger() << "✔ Preparing for " << SLEEP_TIME << " seconds sleep" << endl;
      Homie.prepareToSleep();
    }
  }

  void onHomieEvent(const HomieEvent &event)
  {
    switch (event.type)
    {
    case HomieEventType::NORMAL_MODE:

      timeout.attach(1.0, max_run);
      // getTemperatures();
  #if USE_SCALE
      getWeight();
  #endif
      getVolt();
      Homie.getLogger() << "DEBUG: After measurements: " << millis() / 1000 << endl;
      break;

    case HomieEventType::WIFI_DISCONNECTED:
      Homie.getLogger() << "Wifi disconnected" << endl;
      break;

    case HomieEventType::MQTT_READY:
      //debug MTU?
      delay(100);

      transmit();
      Homie.getLogger() << "DEBUG: After transmit(): " << millis() / 1000 << endl;
      Homie.getLogger() << "✔ Data is transmitted, waiting for package acks..." << endl;
      //max_run();
      delay(100);
      Homie.prepareToSleep();
      break;

    case HomieEventType::READY_TO_SLEEP:

      Homie.getLogger() << "DEBUG: Total runtime: " << millis() / 1000 << endl;
      buf[0] = STATE_SLEEP_WAKE;
      system_rtc_mem_write(RTC_STATE, buf, 1); // set state for next wakeUp
      ESP.deepSleep(SLEEP_TIME * 1000000);
      Homie.getLogger() << "Sleeping now!" << endl;
      break;
    }
  }

  void loopHandler()
  {
  }

  const int sleepSeconds = 5;

  void setup()
  {
    Homie.disableResetTrigger();
    Homie.disableLedFeedback(); // collides with Serial on ESP07

    WiFi.forceSleepBegin();                // send wifi directly to sleep to reduce power consumption
    system_rtc_mem_read(RTC_BASE, buf, 2); // read 2 bytes from RTC-MEMORY to get our state
    Serial.begin(115200);
    Homie.getLogger() << endl;
    //                   << endl;

    if ((buf[0] != 0x55) || (buf[1] != 0xaa)) // cold start, magic number is not present in rtc
    {
      state = STATE_COLDSTART;
      buf[0] = 0x55;
      buf[1] = 0xaa;                          //set and write the magic number
      system_rtc_mem_write(RTC_BASE, buf, 2); //set and write the magic number
    }
    else // reset was due to sleep-wake cycle
    {
      system_rtc_mem_read(RTC_STATE, buf, 1);
      state = buf[0];
    }

    // // now the restart cause is clear, handle the different states
    Serial.printf("State: %d\r\n", state);
    Serial.printf("State STATE_COLDSTART : %d\r\n", STATE_COLDSTART);
    Serial.printf("State STATE_SLEEP_WAKE: %d\r\n", STATE_SLEEP_WAKE);
    Serial.printf("State STATE_CONNECT_WIFI: %d\r\n", STATE_CONNECT_WIFI);

    switch (state)
    {
    case STATE_SLEEP_WAKE:
    case STATE_COLDSTART: // first run after power on - initializes
      // prepare to activate wifi
      buf[0] = STATE_CONNECT_WIFI; // one more sleep required to to wake with wifi on
      WiFi.forceSleepWake();
      WiFi.mode(WIFI_STA);
      system_rtc_mem_write(RTC_STATE, buf, 1); // set state for next wakeUp

      ESP.deepSleep(sleepSeconds * 1000000, WAKE_RFCAL);

      break;
    case STATE_CONNECT_WIFI:
      Homie.onEvent(onHomieEvent);
      sleepTimeSetting.setDefaultValue(DEFAULT_SLEEP_TIME);
      /*
      * Can't set more than one value to default, see issue #323
      * https://github.com/marvinroger/homie-esp8266/issues/323
      */
      weightOffsetSetting.setDefaultValue(DEFAULT_WEIGHT_OFFSET);
      kilogramDividerSetting.setDefaultValue(DEFAULT_KILOGRAM_DIVIDER);
      vccAdjustSetting.setDefaultValue(DEFAULT_VCC_ADJUST);
      calibrationTemperatureSetting.setDefaultValue(DEFAULT_CALIBRATION_TEMPERATURE);
      calibrationFactorSetting.setDefaultValue(DEFAULT_CALIBRATION_FACTOR_GRAM_DEGREE);

      Homie.getLogger() << "DEBUG: Turn on WIFI: " << millis() / 1000 << endl;
      WiFi.forceSleepWake();
      delay(500);
      wifi_set_sleep_type(MODEM_SLEEP_T);
      Homie_setFirmware(FW_NAME, FW_VERSION);
      Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
      weightNode.advertise("unit");
      weightNode.advertise("kilogram");
      temperatureNode0.advertise("unit");
      temperatureNode0.advertise("degrees");

      temperatureNode1.advertise("unit");
      temperatureNode1.advertise("degrees");

      batteryNode.advertise("unit");
      batteryNode.advertise("volt");

      Homie.getLogger() << "DEBUG: Before setup(): " << millis() / 1000 << endl;

      Homie.setup();
      break;
    }

    Serial.println("\n\nWake up");
  }

  void loop()
  {
    Homie.loop();
  }