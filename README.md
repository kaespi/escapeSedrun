# escapeSedrun

This is a project containing code for running an escape game in the Cevi Muttenz Sola 2019 in Sedrun. The project contains many subparts such as
* wifiButtons
  * wifi_rx


## wifiButtons

In this project we want to connect buttons through WiFi using ESP-01 ESP8266 modules to activate a mechanism if pressed in the right order. The reason for using WiFi here is to get rid of the nasty cabling and to have a more fancy solution.

This project consists of multiple transmitters sensing the buttons (wifi_tx) and one receiver, or host (wifi_rx).

All projects are designed for the [Arduino framework](https://www.arduino.cc) with the esp8266 "board manager" installed (I had to install the Arduino's very latest version instead of the one installed on Ubuntu. Otherwise I didn't even have the "board manager" feature!).
