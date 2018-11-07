//==================================================================================================
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
//==================================================================================================
#include <SPI.h>
#include <Blynk.h>
#include <ESP8266WiFi.h>
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

char aBuf[] = "+234";
//================================================================================================== 
void printTemp()
{
  if (aBuf[0] == '+')
    char_24x16(12, 0, 3);
    else
    char_24x16(13, 0, 3);
  
  char_24x16(aBuf[1] - '0', 16, 3);
  char_24x16(aBuf[2] - '0', 32, 3);
  char_24x16(14, 48, 3);
  char_24x16(aBuf[3] - '0', 56, 3);
  char_24x16(11, 72, 3);
}
//================================================================================================== 
void setup()
{  
  lcd_init();
  printTemp();
        
  Serial.begin(115200);  
  Blynk.begin(auth, ssid, pass, "10.10.10.22", 8080);
 
  sensors.begin();
  timer.setInterval(1000L, sendTemps); 
  //timerSpi.setInterval(5, ledInd); 
}
//==================================================================================================
float presetTemp = 220;
//==================================================================================================
void sendTemps()
{  
  sensors.requestTemperatures(); // Polls the sensors
  float tempBabyRoom = sensors.getTempCByIndex(0); // Gets first probe on wire in lieu of by address  
  Serial.println(tempBabyRoom, 4);
  
  if( tempBabyRoom >= 0) aBuf[0] = '+';
  else                   { aBuf[0] = '-'; tempBabyRoom = tempBabyRoom * (-1);}

  int t = (int)(tempBabyRoom * 10);
  aBuf[3] = t%10 + '0'; t= t/10;
  aBuf[2] = t%10 + '0';
  aBuf[1] = t/10 + '0';

  printTemp();
  
 
  tempBabyRoom = (float)((int)(tempBabyRoom*10))/10;
  Blynk.virtualWrite(3, tempBabyRoom); 
  //if(tempBabyRoom > 26) Blynk.notify(String("Alarma"));
  Blynk.virtualWrite(2, presetTemp);
} 
//==================================================================================================
void loop()
{
  Blynk.run(); 
  timer.run();
}
//==================================================================================================
 BLYNK_WRITE(V5)
  {
     presetTemp = param.asFloat(); // assigning incoming value from pin V1 to a variable
    
  }
    
