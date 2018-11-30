//==================================================================================================
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
//==================================================================================================
#include <ESP8266WiFi.h>
#include <Blynk.h>
#include <BlynkSimpleEsp8266.h>
#include "N2730.h"
#include <OneWire.h>
#include <DallasTemperature.h>
//==================================================================================================
#define ONE_WIRE_BUS 2 // DS18B20 pin connect
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
BlynkTimer timer;
//==================================================================================================
int cntr = 0, c = 0;
unsigned long color = GREEN;
//==================================================================================================
void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);     
  LCD_wakeup(0);  
  sensors.begin();
  timer.setInterval(1000L, sendTemps);   
}
//==================================================================================================
int stage = 3;
//==================================================================================================
void sendTemps()
{  
  sensors.requestTemperatures(); // Polls the sensors
  int tempBabyRoom = (int)(10 * sensors.getTempCByIndex(0)); // Gets first probe on wire in lieu of by address  
  Serial.println(tempBabyRoom);
  switch(stage)
  {
    case 0: printDigit_16x32BG(20+32*0, 98, BLUE, WHITE, tempBabyRoom / 100); break;
    case 1: tempBabyRoom = tempBabyRoom % 100; 
            printDigit_16x32BG(20+32*1, 98, BLUE, WHITE, tempBabyRoom / 10); break;
    case 2: tempBabyRoom = tempBabyRoom % 100;
            printDigit_16x32BG(20+32*2, 98, BLUE, WHITE, 10); break;
    case 3: tempBabyRoom = tempBabyRoom % 100;
            printDigit_16x32BG(20+32*3, 98, BLUE, WHITE, tempBabyRoom % 10); break;
  }

  if(stage) stage--;
  else stage = 3;
} 
//==================================================================================================
void loop() 
{ 
  timer.run();
//  printString (10, cntr, color, WHITE, "Hello, World!!!");
//  print_icon (200, 40, RED, WHITE, c);
//  cntr +=20;
//  if (cntr > 100) 
//  {
//    cntr = 0; 
//    c++;
//    if(c == 6) c = 0;
//    if(c == 0) color = RED;
//    if(c == 1) color = GREEN;
//    if(c == 2) color = BLUE;
//    if(c == 3) color = YELLOW;
//    if(c == 4) color = WHITE;
//    if(c == 5) color = BLACK;
//  }
//  delay(1000);
}
