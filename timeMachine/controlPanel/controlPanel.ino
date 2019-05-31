/*  controlPanel: four RFID readers connected and activated with analog input
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

#define DEBUG                       //!< enable debug output over the serial line

#define WIFI_ANALOG_PIN 0           //!< analog pin for reading the status of the "WiFi buttons"
#define ANALOG_3P3V_PIN_THRES 338   //!< threshold for deciding whether an input pin is 1 or 0 (3.3V pin) (=round(3.3/5*1023/2))

//! type for the state of the time machine
typedef enum TIME_MACHINE_STATE_e
{
    TIME_MACHINE_OFF = 0,   //!< time machine off
    TIME_MACHINE_ON  = 1    //!< time machine on/enabled
} TIME_MACHINE_STATE_t;

//! current state of the time machine (initialized to off)
static TIME_MACHINE_STATE_t stTimemachine;

//! Initialization
void setup()
{
    // for debugging purposes, initialize a serial connection
    Serial.begin(115200);
    delay(10);

    // ************** TIME MACHINE INITIALIZATION **************
    stTimemachine = TIME_MACHINE_OFF;
}

//! Continuous running loop
void loop()
{
    if (stTimemachine==TIME_MACHINE_OFF)
    {
        // read the value from the sensor to see if the time machine
        // is activated
        int pinVal = analogRead(WIFI_ANALOG_PIN);
        if (pinVal > ANALOG_3P3V_PIN_THRES)
        {
            stTimemachine = TIME_MACHINE_ON;

#ifdef DEBUG
            Serial.print("Time machine activated (");
            Serial.print(pinVal);
            Serial.println(")");
#endif
        }
    }
    else if (stTimemachine==TIME_MACHINE_ON)
    {
        // time machine activated and running
    }
}
