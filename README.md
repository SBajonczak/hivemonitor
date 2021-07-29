![image](https://github.com/SBajonczak/hivemonitor/workflows/PlatformIO%20CI/badge.svg)
![image](https://img.shields.io/github/v/tag/SBajonczak/hivemonitor.svg)

## :ledger: Index

- [About](#beginner-about)
- [Technologies](#zap-technologies)
  - [Configured Platforms](#configured_platforms)
  - [Development](#package-development)
  - [Pre-Requisites](#notebook-pre-Requisites)
  - [Branches](#cactus-branches)
- [Development Environment](#nut_and_bolt-development_environment)
  - [Build the Firmware](#hammer-build_the_firmware)
  - [Upload firmware](#rocket-upload_the__firmware)
  - [Upload the configuration](#page_facing_up-upload_the_configuration)  
- [Hardwaresetup](#electric_plug-hardwaresetup)
  - [Components List](#components_List)
  - [Wiring Schema](#wiring_schema)
  - [Wiring H30A to HX711](#wiring_h30a_to_hx711)
- [Configuration](#configuration)
  - [Upload_Configuration](#upload_configuration)
- [Planned Features](#planned_features)
# :beginner: About
This Software is build for Beekeepers, to collect some measurements and allows you to get some insights in your hive. 

With this you can automatically measure: 

* The Weight
* The Temperature (outside and inside the Hive) 
* The Battery Voltage

Also it transmits to any MQTT Server. By using the deep sleep mode, you will can use it nearly several weeks.
In addition with a solar cell, you are free to use without a powerplug!

You can Update your device by connecting it to your pc, or you use the OTA method.

# Technologies
This project will not work alone, so it has dependencies. 

## Configured Platforms
Actually it was Build and Testet on the following devices

* D1_Mini
* D1 Mini Huzzah
* ESP 8266 Devboard

## :package: Development


## :notebook:Pre-Requisites
This Software is based on the [Homie for ESP8266](https://github.com/marvinroger/homie-esp8266) Framework.

The following libaries are required:

* [HX711](https://github.com/bogde/HX711)
* [RunningMedian](https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian)    
* [Homie-esp8266](https://github.com/marvinroger/homie-esp8266)
* ArduinoJson@^5.13.4
* OneWire@^2.3.5
* DallasTemperature@^3.9.1
## :cactus: Branches
Here some description about the used branches

|Branch|Description|
|-|-|
|master|This contains the latest stable version|
|development|In this branch, I will put every development work for now. This branch is __NOT__ stable|


# :nut_and_bolt: Development Environment
First of all, the follwing commands required an installation of Platform IO. You can install it with the follwoing command: 

```bash
setup-virtualenv
```

## :hammer: Build the Firmware
You can Build your firmware very easyly with the following command:

```bash
make build-complete
```

## :rocket: Upload the firmware
After a successfull build you can upload it to your connected device with: 

```bash
make upload-firmware
```
In some cases you have multiple device connected. In this case, you must set the port to upload before with this command:

```bash
export PLATFORMIO_UPLOAD_PORT=/dev/ttyUSB0
```

This upload the firmware throught the /dev/ttyUSB0 port.

## :page_facing_up: Upload the configuration
```bash
export PLATFORMIO_UPLOAD_PORT=/dev/ttyUSB0
make upload-config
```

# :electric_plug:  Hardwaresetup 
The following section will describe the hardware construction

## Components List
I Ordered the following parts from my local seller

* 1 x [Hx711](https://www.amazon.de/Verst%C3%A4rker-Gewichtssensoren-Anzeige-Modul-Digital-Konverter-Mikrocontroller/dp/B07MTYT95R/ref=asc_df_B07MTYT95R)
* 1 x Solarcell (If you use the POWER of the sun :))
* 1 x Lipo with 3.7 Voltage and 1200 mA 
* 1 x [H30A](https://www.bosche.eu/waagenkomponenten/waegezellen/plattform-waegezellen/plattform-waegezellen-h30a?gclid=Cj0KCQiAm4TyBRDgARIsAOU75sopW-AKahFClFZDqvMqvNG2f-zCtpa3S802P4ADE2INFBI6E7GvkkcaAq4JEALw_wcB) Weight cell (from Bosche)
* 1 x Wemos D1 mini Huzzah
* 1 x [DS18B20](https://www.sensorshop24.de/temperaturfuehler/luftfuehler/luftfuehler-bis-105-c/luftfuehler-bis-105-c-15df65d/a-96748/?ReferrerID=7&gclid=Cj0KCQiAm4TyBRDgARIsAOU75sqCkTF0dII80wNX8DOfFXFGCFUWQIpZqPubA2ZrfuUDXD-CR1SEdg4aAiVcEALw_wcB)
* 1 x [S8050](http://media.nkcelectronics.com/datasheet/s8050.pdf) as minimal switch curcuit.
* 1 x 210R
* 1 x  10K 
* 1 x TP4056

## Wiring Schema
Together you can wire it up lik this schematic circuit:
![Circuit](./wiring.png)

## Wiring H30A to HX711

I was sometimes confused about the wiring of the loadcell to the HX711. So I decided to write down the wirings here to:

[Wiring](./h30A_hx711.png)


# Configuration 
The Configuration is done with an json file. An example of it looks like this:

```json
{
	"wifi":	{
		"ssid": "SSID",
		"password": "mysecret"
	},
	"mqtt":	{
		"host": "192.168.178.55",
		"port": 1886,
		"base_topic": "",
		"auth": true,
		"username": "username",
		"password": "password"
	},
	"name":"hive-teststand",
	"ota": {
		"enabled":false
	},
	"device_id":"hive-teststand",
	"settings": {
		"sleepTime": 3600,
    	"sendInterval": 320,
        "weightOffset": 244017.00,
        "kilogramDivider": 22.27
  	}
}
```
The following table will give you more insights about the settings.


|Setting|Description|
|-|-|
|wifi.sid|the wifis name.|
|wifi.password|The password for the wifi|
|mqtt.host|The host for the mqtt. This can be a dns name or a ip address.|
|mqtt.port|The Mqtt Port.|
|mqtt.base_topic|If there is a prefix required for a topic, then you can set here the basetopic.|
|mqtt.auth|__true__ when using authentication, or __false__ when not using it|
|mqtt.username|The username when authentication is configured.|
|mqtt.password|The password when authentication is configured.|
|name|This is the device name. This will shown up in the router later|
|ota.enabled|__true__ to enable _o_ver _t_he _a_ir update.|
|device_id|The device ID, possible the same as name.|
|settings.sleepTime|The Sleepingtime in seconds|
|settings.sendInterval|The intervall to send, the sleep time will be different. When you have a send intervall from 60 seconds and a weight intervall of 90 seconds it will take 2 rounds of wake up|
|settings.weightOffset|The weight offset. This is neccessary for the adjustment of the weightcell. Because there are some base weights.|
|settings.kilogramDivider|The divider to get the kilograms.|


## Upload_Configuration
After you modified the configuration to your setting, you can upload this to your device.

```bash
 make upload-config
```

# Planned Features
I use this devices for my hives to. So I am not ready yet. 

For future Releases I planning to improve some thing and extend it with the following planned features:

* Support Lora
* Adding custom Webinterface to Configure the Device within the wireles connection
* Adding Maintenance Mode
(Especcially when you work at the bees)


Feel free to suggest me more Features.
