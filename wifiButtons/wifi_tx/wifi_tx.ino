/*  wifi_tx: send status updates over UDP (WiFi using a ESP8266) on connected button
    Copyright (C) 2019  Kaspar Giger <kg@kgmw.ch>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define DEBUG                   //!< enables debug output (on the serial line)

#define INPUT_PIN 2             //!< GPIO number to detect a pressed button
#define BUTTON_CHAR 'A'         //!< button name/number
#define MIN_DELAY_PIN_READ 5    //!< waiting time for reading the pin (to prevent detecting glitches) [ms]

const char* ssid = "escaperoom";
const char* password = "abcdefgh12345678";
const IPAddress hostIp = IPAddress(192, 168, 0, 1);
#define UDP_PORT 16         //!< port to transmit UDP packets

//! UDP client instance
WiFiUDP udpClient;

typedef enum BUTTON_STATE_e
{
    BUTTON_IDLE = 0,    //!< button not pressed
    BUTTON_DOWN = 1     //!< button pressed
} BUTTON_STATE_t;

//! button state
BUTTON_STATE_t stButton;

//! time when the button changed the state the last time [ms]
unsigned long timeLastChange;

//! Initialization
void setup()
{
    // for debugging purposes, initialize a serial connection
    Serial.begin(115200);
    delay(10);

    // ************** PIN INITIALIZATION **************
    pinMode(INPUT_PIN, INPUT);
    stButton = BUTTON_IDLE;
    timeLastChange = millis();

    // ************** WIFI NETWORK INITIALIZATION **************
    Serial.println();
    Serial.println();
    Serial.print("Connecting to WiFi network with SSID ");
    Serial.println(ssid);
    Serial.print("and Passphrase ");
    Serial.println(password);

    // configure WiFi to access point mode
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi started");

    // ************** UDP HOST INITIALIZATION **************
    udpClient.begin(UDP_PORT);

    Serial.printf("Check for button on pin %u and send status over UDP, port %u\n", INPUT_PIN, UDP_PORT);

    char udpMsg[6] = { 'I', 'N', 'I', 'T', BUTTON_CHAR, '\0' };
    udpClient.beginPacket(hostIp, UDP_PORT);
    udpClient.write(udpMsg);
    udpClient.endPacket();
}

//! Continuous running loop
void loop()
{
    // read the pin's value
    int pinVal = digitalRead(INPUT_PIN);
    unsigned long tNow = millis();

    int stateChanged = 0;
    if ((tNow-timeLastChange) >= MIN_DELAY_PIN_READ)
    {
        if (stButton==BUTTON_IDLE && pinVal==LOW)
        {
            // button started to be pressed down
            stButton = BUTTON_DOWN;
            stateChanged = 1;
#ifdef DEBUG
            Serial.println("Button pressed");
#endif
        }
        else if (stButton==BUTTON_DOWN && pinVal==HIGH)
        {
            // button was released
            stButton = BUTTON_IDLE;
            stateChanged = 1;
#ifdef DEBUG
            Serial.println("Button released");
#endif
        }
    }

    if (stateChanged)
    {
        char udpMsg[6] = { 'B', 'T', 'N', '0', BUTTON_CHAR, '\0' };

        udpClient.beginPacket(hostIp, UDP_PORT);
        if (stButton==BUTTON_DOWN)
        {
            udpMsg[3] = '1';
        }
        udpClient.write(udpMsg);
        udpClient.endPacket();

        timeLastChange = tNow;
    }
}
