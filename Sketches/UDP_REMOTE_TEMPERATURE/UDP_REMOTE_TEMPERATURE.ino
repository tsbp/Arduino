#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <DallasTemperature.h>
//==================================================================================================
#define ONE_WIRE_BUS 2 // DS18B20 pin connect
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
char aBuf[] = "+234";

const char* ssid = "Solar";
const char* password = "123454321";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "Hi there! Got the message :-)";  // a reply string to send back

char  _replyPacket[2];  // a reply string to send back
int cntr = 0;

IPAddress ip;


void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  ip = WiFi.localIP();
  ip[3] = 255;
  
  sensors.begin();
}



void loop()
{
//  int packetSize = Udp.parsePacket();
//  if (packetSize)
//  {
//    // receive incoming UDP packets
//    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
//    int len = Udp.read(incomingPacket, 255);
//    if (len > 0)
//    {
//      incomingPacket[len] = 0;
//    }
////    Udp.flush();
//    Serial.printf("UDP packet contents: %s\n", incomingPacket);
//
//    // send back a reply, to the IP address and port we got the packet from
//    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
//    Udp.write(replyPacket);
//    Udp.endPacket();
//    
//  }
 //=======================================================================================
 // send temp via udp
 //=======================================================================================
    sensors.requestTemperatures(); // Polls the sensors
    float tempBabyRoom = sensors.getTempCByIndex(0); // Gets first probe on wire in lieu of by address     
     
     if( tempBabyRoom >= 0) aBuf[0] = '+';
     else                   { aBuf[0] = '-'; tempBabyRoom = tempBabyRoom * (-1);}

     int t = (int)(tempBabyRoom * 10);
     aBuf[3] = t%10 + '0'; t= t/10;   
     aBuf[2] = t%10 + '0';
     aBuf[1] = t/10 + '0';
     
     
    Udp.beginPacket(ip, 4210);
    Udp.write(aBuf);
    Udp.endPacket();
    delay(2000);
}
