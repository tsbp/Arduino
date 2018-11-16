//==================================================================================================
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
//==================================================================================================
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "HCMS.h"
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

WidgetLED led(V6);
//==================================================================================================
#define PIN_RELAY (5)
float presetTemp = 25, hystTemp = 0.1;
char aBuf[] = "+234";
//==================================================================================================
void sendTemps()
{  
  sensors.requestTemperatures(); // Polls the sensors
  float tempBabyRoom = sensors.getTempCByIndex(0); // Gets first probe on wire in lieu of by address  
  Serial.println(tempBabyRoom, 4);
  
//  if( tempBabyRoom >= 0) aBuf[0] = '+';
//  else                   { aBuf[0] = '-'; tempBabyRoom = tempBabyRoom * (-1);}

  int t = (int)(tempBabyRoom * 10);
  aBuf[3] = t%10 + '0'; t= t/10;
  aBuf[2] = '.';
  aBuf[1] = t%10 + '0';
  aBuf[0] = t/10 + '0';
  
 
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
  
  Blynk.virtualWrite(3, tempBabyRoom); 
  //if(tempBabyRoom > 26) Blynk.notify(String("Alarma"));
  Blynk.virtualWrite(2, presetTemp);
  Load_HCMS( aBuf[0],  aBuf[1],  aBuf[2],  aBuf[3]);
} 
//================================================================================================== 
void setup()
{ 
  Serial.begin(115200);       
  HCMS_init(); 
  Load_HCMS( '-', '-', '-', '-');
  pinMode(PIN_RELAY, OUTPUT);
  sensors.begin();
  Blynk.begin(auth, ssid, pass, "10.10.10.22", 8080);
  timer.setInterval(1000L, sendTemps); 
}
//==================================================================================================
void loop()
{  
  Blynk.run(); 
  timer.run();
}
//==================================================================================================
 BLYNK_WRITE(V5) // slider  temperature preset
  {     
     presetTemp = param.asFloat(); // assigning incoming value from pin V1 to a variable     
     int t = (int)(presetTemp * 10);
     aBuf[3] = t%10 + '0'; t= t/10;
     aBuf[2] = '.';
     aBuf[1] = t%10 + '0';
     aBuf[0] = t/10 + '0';
     Load_HCMS( aBuf[0],  aBuf[1],  aBuf[2],  aBuf[3]);
  }
//==================================================================================================
 BLYNK_WRITE(V4) // slider  temperature preset
  {     
     hystTemp = param.asFloat(); // assigning incoming value from pin V1 to a variable     
     int t = (int)(hystTemp * 10);
     aBuf[3] = t%10 + '0'; t= t/10;
     aBuf[2] = '.';
     aBuf[1] = t%10 + '0';
     aBuf[0] = t/10 + '0';
     Load_HCMS( aBuf[0],  aBuf[1],  aBuf[2],  aBuf[3]);
  }
    
