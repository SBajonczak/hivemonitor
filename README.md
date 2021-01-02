# Hive Monitor Software
This Code represents a Hive Monitoring System for any ESP Hardware (with enough GPIO Ports ;)). 

The Firmware contains the following Features: 


* Measuring
    * The Weight
    * The Temperature (inside the Hive) 
    * The Battery Voltage
* Transmit the Measurement as JSON to an MQTT Server
* Support Update OTA Server (to upload new firmware updates)
* Deep Sleep (to enhance the battery lifetime)



## Dependencies

This Software is based on the [Homie ESP 8266]( https://github.com/marvinroger/homie-esp8266) Framework.
Because this delivery the basic functionality to communicate with a mqtt server and brings it own (easy) setting. 

The following libaries are required:

* HX711:              https://github.com/bogde/HX711
* RunningMedian:      https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian
* Homie-esp8266:      https://github.com/marvinroger/homie-esp8266
* Bounce2:            https://github.com/thomasfredericks/Bounce2
* ESPAsyncTCP:        https://github.com/me-no-dev/ESPAsyncTCP
* async-mqtt-client:  https://github.com/marvinroger/async-mqtt-client


## Hardware 
I created a schematic overview: 
![Circuit](./wiring.png)

In fact I use for now a Wemos d1 mini Huzzah instead of a ESP DEV Board. 

for now I testing it on a breadboard design. If I have a final design, I will create a PCB for this. For now my Setting looks like this:


![rr](./Schematic_Hive Mon V1_2021-01-02.jpg)


## UploDING CONFIGURATIONS

>> pio run -t uploadfs