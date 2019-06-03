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

#include <SPI.h>
#include <MFRC522.h>

#define DEBUG                       //!< enable debug output over the serial line

#define WIFI_ANALOG_PIN 0           //!< analog pin for reading the status of the "WiFi buttons"
#define ANALOG_3P3V_PIN_THRES 338   //!< threshold for deciding whether an input pin is 1 or 0 (3.3V pin) (=round(3.3/5*1023/2))
#define NUM_MFRC522         1       //!< number of MFRC522 modules connected

// Arduino Uno pins to coummunicate with the MFRC522s
#define MFRC522_RST_PIN     9       //!< pin connected to the RST on each MFRC522 module
#define MFRC522_1_SS_PIN   10       //!< pin connected to the first MFRC522's SPI SS pin (NSS, slave select)
#define MFRC522_2_SS_PIN    8       //!< pin connected to the second MFRC522's SPI SS pin (NSS, slave select)
#define MFRC522_3_SS_PIN    7       //!< pin connected to the third MFRC522's SPI SS pin (NSS, slave select)
#define MFRC522_4_SS_PIN    6       //!< pin connected to the fourth MFRC522's SPI SS pin (NSS, slave select)

#define MFRC522_GAIN_MASK   0x7     //!< gain mask to set the antenna gain (see p. 59, table 98 of NXP MFRC522 manual)

//! type for the state of the time machine
typedef enum TIME_MACHINE_STATE_e
{
    TIME_MACHINE_OFF = 0,   //!< time machine off
    TIME_MACHINE_ON  = 1    //!< time machine on/enabled
} TIME_MACHINE_STATE_t;

//! current state of the time machine (initialized to off)
static TIME_MACHINE_STATE_t stTimemachine;

//! MFRC522 modules connected
static MFRC522 mfrc522[NUM_MFRC522];

//! Initialization
void setup()
{
    // for debugging purposes, initialize a serial connection
    Serial.begin(115200);
    delay(10);

    // ************** TIME MACHINE INITIALIZATION **************
    stTimemachine = TIME_MACHINE_OFF;

    // ************** MFRC522 INITIALIZATION **************
    // initialize the SPI bus first
    SPI.begin();

    // initialize each module
    for (int k=0; k<NUM_MFRC522; k++)
    {
        byte ssPin = (k==0 ? MFRC522_1_SS_PIN :
                      k==1 ? MFRC522_2_SS_PIN :
                      k==2 ? MFRC522_3_SS_PIN :
                      k==3 ? MFRC522_4_SS_PIN : -1);
        if (ssPin < 0)
        {
            continue;
        }

        mfrc522[k].PCD_Init(ssPin, MFRC522_RST_PIN);
#ifdef DEBUG
        Serial.print("MFRC522 #");
        Serial.print(k+1);
        Serial.print(": ");
        mfrc522[k].PCD_DumpVersionToSerial();
#endif

#if defined(MFRC522_GAIN_MASK) && (MFRC522_GAIN_MASK >= 0x0) && (MFRC522_GAIN_MASK <= 0x7)
        // set gain to desired value
        mfrc522[k].PCD_SetAntennaGain(MFRC522_GAIN_MASK << 4);
#endif
    }
}

//! checks the state of the RFID cards potentially laid on the readers
static void checkRfidCards(void)
{
    for (int k=0; k<NUM_MFRC522; k++)
    {
        if (mfrc522[k].PICC_IsNewCardPresent() && mfrc522[k].PICC_ReadCardSerial())
        {
#ifdef DEBUG
            Serial.print("MFRC522 #");
            Serial.print(k+1);
            Serial.println(":");
            Serial.print("size=");
            Serial.println(mfrc522[k].uid.size);
            for (int n=0; n<mfrc522[k].uid.size; n++)
            {
                Serial.print("byte #");
                Serial.print(n);
                Serial.print("=");
                Serial.println(mfrc522[k].uid.uidByte[n]);
            }
#endif // DEBUG

            // Halt PICC
            mfrc522[k].PICC_HaltA();
            // Stop encryption on PCD
            mfrc522[k].PCD_StopCrypto1();
        }
    }
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
        // time machine activated and running => check RFID cards
        checkRfidCards();
    }
}
