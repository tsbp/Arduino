#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <DallasTemperature.h>
#include "driver_nokia_1100_lcd.h"
#include <Ticker.h>  //Ticker Library
#include <String.h>
//==================================================================================================
const char* ssid = "Solar";
const char* password = "123454321";
//==================================================================================================
#define ONE_WIRE_BUS 2 // DS18B20 pin connect
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
char aBuf[] = "+234";
float temperature, presetTemp, hystTemp;
Ticker ticker;
//==================================================================================================
WiFiUDP Udp;
unsigned int localUdpPort = 8266;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[sizeof(float) +1];  // a reply string to send back
//================================================================================================== 
void printTemp(float aT)
{
  int a = (int)(aT * 10);
  if (a < 0)  { char_24x16(13, 0, 1); a = a * (-1);}
  else          char_24x16(12, 0, 1);
  
  char_24x16(a/100 , 16, 1); a%=100;
  char_24x16(a/10, 32, 1);
  char_24x16(14, 48, 1);
  char_24x16(a%10, 56, 1);
  char_24x16(11, 72, 1);
}
//================================================================================================== 
void printPreset(float aP, float aH)
{
  int a = (int)(aP * 10);  
  char_10x16(a/100, 5,  5); a%=100;
  char_10x16(a/10 , 15, 5);
  char_10x16(  11 , 25, 5);
  char_10x16(a%10 , 30, 5);

  a = (int)(aH * 10);  
  char_10x16(a/10, 66, 5);
  char_10x16(  11, 76, 5);
  char_10x16(a%10, 81, 5);
}
//==================================================================================================
void getTemperature()
{
  sensors.requestTemperatures(); // Polls the sensors
  temperature = (float)((int)(sensors.getTempCByIndex(0)*10)) / 10; // Gets first probe on wire in lieu of by address 
  printTemp(temperature);
  printPreset(presetTemp, hystTemp);
}
//==================================================================================================
void setup()
{
  Serial.begin(115200);
  Serial.println();
  lcd_init();
  printTemp(0);
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  printTemp(-88.8);
  replyPacket[sizeof(float)] = 0;
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  
  sensors.begin();
  ticker.attach(1.5, getTemperature);
}
//==================================================================================================
void udpReceive()
{
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(aBuf, 30);
    if (len > 0)
    {
      aBuf[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", aBuf);   
    
   
   memcpy(&presetTemp, aBuf,     sizeof(float));
   memcpy(&hystTemp,   aBuf + 4, sizeof(float));
   Serial.print("presetTemp = "); Serial.println(presetTemp);
   Serial.print("hystTemp = ");   Serial.println(hystTemp);
   
   
   // send back a reply, to the IP address and port we got the packet from
   memcpy(replyPacket, &temperature, sizeof(float));
   Serial.print("T = "); Serial.println(temperature);
   for(int i = 0; i < sizeof(float); i++)  Serial.print(replyPacket[i], HEX); Serial.println();
   Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
   Udp.write(replyPacket);
   Udp.endPacket();
  }
}
//==================================================================================================
void loop()
{
  udpReceive();
}
