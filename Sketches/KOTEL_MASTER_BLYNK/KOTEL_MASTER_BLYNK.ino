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
float presetTemp = 22.51, tempBabyRoom = 0, hystTemp = 0.2;
#define PIN_RELAY (5)
//================================================================================================== 
void setup()
{          
  Serial.begin(115200);  
  Blynk.begin(auth, ssid, pass, "10.10.10.22", 8080);

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  timer.setInterval(2000L, sendTemps); 
  ip = WiFi.localIP();
  ip[3] = 255; 
  //replyPacket[8] = 0;

  pinMode(PIN_RELAY, OUTPUT);  
}
//==================================================================================================
void sendTemps()
{ 
  
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
  Blynk.virtualWrite(3, presetTemp -0.01);
  Blynk.virtualWrite(4, hystTemp);
  //if(tempBabyRoom > 26) Blynk.notify(String("Alarma"));

     
  // send back a reply, to the IP address and port we got the packet from
  memcpy(replyPacket,     &presetTemp, sizeof(float));
  memcpy(replyPacket + 4, &hystTemp,   sizeof(float));
  

  
  for(int i = 0; i < 8; i++)  Serial.print(replyPacket[i], HEX); Serial.println();
  
  Udp.beginPacket(ip, localUdpPort);
  Udp.write(replyPacket);
  Udp.endPacket();
  
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
    memcpy(&tempBabyRoom, aBuf,     sizeof(float));
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
 BLYNK_WRITE(V5)
  {
     presetTemp = param.asFloat() + 0.01; // assigning incoming value from pin V1 to a variable   
     Serial.print("presetTemp = ");    
     Serial.println(presetTemp); 
  }
//==================================================================================================
 BLYNK_WRITE(V6)
  {
     hystTemp = param.asFloat() + 0.01; // assigning incoming value from pin V1 to a variable   
      Serial.print("hystTemp = ");    
     Serial.println(hystTemp);   
  }
    
