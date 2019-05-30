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

#define WIFI_ANALOG_PIN 0   //!< analog pin for reading the status of the "WiFi buttons"

void setup() {
    // for debugging purposes, initialize a serial connection
    Serial.begin(115200);
    delay(10);
}

void loop() {
    // read the value from the sensor:
    int pinVal = analogRead(WIFI_ANALOG_PIN);
    Serial.print("pin value");
    Serial.println(pinVal);

    // wait for half a second
    delay(500);
}
