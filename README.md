[COFCON][1]
======
[1]: https://github.com/Schm1tz1/cofcon

Cofcon is a simple PID-based controller for espresso and coffee machines. It only needs an Arduino-Compatible microcontroller, a temperature-sensor and a SSR to control the heating of the espresso machine.
This is not another copy-and-paste-PID, instead the [Arduino-PID-Library][2] by br3ttb are used as these controller seems to work very stable with standard-parameters and allows auto-tuning to achieve a very high temperature stability (about 0.1°C in the case of my Gaggia CC).
The basic idea of the heater control and parts of the code are based on [BBCC by Tim Hirzel][3] which can be found on Arduino Playground, the serial interface code and graphing output should be compatible.
The code is designed in a (more or less) modular way so new modules or even a simple simulation for testing purposes can be selected using preprocessor defines in the main file.
It is possible to control the board vie bluetooth using a HC-xx-modue for your Arduino. A simple control-app has been designed using the [MIT AppInventor2][4], but it is still in development stage. For details on the app see the README-File in the android-directory.

[4]: http://ai2.appinventor.mit.edu

What you need
=============
* an espresso machine where you have access to the heating power connection/thermostat switch (obviously). One warning: You should have the schematics/powering scheme and you should know what you are doing ! Please be careful, always UNPLUG the machine before you open it (yes, really UNPLUG - it needs to be disconnected from the AC line).
* an arduino-compatible microcontroller - tests have been performed with the UNO and NANO but in principle any other controller should do. You only need 3-4 IO-Pins, so even a simple ATTINY-board should be enough.
* one (or more) temperature-sensors - this controller has been running with the digital Dallas DS18B20 and TSIC-306 so far, but feel free to implement any other sensor type and contribute to this project
* a SSR which is capable of switching your load due to the heating. Usually a SSR capable of switching 8-10A at 240V should be fine, but be sure to check the power consumption of you machine before
* a few cables, maybe pull-up/protection resistors depending on the sensors and SSR you are using
* [Arduino-PID-Library][2] installed in your arduino/libraries
* custom libraries depending on the sensors you are using (wire, dallas, tsic) which are available here on github or Arduino Playground

[2]: https://github.com/br3ttb/Arduino-PID-Library
[3]: http://playground.arduino.cc/Main/BarebonesPIDForEspresso

How to get started
==================
* have the libraries needed for this project installed in your arduino/libraries
* cope the cofcon-folder with its sketch files into your arduino-folder
* restart your Arduino-IDE and the project 'cofcon' should show up in your Sketchbook-Menu
* (de)activate the sensors you want to use with the #define-switches of cofcon.ino
* by default, a simple simulation-mode is activated for testing purposes (deactivate by commenting out the #define SIMULATION_MODE)
