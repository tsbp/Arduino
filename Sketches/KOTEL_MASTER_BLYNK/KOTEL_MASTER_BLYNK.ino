//==================================================================================================
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
//==================================================================================================
#include <SPI.h>
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <String.h>
#include "driver_nokia_1100_lcd.h"
#include <EEPROM.h>
//==================================================================================================
#define ONE_WIRE_BUS 2 // DS18B20 pin connect
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

BlynkTimer timer;//, timerSpi;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "4cf6924bdf534d4dbff1fd2fdb5ec8b7";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Solar";
char pass[] = "123454321";

char aBuf[30];

WiFiUDP Udp;
IPAddress ip;
unsigned int localUdpPort = 8266;  // local port to listen on
//char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[2 * sizeof(float)];
//==================================================================================================
WidgetLED led(V7);
float presetTemp = 22.0, tempBabyRoom = 0, hystTemp = 0.2;
#define PIN_RELAY (12)
//================================================================================================== 
void setup()
{          
  Serial.begin(115200);  
  Serial.println("");
  EEPROM.begin(10);
  char tmp[4];
  
  for(int i = 0; i < 4; i++) tmp[i] = EEPROM.read(i); 
  memcpy(&presetTemp, tmp,     sizeof(float));
  
  for(int i = 0; i < 4; i++) tmp[i] = EEPROM.read(i + 4);
  memcpy(&hystTemp, tmp,     sizeof(float));
  
  Serial.print("presetTemp = "); Serial.println(presetTemp);   
  Serial.print("hystTemp = ");   Serial.println(hystTemp);   
  
  Blynk.begin(auth, ssid, pass, "10.10.10.22", 8080);

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  timer.setInterval(1000L, sendTemps); 
  ip = WiFi.localIP();
  ip[3] = 255; 
  //replyPacket[2 * sizeof(float)] = 0;

  memcpy(replyPacket,     &presetTemp, sizeof(float));
  memcpy(replyPacket + 4, &hystTemp,   sizeof(float));

  pinMode(PIN_RELAY, OUTPUT);  
}
//==================================================================================================
char paramString[5] = "";
//==================================================================================================
void parametersToString()
{
  int a = (int)(presetTemp*10);
  paramString[0] = a/100 +'0'; a %= 100;
  paramString[1] = a/10 + '0';
  paramString[2] = a%10 + '0';

  a = (int)(hystTemp *10);
  paramString[3] = a/10 + '0';
  paramString[4] = a%10 + '0';
  
}
//==================================================================================================
void sendTemps()
{ 
  // send back a reply, to the IP address and port we got the packet from
//  memcpy(replyPacket,     &presetTemp, sizeof(float));
//  memcpy(replyPacket + 4, &hystTemp,   sizeof(float));
  

  
//  for(int i = 7; i >= 0; i--)  
//  {
//    Serial.print("0x");
//    Serial.print(replyPacket[i], HEX); 
//    Serial.print("; ");
//  }
//  Serial.println();
  parametersToString();
  Udp.beginPacket(ip, localUdpPort);
  Udp.write(paramString);
  Udp.endPacket();
 //==================================
  if(tempBabyRoom <= (presetTemp - hystTemp))  
  {
    digitalWrite(PIN_RELAY, HIGH);
    led.on();
  }
  else if (tempBabyRoom >= (presetTemp + hystTemp))
  {
    digitalWrite(PIN_RELAY, LOW);
    led.off();
  }
  //========================================
  Blynk.virtualWrite(2, tempBabyRoom);
  Blynk.virtualWrite(3, presetTemp);
  Blynk.virtualWrite(4, hystTemp);
  //if(tempBabyRoom > 26) Blynk.notify(String("Alarma"));
  
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

    //tempBabyRoom = atof(aBuf) / 10; 
    //memcpy(&tempBabyRoom, aBuf,     sizeof(float));
   char tt[3];
   memcpy(tt, aBuf, 3);    tempBabyRoom = (float)(atoi(tt)) / 10;
    
    Serial.print("tempBabyRoom = ");    
    Serial.println(tempBabyRoom);    
  }
}
//==================================================================================================
void loop()
{
  Blynk.run(); 
  timer.run();
  udpReceive();
}
//==================================================================================================
void saveData()
{
   EEPROM.begin(10);
   char tmp[8]; 
   memcpy(tmp,     &presetTemp,  sizeof(float)); 
   memcpy(tmp + 4, &hystTemp,  sizeof(float)); 
   for(int i = 0; i < 8; i++) EEPROM.write(i, tmp[i]);
   EEPROM.end(); 
}
//==================================================================================================
 BLYNK_WRITE(V5)
  {
     presetTemp = param.asFloat(); // assigning incoming value from pin V1 to a variable   
     Serial.print("presetTemp = ");    
     Serial.println(presetTemp); 
//     memcpy(replyPacket,     &presetTemp, sizeof(float));
//     memcpy(replyPacket + 4, &hystTemp,   sizeof(float));
     saveData();
  }
//==================================================================================================
 BLYNK_WRITE(V6)
  {
     hystTemp = param.asFloat(); // assigning incoming value from pin V1 to a variable   
     Serial.print("hystTemp = ");    
     Serial.println(hystTemp);  
//     memcpy(replyPacket,     &presetTemp, sizeof(float));
//     memcpy(replyPacket + 4, &hystTemp,   sizeof(float));
     saveData();
  }
    
