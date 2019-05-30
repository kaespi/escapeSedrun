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

#define MIN(a,b) ((a) < (b) ? (a) : (b))    //!< Minium of two variables
#define MAX(a,b) ((a) > (b) ? (a) : (b))    //!< Maximum of two variables

const char* ssid = "escaperoom";
const char* password = "abcdefgh12345678";
#define UDP_PORT 16         //!< port to listen for UDP packets

//! UDP host instance
WiFiUDP udpHost;

//! Initialization
void setup()
{
    // for debugging purposes, initialize a serial connection
    Serial.begin(115200);
    delay(10);

    // ************** PIN INITIALIZATION **************
    pinMode(OUTPUT_PIN, OUTPUT);
 
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

//! Read UDP packets in the pipeline
static void readUdpPackets()
{
    static char buf[PACKET_BUF_SIZE];
    buf[PACKET_BUF_SIZE-1] = 0; // properly terminate buf (for potential printing)
    int nBytes = udpHost.read(buf, PACKET_BUF_SIZE-1);
    DBG_PACKETS(buf, nBytes);
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
