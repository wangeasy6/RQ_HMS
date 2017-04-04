# RQ_HMS
### Made a home monitoring system with Raspberry Pi and QT.

## two folders:

*    /linux store the code on the Raspberry Pi.
*	 /QT shore the code on the QT.

## Raspberry Pi physical connection:

* DTH11 sensor monitor temperature and humidity,Connected to WiringPi_GPIO_1
* Infrared sensor monitor pass and into,Connected to WiringPi_GPIO_4
* Camera connected to USB.

## The system is still under development

* Makefile , Use files := $(foreach n,$(names),$(n).o)
* Linux , Logical upgrade
* Linux , Add camera
* QT , Communication and Interface
