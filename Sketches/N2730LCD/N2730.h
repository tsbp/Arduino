//==================================================================================================
#include <SPI.h>
#include "fonts.h"
//==================================================================================================
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
    void serialSendData(char a, char b)
    {
        digitalWrite(_ss_pin, LOW);
        SPI.transfer(a);        
        SPI.transfer(b);
        digitalWrite(_ss_pin, HIGH);
    }   
};
ESP_SPI espSpi(SS);
 //==================================================================================================
struct uLCD_WORD
  {
    char control;
    char data;
  };
uLCD_WORD lcdWord;

//==================================================================================================
#define COMMAND         (0)
#define DATA            (1)

#define LCD_WRX_BIT                     (1<<2)//(BIT2)
#define LCD_CSX_BIT                     (1<<6)//(BIT6)
#define LCD_RESX_BIT                    (1<<4)//(BIT4)
#define LCD_TE_BIT                      (1<<5)//(BIT5)
#define LCD_DCX_BIT                     (1<<3)//(BIT3)
#define LCD_RDX_BIT                     (1<<1)//(BIT1)


#define SWRESET          (0x01) // Software reset
#define RDDID           (0x04) // Read Display ID
#define RDDST         (0x0A) // Read Display Power Mode
#define RDDPM           (0x0B) // Read Display MADCTR
#define RDDMADCTR   (0x0C) // Read Display Pixel Format
#define RDDCOLMOD (0x0D) // Read Display Image Mode
#define RDDIM           (0x0E) // Read Display Signal Mode
#define RDDSM         (0x0F) // Read Display Self-diagnostic result
#define RDDSDR          (0x10) // Sleep in & booster off
#define SLPOUT          (0x11) // Sleep out & booster on
#define PTLON         (0x12) // Partial mode on
#define NORON         (0x13) // Partial off (Normal)
#define INVOFF          (0x20) // Display inversion off (normal)
#define INVON         (0x21) // Display inversion on
#define GAMSET          (0x26) // Gamma curve select
#define DISPOFF         (0x28) // Display off
#define DISPON          (0x29) // Display on
#define CASET         (0x2A) // Column address set
#define RASET         (0x2B) // Row address set
#define RAMWR           (0x2C) // Memory write
#define RAMRD         (0x2E) // Memory read
#define PTLAR0          (0x30) // Partial start/end address set
#define TEOFF           (0x34) // Tearing effect line off
#define TEON          (0x35) // Tearing effect mode set & on
#define MADCTR          (0x36) // Memory data access control
#define IDMOFF          (0x38) // Idle mode off
#define IDMON         (0x39) // Idle mode on
#define COLMOD          (0x3A) // Interface pixel format
#define WRDISBV         (0x51) // Write Display Brightness
#define RDDISBV0  (0x52) // Read Display Brightness value
#define WRCTRLD   (0x53) // Write Control Display
#define RDCTRLD0  (0x54) // Read Control Display
#define WRCABC          (0x55) // Wrtie Content Adaptive Brightness
#define RDCABC          (0x56) // Read Content Adaptive Brightness
#define RDID1         (0xDA) // Read ID1
#define RDID20          (0xDB) // Read ID2
#define RDID3         (0xDC) // Read ID3
#define IFMODE          (0xB0) // Set display interface mode
#define DISCLK          (0xB1) // Display clock set
#define INVCTR          (0xB2) // Display inversion control
#define REGCTR          (0xC0) // Regulator control
#define VCOMCTR0  (0xC1) // VCOML/VCOMH voltage control
#define GAMCTR1   (0xC8) // Set gamma correction characteristics
#define GAMCTR2   (0xC9) // Set gamma correction characteristics
#define GAMCTR3   (0xCA) // Set gamma correction characteristics
#define GAMCTR4         (0xCB) // Set gamma correction characteristics
#define EPPGMDB         (0xD0) // Write ID2,VCOM Offset for EEPROM program
#define EPERASE         (0xD1) // EEPROM erase
#define EPPROG          (0xD2) // EEPROM program
#define EPRDVRF         (0xD3) // EEPROM read, verify register set
#define RDVCOF          (0xD9) // VCOM offset bits read
#define LEDCTRL         (0xEF) // Write LED control value

#define LCD_WRX(n) {if (n) lcdWord.control |= LCD_WRX_BIT;   else lcdWord.control &= ~LCD_WRX_BIT;}
#define LCD_CSX(n) {if (n) lcdWord.control |= LCD_CSX_BIT;   else lcdWord.control &= ~LCD_CSX_BIT;}
#define LCD_RESX(n){if (n) lcdWord.control |= LCD_RESX_BIT;  else lcdWord.control &= ~LCD_RESX_BIT;}
#define LCD_TE(n)  {if (n) lcdWord.control |= LCD_TE_BIT;    else lcdWord.control &= ~LCD_TE_BIT;}
#define LCD_DCX(n) {if (n) lcdWord.control |= LCD_DCX_BIT;   else lcdWord.control &= ~LCD_DCX_BIT;}
#define LCD_RDX(n) {if (n) lcdWord.control |= LCD_RDX_BIT;   else lcdWord.control &= ~LCD_RDX_BIT;}

//==============================================================================
#define RED       (0xff0000)
#define GREEN     (0x00ff00)
#define BLUE      (0x0000ff)
#define YELLOW    (0xffff00)
#define WHITE     (0xffffff)
#define BLACK     (0x000000)
//==================================================================================================
void lcdWrite(unsigned char aType, unsigned char aByte)
{

  LCD_DCX(aType);
  espSpi.serialSendData(lcdWord.control, lcdWord.data);
  lcdWord.data = aByte;
  espSpi.serialSendData(lcdWord.control, lcdWord.data);
  LCD_WRX(0);
  espSpi.serialSendData(lcdWord.control, lcdWord.data);
  LCD_WRX(1);
  espSpi.serialSendData(lcdWord.control, lcdWord.data);
}
//==================================================================================================
void LCD_wakeup(char aOrient)
{
  
  SPI.begin();
  espSpi.begin();
   
  LCD_RDX(1);   espSpi.serialSendData(lcdWord.control, lcdWord.data);
  LCD_CSX(1);   espSpi.serialSendData(lcdWord.control, lcdWord.data);          //RES = 0;
  delay(10); //for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
  LCD_CSX(0);   espSpi.serialSendData(lcdWord.control, lcdWord.data);          //RES = 1;
  LCD_WRX(1);   espSpi.serialSendData(lcdWord.control, lcdWord.data);             //WR = 1;
  LCD_RESX(0);  espSpi.serialSendData(lcdWord.control, lcdWord.data);//LCD_RESX(0);             //RES = 0;
   delay(10); //for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
  LCD_RESX(1);  espSpi.serialSendData(lcdWord.control, lcdWord.data);//LCD_RESX(1);             //RES = 1;
  lcdWrite(COMMAND, SWRESET); //SendCom(0x01);       // ����� ������� �� ������
   delay(10); //for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(5);
  lcdWrite(COMMAND, SLPOUT); //SendCom(0x11);       // ����� ������� �� ������
  lcdWrite(COMMAND, DISPON); //SendCom(0x29);       // ��������� �������
   delay(10); //for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
  lcdWrite(COMMAND, MADCTR); //SendCom(0x36);
  lcdWrite(DATA,    aOrient); //SendDat(0xC0);       // ������� ���������� �������
   delay(10); //for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
   delay(10); //for(unsigned int i = 0; i < 0xffff; i++);       // delay_ms(1);
//  lcdWrite(COMMAND, COLMOD); //SendCom(0x3A);
//  lcdWrite(DATA,    7); //SendDat(0b00000111); // 24 �����������

}
//==============================================================================
void  setPixel(unsigned long aCol)
{
  lcdWrite(DATA, (unsigned char)(aCol >> 16));
  lcdWrite(DATA, (unsigned char)(aCol >> 8));
  lcdWrite(DATA, (unsigned char)aCol);
}
//==================================================================================================
void  char_6x8 (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
{

   lcdWrite(COMMAND, CASET);

   lcdWrite(DATA, (unsigned char)((aX) >> 8));
   lcdWrite(DATA, (unsigned char)(aX));
   lcdWrite(DATA, (unsigned char)((aX+11) >> 8));
   lcdWrite(DATA, (unsigned char)(aX+11));

   lcdWrite(COMMAND, RASET);

   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY));
   lcdWrite(DATA, (unsigned char)((aY+16) >> 8));
   lcdWrite(DATA, (unsigned char)(aY+16));



   lcdWrite(COMMAND, RAMWR);
   int i, j;

   for(i = 0; i < 8; i++)
   {
      for(j = 0; j < 6; j++)
      {
        if (Font8x6[aChar*6+j] & (1 << i)) { setPixel(aCOL); setPixel(aCOL);}
        else                               { setPixel(aBGCOL); setPixel(aBGCOL);}
      }
      for(j = 0; j < 6; j++)
      {
        if (Font8x6[aChar*6+j] & (1 << i)) { setPixel(aCOL); setPixel(aCOL);}
        else                               { setPixel(aBGCOL); setPixel(aBGCOL);}
      }
   }
}
//==================================================================================================
void  printString (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, const char *aStr)
{
  while (*aStr)
  {
    char_6x8(aX, aY, aCOL, aBGCOL, *aStr++);
    aX += 12;
  }
}
//==============================================================================
void print_icon (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
{
   lcdWrite(COMMAND, CASET);

   lcdWrite(DATA, (unsigned char)((aX) >> 8));
   lcdWrite(DATA, (unsigned char)(aX));
   lcdWrite(DATA, (unsigned char)((aX+23) >> 8));
   lcdWrite(DATA, (unsigned char)(aX+23));

   lcdWrite(COMMAND, RASET);
   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY));
   lcdWrite(DATA, (unsigned char)((aY+23) >> 8));
   lcdWrite(DATA, (unsigned char)(aY+23));

   lcdWrite(COMMAND, RAMWR);

   int i, j, k;

   for(i = 0; i < 3; i++)
     for(j = 0; j < 8; j++)
       for(k = 0; k < 24; k++)
       {
         if (icons[24*3*aChar + 24*i + k] & (1 << j)) { setPixel(aCOL);}
         else                                         { setPixel(aBGCOL);}
       }
}
//==============================================================================
void  printDigit_16x32 (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
{
   lcdWrite(COMMAND, CASET);

   lcdWrite(DATA, (unsigned char)((aX) >> 8));
   lcdWrite(DATA, (unsigned char)(aX));
   lcdWrite(DATA, (unsigned char)((aX+15) >> 8));
   lcdWrite(DATA, (unsigned char)(aX+15));

   lcdWrite(COMMAND, RASET);
   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY));
   lcdWrite(DATA, (unsigned char)((aY+31) >> 8));
   lcdWrite(DATA, (unsigned char)(aY+31));

   lcdWrite(COMMAND, RAMWR);

   int i, j, k;

   for(i = 0; i < 4; i++)
     for(j = 0; j < 8; j++)
       for(k = 0; k < 16; k++)
       {
         if (GOST_16x32[16*4*aChar + 16*i + k] & (1 << j)) { setPixel(aCOL);}
         else                               { setPixel(aBGCOL);}
       }
}
//==============================================================================
void  printDigit_16x32BG (unsigned int aX, unsigned int aY, unsigned long aCOL, unsigned long aBGCOL, unsigned char aChar)
{
   lcdWrite(COMMAND, CASET);

   lcdWrite(DATA, (unsigned char)((aX) >> 8));
   lcdWrite(DATA, (unsigned char)(aX));
   lcdWrite(DATA, (unsigned char)((aX+31) >> 8));
   lcdWrite(DATA, (unsigned char)(aX+31));

   lcdWrite(COMMAND, RASET);
   lcdWrite(DATA, (unsigned char)(aY >> 8));
   lcdWrite(DATA, (unsigned char)(aY));
   lcdWrite(DATA, (unsigned char)((aY+63) >> 8));
   lcdWrite(DATA, (unsigned char)(aY+63));

   lcdWrite(COMMAND, RAMWR);

   int i, j, k;

   for(i = 0; i < 4; i++)
     for(j = 0; j < 8; j++)
     {
       for(k = 0; k < 16; k++)
       {
         if (GOST_16x32[16*4*aChar + 16*i + k] & (1 << j)) { setPixel(aCOL); setPixel(aCOL);}
         else                               { setPixel(aBGCOL); setPixel(aBGCOL);}
       }
        for(k = 0; k < 16; k++)
       {
         if (GOST_16x32[16*4*aChar + 16*i + k] & (1 << j)) { setPixel(aCOL); setPixel(aCOL);}
         else                               { setPixel(aBGCOL); setPixel(aBGCOL);}
       }
     }
}
//==============================================================================
