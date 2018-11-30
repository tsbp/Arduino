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
unsigned int localUdpPort = 4210;  // local port to listen on
//char incomingPacket[255];  // buffer for incoming packets
//==================================================================================================
WidgetLED led(V6);
float presetTemp = 22, tempBabyRoom = 0, hystTemp = 0.2;
#define PIN_RELAY (5)
//================================================================================================== 
void printTemp()
{
  if (aBuf[0] == '+')
    char_24x16(12, 0, 1);
    else
    char_24x16(13, 0, 1);
  
  char_24x16(aBuf[1] , 16, 1);
  char_24x16(aBuf[2], 32, 1);
  char_24x16(14, 48, 1);
  char_24x16(aBuf[3], 56, 1);
  char_24x16(11, 72, 1);
}
//================================================================================================== 
void printPreset(int a)
{
  char b[3];
  b[2] = a%10; a= a/10;
  b[1] = a%10;
  b[0] = a/10; 
  
  char_10x16(b[0], 5, 5);
  char_10x16(b[1], 15, 5);
  char_10x16(  11, 25, 5);
  char_10x16(b[2], 30, 5);

  
  char_10x16((int)hystTemp, 66, 5);
  char_10x16(  11, 76, 5);
  char_10x16((int)(hystTemp*10)%10, 81, 5);
}
//================================================================================================== 
void setup()
{  
  lcd_init();
  printTemp();
        
  Serial.begin(115200);  
  Blynk.begin(auth, ssid, pass, "10.10.10.22", 8080);

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  timer.setInterval(2000L, sendTemps);  

  pinMode(PIN_RELAY, OUTPUT);
  printPreset((int)(presetTemp *10));
}


//==================================================================================================
void sendTemps()
{ 
  int t = (int)(tempBabyRoom*10);
  aBuf[3] = t%10; t= t/10;
  aBuf[2] = t%10;
  aBuf[1] = t/10;
  printTemp();
  //=========================================
  tempBabyRoom = (float)((int)(tempBabyRoom*10))/10;
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
  Blynk.virtualWrite(3, tempBabyRoom); 
  //if(tempBabyRoom > 26) Blynk.notify(String("Alarma"));
  Blynk.virtualWrite(2, presetTemp);
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

    tempBabyRoom = atof(aBuf) / 10; 
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
     presetTemp = param.asFloat(); // assigning incoming value from pin V1 to a variable
     printPreset((int)(presetTemp *10));
  }
//==================================================================================================
 BLYNK_WRITE(V4)
  {
     hystTemp = param.asFloat(); // assigning incoming value from pin V1 to a variable   
     printPreset((int)(presetTemp *10));  
  }
    
