/*
 Справка
  addr - адрес байта
 
 EEPROM.begin(512);          // Каждый раз объявляем если хотим прочесть или записать для ESP
 EEPROM.write(addr,  val);   // записали переменную val(размером int8_t) по адресу addr 
 EEPROM.read(addr,  val);    // прочитать из адреса addr в переменную val(размером int8_t)

// Внимание Размеры должны совпадать иначе могут быть проблемы
// записали число val(библиотека "EEPROMAnything.h" размер определит сама) по адресу addr
 EEPROM_writeAnything(addr, val);
// прочитать из адреса addr в переменную val(библиотека "EEPROMAnything.h" размер определит сама) 
 EEPROM_readAnything(addr, val);  
  
 EEPROM.commit();        // закончить запись
 EEPROM.end() тоже производит запись данных,
 а также освобождает оперативную память от данных,
 запись которых произведена.

   Размеры переменных
   8бит = 1 байт
   u - перед int значит что число положительное
   
 int8_t     |  char                |   от -128 до 127               | 1 byte
 uint8_t    |  byte, unsigned char |   от 0 до 255                  | 1 byte
 int16_t    |  int                 |   от -32768 до 32767           | 2 byte
 uint16_t   |  unsigned int, word  |   от 0 до 65535                | 2 byte
 int32_t    |  long                |  от  -2147483648 до 2147483647 | 4 byte
 uint32_t   |  unsigned long       |  от 0 до 4294967295            | 4 byte
 
 и еще бывают int64_t и  uint64_t
*/

#include <EEPROM.h>         
#include "EEPROMAnything.h" // http://playground.arduino.cc/Code/EEPROMWriteAnything

// Функция чтения данных из Энергонезовисимой памяти EEPROM
void EEPROM_READ()
{
 // Serial.println("EEPROM_READ_Start");
  
  int addr=0; // Номер байта EEPROM
  EEPROM.begin(512);
// Флаги адреса с 0 по 19
  flag_AutoManual = EEPROM.read(addr);   // Адрес 0
//  flag_TableStop = EEPROM.read(addr+1);  // Адрес 1

// Уставки констант с 20 по 39
  addr = 20;
  Delta = EEPROM.read(addr);             // Адрес 20
  AlarmTempStart = EEPROM.read(addr+4);  // Адрес 24 
  AlarmTempStop = EEPROM.read(addr+8);   // Адрес 28

// Уставки температуры c 100 по 199
  addr = 100;
  for (int i=0; i < 4; i++)
  {EEPROM_readAnything(addr+i*10, TempTuning[i]);}  
/*  
  TempTuning[0] // Адрес 100
  TempTuning[1] // Адрес 110
  TempTuning[2] // Адрес 120
  TempTuning[3] // Адрес 130
*/    
// Уставки времени c 200 по 399
  addr = 200;
  for (int i=0; i < 4; i++)
  {EEPROM_readAnything(addr+i*10, TimerTuning[i]);} 
/*  
  TimerTuning[0] // Адрес 200
  TimerTuning[1] // Адрес 210
  TimerTuning[2] // Адрес 220
  TimerTuning[3] // Адрес 230
*/     
  EEPROM.end(); 

  //Serial.println("EEPROM_READ_Stop");
}
