/*  wifi_rx: setup a WiFi network with an ESP8266 and listen for UDP packets
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

#define DEBUG               //!< enables debug output (on the serial line)
#ifdef DEBUG
#  define DBG_PACKETS(buf,len) debugUdpPackets(buf,len)
#else
#  define DBG_PACKETS(buf,len)
#endif

#define OUTPUT_PIN 2        //!< GPIO number to signal a correctly detected button sequence
#define PACKET_BUF_SIZE 255 //!< size of the packet buffer (for reading the UDP packets)
#define NUM_BUTTONS 4       //!< number of buttons expected
#define SEQUENCE_LEN 8      //!< length of the sequence

#define MIN(a,b) ((a) < (b) ? (a) : (b))    //!< Minium of two variables
#define MAX(a,b) ((a) > (b) ? (a) : (b))    //!< Maximum of two variables
#define MASK(nbits) ((nbits)>=32 ? ((unsigned int)0xFFFFFFFF) : ((unsigned int)(1 << (nbits))-1))

#if (SEQUENCE_LEN > 16)
#  error "Maximum sequence lenght can be 16"
#endif

const char* ssid = "escaperoom";
const char* password = "abcdefgh12345678";
#define UDP_PORT 16         //!< port to listen for UDP packets

//! UDP host instance
WiFiUDP udpHost;

//! sequence state
unsigned int btnSeq[NUM_BUTTONS];

//! Initialization
void setup()
{
    // for debugging purposes, initialize a serial connection
    Serial.begin(115200);
    delay(10);

    // ************** PIN INITIALIZATION **************
    pinMode(OUTPUT_PIN, OUTPUT);
    digitalWrite(OUTPUT_PIN, LOW);

    // ************** SEQUENCE DETECTION INITIALIZATION **************
    for (int k=0; k<NUM_BUTTONS; k++)
    {
        btnSeq[k] = 0;
    }
 
    // ************** WIFI NETWORK INITIALIZATION **************
    Serial.println();
    Serial.println();
    Serial.print("Setting up WiFi network with SSID ");
    Serial.println(ssid);
    Serial.print("and Passphrase ");
    Serial.println(password);
 
    // configure WiFi to access point mode
    WiFi.persistent(false);
    WiFi.mode(WIFI_AP);
    // set IP address and subnet mask for access point (i.e. this device)
    WiFi.softAPConfig(IPAddress(192,168,0,1), IPAddress(192,168,0,1), IPAddress(255, 255, 255, 0));
    
    // initialize DHCP configuration
    struct dhcps_lease dhcpConfig;
    dhcpConfig.enable = true;
    dhcpConfig.start_ip = IPAddress(192,168,0,2);
    dhcpConfig.end_ip   = IPAddress(192,168,0,100);
    wifi_softap_set_dhcps_lease(&dhcpConfig);

    // start the access point
    WiFi.softAP(ssid, password);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
 
    // give it some 2 seconds to initialize
    delay(2000);
    Serial.println("");
    Serial.println("WiFi started");
 
    // ************** UDP HOST INITIALIZATION **************
    udpHost.begin(UDP_PORT);
    Serial.printf("Now listening on port %u for UDP packets\n", UDP_PORT);
}

#ifdef DEBUG
static void debugUdpPackets(char* buf, int N)
{
    // remove any trailing space (white space, newline, etc.)
    buf[N] = '\0';
    while (--N > 0)
    {
        if (buf[N]=='\0' || buf[N]=='\n' || buf[N]=='\r' || buf[N]=='\t' || buf[N]==' ')
        {
            buf[N] = 0;
        }
        else
        {
            break;
        }
    }
    
    Serial.printf("Received packet '%s'\n", buf);
}
#endif // DEBUG

//! Handles the sequence detection
static void detectSequence(const int btnIndex, int btnState)
{
    // the correct sequence is defined as (character means: button press "1" and release "0")
    // A - D - C - B - C - A - B - D
    const unsigned int refSeq[NUM_BUTTONS] = {
                    // A  D  C  B  C  A  B  D
        0x00008020, // 10 00 00 00 00 10 00 00
        0x00000208, // 00 00 00 10 00 00 10 00
        0x00000880, // 00 00 10 00 10 00 00 00
        0x00002002  // 00 10 00 00 00 00 00 10
    };

    unsigned int allSeqCorrect = 1;
    unsigned int seqMask = MASK(2*SEQUENCE_LEN);
    for (int k=0; k<NUM_BUTTONS; k++)
    {
        btnSeq[k] <<= 1;
        if (btnIndex==k && btnState)
        {
            btnSeq[k] |= 1;
        }

        // added the "<< 1" below because we don't want to wait for the
        // release of the final button
        if (((btnSeq[k] << 1) & seqMask) != refSeq[k])
        {
            allSeqCorrect = 0;
        }
    }

    if (allSeqCorrect)
    {
        // successful sequence detection
        Serial.println("Sequence detected");
        digitalWrite(OUTPUT_PIN, HIGH);
    }
}

//! Read UDP packets in the pipeline
static void readUdpPackets()
{
    static char buf[PACKET_BUF_SIZE];
    buf[PACKET_BUF_SIZE-1] = 0; // properly terminate buf (for potential printing)
    int nBytes = udpHost.read(buf, PACKET_BUF_SIZE-1);
    DBG_PACKETS(buf, nBytes);

    // process the packets (search for pattern "BTN{0,1}{A,B,C,D,...}")
    if (nBytes >= 5 && buf[0]=='B' && buf[1]=='T' && buf[2]=='N')
    {
        int btnState = (buf[3]=='1' ? 1 :
                        buf[3]=='0' ? 0 : -1);
        int btnIndex = (buf[4] >= 'A' && buf[4] <= 'Z' ? buf[4] - 'A' : -1);

        if (btnState >= 0 && btnIndex >= 0)
        {
#ifdef DEBUG
            Serial.printf("Button %u %s\n", (unsigned char)btnIndex, btnState==1 ? "pressed" : "released");
#endif
            detectSequence(btnIndex, btnState);
        }
    }
}

//! Continuous running loop
void loop()
{
    int packetSize = udpHost.parsePacket();
    if (packetSize > 0)
    {
        // UDP packets received, decode them
        readUdpPackets();
    }
}
