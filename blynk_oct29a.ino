//==================================================================================================
#include <SPI.h>
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
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
//==============================================================================
#define SEG_A  (1 << 0)
#define SEG_B (1 << 3)
#define SEG_C (1 << 5)
#define SEG_D (1 << 7)
#define SEG_E (1 << 1)
#define SEG_F (1 << 2)
#define SEG_G (1 << 4)
#define SEG_H (1 << 6)
//==============================================================================
#define DIG_0 (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define DIG_1 (SEG_C | SEG_B )
#define DIG_2 (SEG_A | SEG_B | SEG_D | SEG_E | SEG_G)
#define DIG_3 (SEG_A | SEG_B | SEG_C | SEG_D | SEG_G )
#define DIG_4 (SEG_B | SEG_C | SEG_F | SEG_G )
#define DIG_5 (SEG_A | SEG_C | SEG_D | SEG_G | SEG_F)
#define DIG_6 (SEG_A | SEG_G | SEG_C | SEG_D | SEG_E | SEG_F)
#define DIG_7 (SEG_A | SEG_B | SEG_C )
#define DIG_8 (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define DIG_9 (SEG_A | SEG_B | SEG_C | SEG_D | SEG_G | SEG_F )
#define DIG__ (0)
#define DIG_F (SEG_A | SEG_E | SEG_F | SEG_G)
#define DIG_n (SEG_C | SEG_E | SEG_G)
#define DIG_A (SEG_A | SEG_B | SEG_C| SEG_E | SEG_F | SEG_G)
#define DIG_L (SEG_D | SEG_E | SEG_F)
#define DIG_r (SEG_E | SEG_G)
#define DIG_S (SEG_A | SEG_C | SEG_D | SEG_F | SEG_G)
#define DIG_t (SEG_D | SEG_E | SEG_F | SEG_G)
#define DIG_b (SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)

//==============================================================================
class ESP_SPI
{
private:
    uint8_t _ss_pin;

public:
    ESP_SPI(uint8_t pin):_ss_pin(pin) {}

    //==============================================
    void begin()
    {
        pinMode(_ss_pin, OUTPUT);
        digitalWrite(_ss_pin, HIGH);
    }
    //==============================================
    void write(uint16_t data)
    {
        digitalWrite(_ss_pin, LOW);
        SPI.transfer((uint8_t)data);        
        SPI.transfer((uint8_t)(data>>8));
        digitalWrite(_ss_pin, HIGH);
    }   
};
//==============================================================================
uint8_t digit[] = {DIG_0, DIG_1, DIG_2, DIG_3, DIG_4,
         DIG_5, DIG_6, DIG_7, DIG_8, DIG_9,
         DIG__, DIG_F, DIG_n, DIG_A, DIG_L,
         DIG_r, DIG_S, DIG_t, DIG_b};
//==================================================================================================
uint8_t disp[3] = {11, 11, 11};
uint8_t disp_off[3] = {0, 11, 11};
ESP_SPI espSpi(SS);
uint8_t* scr = disp_off;
//==================================================================================================
void display(uint8_t *aDisp, uint8_t aBlink)
{
  static int blink = 100;
  static int pos = 0;
  pos++ ;
  if(pos >= 3) pos = 0;

  if (blink) blink--;
  else blink = 200;

  if (aBlink & (1 << pos))
  {
    if (blink > 100) espSpi.write(((~digit[aDisp[pos]]) << 8) + (1 << (pos + 1)));
    else  espSpi.write(((~digit[10]) << 8) + (1 << (pos + 1)));
  }
  else      espSpi.write(((~digit[aDisp[pos]]) << 8) + (1 << (pos + 1)));
}
//==================================================================================================
#define ONE_WIRE_BUS 4 // DS18B20 pin connect
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

BlynkTimer timer, timerSpi;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "4cf6924bdf534d4dbff1fd2fdb5ec8b7";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Solar";
char pass[] = "123454321";
//================================================================================================== 
void setup()
{
  // Debug console
  Serial.begin(115200);  
  Blynk.begin(auth, ssid, pass, "10.10.10.22", 8080);
  SPI.begin();
  espSpi.begin();
  display(scr, 6); 
  sensors.begin();
  timer.setInterval(5000L, sendTemps); 
  timerSpi.setInterval(5, ledInd); 
}
//==================================================================================================
void ledInd()
{  
   display(scr, 0);    
}
//==================================================================================================
void sendTemps()
{  
  sensors.requestTemperatures(); // Polls the sensors
  float tempBabyRoom = sensors.getTempCByIndex(0); // Gets first probe on wire in lieu of by address  
  //Serial.println(tempBabyRoom, 4);
  tempBabyRoom = (float)((int)(tempBabyRoom*10))/10;
  Blynk.virtualWrite(3, tempBabyRoom); 
} 
//==================================================================================================
void loop()
{
  Blynk.run(); 
  timer.run();
  timerSpi.run();
}
//==================================================================================================
 BLYNK_WRITE(V5)
  {
    int tmp = param.asInt(); // assigning incoming value from pin V1 to a variable
    scr[2] = tmp % 10; tmp /= 10;
    scr[1] = tmp % 10; tmp /= 10;
    scr[0] = tmp;
    }
    
