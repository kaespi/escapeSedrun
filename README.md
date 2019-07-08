# escapeSedrun

This is a project containing code for running an escape game in the Cevi Muttenz Sola 2019 in Sedrun. The project contains many subparts such as
* wifiButtons
  * wifi_rx
  * wifi_tx
* timeMachine
  * controlPanel
* passwordWin


## wifiButtons

In this project we want to connect buttons through WiFi using ESP-01 ESP8266 modules to activate a mechanism if pressed in the right order. The reason for using WiFi here is to get rid of the nasty cabling and to have a more fancy solution.

This project consists of multiple transmitters sensing the buttons (wifi_tx) and one receiver, or host (wifi_rx).

All projects are designed for the [Arduino framework](https://www.arduino.cc) with the esp8266 "board manager" installed (I had to install the Arduino's very latest version instead of the one installed on Ubuntu. Otherwise I didn't even have the "board manager" feature!).

The host (wifi_rx) starts a WiFi network with SSID "escaperoom" and passphrase "abcdefgh12345678". The host will occupy IP address 192.168.0.1, subnet mask 255.255.255.0. It also starts a DHCP service with leases in the range 192.168.0.[2..100].

The client (wifi_tx) connects to the WiFi network with SSID "escaperoom" and passphrase "abcdefgh12345678". When it detects the button is pressed then it sends a UDP packet to the host (192.168.0.1) and again another UDP message when the button is released.

## timeMachine

In this project we want to build some sort of "backend" for a time machine with fancy control panel. It has an ESP-01 connected (the wifi_rx). The time machine/control panel shall be activated once the WiFi host flags a high level on its GPIO output pin.

The main riddle behind the time machine is using RFID badges and place them on the proper readers.

## passwordWin

A windows application written in VisualBasic to prompt for a password (fullscreen) and play a video once the correct password is entered.