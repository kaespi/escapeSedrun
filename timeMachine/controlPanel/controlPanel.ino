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

//#define DEBUG                     //!< enable debug output over the serial line

#define WIFI_ANALOG_PIN 0           //!< analog pin for reading the status of the "WiFi buttons"
#define ANALOG_3P3V_PIN_THRES 338   //!< threshold for deciding whether an input pin is 1 or 0 (3.3V pin) (=round(3.3/5*1023/2))
#define NUM_MFRC522         4       //!< number of MFRC522 modules connected

#define T_CARD_CHECK_MS   500       //!< time interval to check the cards' states [ms]

#define LED_PIN_RED1        2       //!< pin connected to two red LEDs
#define LED_PIN_RED2        3       //!< pin connected to two red LEDs
#define LED_PIN_YELLOW1     4       //!< pin connected to two yellow LEDs
#define LED_PIN_YELLOW2     5       //!< pin connected to two yellow LEDs
#define T_LED_BLINK_MS    200       //!< LED blink interval

// Arduino Uno pins to coummunicate with the MFRC522s
//#define T_MFRC_RESET_MS  5000     //!< interval for resetting (re-initializing MFRC522 modules)
#define MFRC522_SELF_TEST           //!< enables the self-test at startup to check if the MFRC's are initialized properly

#define MFRC522_RST_PIN     9       //!< pin connected to the RST on each MFRC522 module
#define MFRC522_1_SS_PIN   10       //!< pin connected to the first MFRC522's SPI SS pin (NSS, slave select)
#define MFRC522_2_SS_PIN    8       //!< pin connected to the second MFRC522's SPI SS pin (NSS, slave select)
#define MFRC522_3_SS_PIN    7       //!< pin connected to the third MFRC522's SPI SS pin (NSS, slave select)
#define MFRC522_4_SS_PIN    6       //!< pin connected to the fourth MFRC522's SPI SS pin (NSS, slave select)

#define MFRC522_GAIN_MASK   0x1     //!< gain mask to set the antenna gain (see p. 59, table 98 of NXP MFRC522 manual)

#define MASK(nbits) ((nbits)>=32 ? ((unsigned int)0xFFFFFFFF) : ((unsigned int)(1 << (nbits))-1))

//! type for the state of the time machine
typedef enum TIME_MACHINE_STATE_e
{
    TIME_MACHINE_OFF = 0,       //!< time machine off
    TIME_MACHINE_ON  = 1,       //!< time machine on/enabled
    TIME_MACHINE_PROGRAMMED = 2 //!< time machine programmed
} TIME_MACHINE_STATE_t;

//! current state of the time machine (initialized to off)
static TIME_MACHINE_STATE_t stTimemachine;

//! time when the LEDs changed state last time
static unsigned long tLastUpdateLed;

//! last state of LEDs
static unsigned long ledState;

//! MFRC522 modules connected
static MFRC522 mfrc522[NUM_MFRC522];

//! correct card identifiers
static const byte correctCards[NUM_MFRC522][4] =
{
    { 163, 140, 118, 26 },
    { 163, 134, 168, 26 },
    {   3,  82,  50, 27 },
    {   3,  19,  33, 27 }
};

//! bitmask with a bit set to 1 if the corresponding card was seen
static unsigned char cardsOk;

//! time of the last status update of the cards [ms]
static unsigned long tLastCheckMs;

#ifdef T_MFRC_RESET_MS
//! time when the MFRC522 were initialized last [ms]
static unsigned long tMfrcInitMs;
#endif

//! switches on the red LEDs
void ledRedOn(void)
{
    digitalWrite(LED_PIN_RED1, HIGH);
    digitalWrite(LED_PIN_RED2, HIGH);
}

//! switches off the red LEDs
void ledRedOff(void)
{
    digitalWrite(LED_PIN_RED1, LOW);
    digitalWrite(LED_PIN_RED2, LOW);
}

//! blinks the red LEDs (inverts basically their state)
void ledRedBlink(void)
{
    if (ledState==0)
    {
        digitalWrite(LED_PIN_RED1, HIGH);
        digitalWrite(LED_PIN_RED2, LOW);
        ledState = 1;
    }
    else
    {
        digitalWrite(LED_PIN_RED1, LOW);
        digitalWrite(LED_PIN_RED2, HIGH);
        ledState = 0;
    }
}

//! switches on the yellow LEDs
void ledYellowOn(void)
{
    digitalWrite(LED_PIN_YELLOW1, HIGH);
    digitalWrite(LED_PIN_YELLOW2, HIGH);
}

//! switches off the yellow LEDs
void ledYellowOff(void)
{
    digitalWrite(LED_PIN_YELLOW1, LOW);
    digitalWrite(LED_PIN_YELLOW2, LOW);
}

//! initializes the MFRC522 connected
void initMfrcs(void)
{
#ifdef T_MFRC_RESET_MS
    tMfrcInitMs = millis();
#endif

    tLastCheckMs = millis();
    cardsOk = 0;

    bool mfrcsReady = false;

    while (!mfrcsReady)
    {
        mfrcsReady = true;

#ifndef T_MFRC_RESET_MS
        if (stTimemachine = TIME_MACHINE_OFF)
        {
            ledRedOn();
            ledYellowOn();
        }
#endif

        // initialize the SPI bus. First stop pending transactions,
        // second initialize the SPI bus.
        SPI.endTransaction();
        SPI.end();
        SPI.begin();
        delay(50);

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
            delay(50);
            if (!mfrc522[k].PCD_PerformSelfTest())
            {
#ifndef T_MFRC_RESET_MS
                ledRedOff();
                ledYellowOff();
                if (k>=0)
                {
                    digitalWrite(LED_PIN_YELLOW1, HIGH);
                }
                if (k>=1)
                {
                    digitalWrite(LED_PIN_YELLOW2, HIGH);
                }
                if (k>=2)
                {
                    digitalWrite(LED_PIN_RED1, HIGH);
                }
                if (k>=3)
                {
                    digitalWrite(LED_PIN_RED2, HIGH);
                }
#endif // !T_MFRC_RESET_MS
                mfrcsReady = false;
                mfrc522[k].PCD_Reset();
#ifndef T_MFRC_RESET_MS
                delay(500);
#endif
            }

#ifdef DEBUG
            Serial.print("MFRC522 #");
            Serial.print(k+1);
            Serial.print(": ");
            mfrc522[k].PCD_DumpVersionToSerial();
            Serial.println("");
#endif

#if defined(MFRC522_GAIN_MASK) && (MFRC522_GAIN_MASK >= 0x0) && (MFRC522_GAIN_MASK <= 0x7)
            // set gain to desired value
            mfrc522[k].PCD_SetAntennaGain(MFRC522_GAIN_MASK << 4);
#endif

            delay(50);
        }

#ifndef T_MFRC_RESET_MS
        if (stTimemachine = TIME_MACHINE_OFF)
        {
            ledRedOff();
            ledYellowOff();
        }
#endif
    }
}

//! Initialization
void setup()
{
#ifdef DEBUG
    // for debugging purposes, initialize a serial connection
    Serial.begin(115200);
#endif
    delay(10);

    // ************** TIME MACHINE INITIALIZATION **************
    stTimemachine = TIME_MACHINE_ON;

    // ************** LED INITIALIZATION **************
    pinMode(LED_PIN_RED1, OUTPUT);
    pinMode(LED_PIN_RED2, OUTPUT);
    pinMode(LED_PIN_YELLOW1, OUTPUT);
    pinMode(LED_PIN_YELLOW2, OUTPUT);
    ledRedOff();
    ledYellowOff();

    tLastUpdateLed = millis();
    ledState = 0;

    // ************** MFRC522 INITIALIZATION **************
    initMfrcs();

#ifdef DEBUG
    Serial.println("Control panel ready");
#endif
}

//! performs actions when time machine is programmed
static void programTimeMachine(void)
{
    stTimemachine = TIME_MACHINE_PROGRAMMED;
    ledRedOff();
    ledYellowOn();
}

//! checks the state of the RFID cards potentially laid on the readers
static void checkRfidCards(void)
{
    for (int k=0; k<NUM_MFRC522; k++)
    {
        if (!(cardsOk & (1<<k)) &&  // do not try to detect the same card twice...
            mfrc522[k].PICC_IsNewCardPresent() &&
            mfrc522[k].PICC_ReadCardSerial())
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

            // Stop encryption on PCD
            mfrc522[k].PCD_StopCrypto1();

            if (mfrc522[k].uid.size==4)
            {
                int cardOk = 1;
                for (int n=0; n<4; n++)
                {
                    if (mfrc522[k].uid.uidByte[n] != correctCards[k][n])
                    {
                        cardOk = 0;
                    }
                }
                if (cardOk)
                {
#ifdef DEBUG
                    Serial.print("card #");
                    Serial.print(k+1);
                    Serial.println(" ok");
#endif
                    cardsOk |= (1 << k);
                }
            }
        }
    }

    if (cardsOk == MASK(NUM_MFRC522))
    {
#ifdef DEBUG
        Serial.println("all cards ok");
#endif
        programTimeMachine();
    }

    unsigned long now = millis();
    if (now - tLastCheckMs >= T_CARD_CHECK_MS)
    {
        cardsOk = 0;
        tLastCheckMs = now;
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
        // red LED blinking
        unsigned long now = millis();
        if (now - tLastUpdateLed > T_LED_BLINK_MS)
        {
            ledRedBlink();
            tLastUpdateLed = now;
        }

#ifdef T_MFRC_RESET_MS
        if (now - tMfrcInitMs > T_MFRC_RESET_MS)
        {
            initMfrcs();
        }
#endif

        // time machine activated and running => check RFID cards
        checkRfidCards();
    }
    else if (stTimemachine==TIME_MACHINE_PROGRAMMED)
    {
        // nothing to be done from here on...
    }
}
