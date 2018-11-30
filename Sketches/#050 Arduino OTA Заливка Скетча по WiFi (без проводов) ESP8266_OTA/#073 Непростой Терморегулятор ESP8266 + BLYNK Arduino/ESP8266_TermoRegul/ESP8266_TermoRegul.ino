/*****************************************************************************   
 *    Автор: Обушенков Алексей Андреевич 
 *    Группа в ВК https://vk.com/engineer24
 *    Канал на YouTube https://www.youtube.com/channel/UCih0Gcl9IEdkR8deDJCiodg
 *    Инженерка Engineering room
 *    
 *    
 *****************************************************************************
 * Скетч собран по материалам сайта http://docs.blynk.cc/
 *****************************************************************************
 * Модули в составе проекта
 * ESP8266 - 12E WIFI RobotDyn (3.68 $) https://goo.gl/k6TRUz
 * ESP8266 - 12E WIFI GREAT WALL (3.44 $) https://goo.gl/DcqYMg
 *  
 * Датчики для улицы:
 *  DHT 11 (1.00 $) https://goo.gl/sCBn3d  Тут дешево но долго шло
 *  DHT 11 (3.11 $) https://goo.gl/rBFbBD  Тут дороже но дойдет быстрее
 *  Желательно брать этот, скоро будет вариант и с ним.
 *  GY-BME280-3.3 (3.78 $) https://goo.gl/1eyGmg
 *  
 * Модуль часов реального времени
 *  DS3231 Модуль RTC (1.95 $) https://goo.gl/3jMusY
 *  RTC DS3231 (часы реального времени)RobotDyn + аккумулятор (2.90 $) https://goo.gl/gGMRak
 *  
 * Датчики температуры в помещении и в полу
 *  DS18B20 Датчик на плате (2.11 $) https://goo.gl/T4AmmR
 *  DS18B20 Датчик температуры в корпусе (2.07 $) https://goo.gl/HmbgWM
 *  
 * Реле (выбор реле зависит от мощности нагрузки) 
 *  1-канальное реле с управлением Высоким и Низким уровнем 10Ампер (0,99 $) https://goo.gl/SnFuXY
 *  1-канальное реле с управлением Высоким и Низким уровнем 30Ампер (3.96 $) https://goo.gl/PW1uYL
 * 
 *****************************************************************************
 * Библиотеки
 *****************************************************************************
 * BLYNK   
 *    https://github.com/blynkkk/blynk-library/releases/tag/v0.5.0
 *    
 * OTA Видео об этом https://youtu.be/AyGs8ptdZJc   
 *    Для возможности прошивки по сетевому порту,
 *    необходимо установить последнюю версию python 
 *    Скачать по ссылке: https://www.python.org/downloads/
 *   
 *    Ссылка на библиотеку Aduino OTA.
 *    https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA
 *    Ссылка которые стоит посетить. Там море инфы об беспроводной прошивке.
 *    http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html
 * 
 * EEPROM
 *    EEPROMAnything Видео о Библиотеке https://youtu.be/xBHor5iW5OE
 *    Материалы к бибилиотеки из видео https://yadi.sk/d/BAYkOBHM3RXA7P
 *
 * Ссылки на библиотеки для модулей
 *    DHT (Температура и Влажность)
 *      https://github.com/adafruit/Adafruit_Sensor
 *      https://github.com/adafruit/DHT-sensor-library
 *    BME280 (Температура, Влажность и Давление)
 *      https://github.com/adafruit/Adafruit_BME280_Library.git
 *    DS18B20 (Термомитер)
 *      https://github.com/milesburton/Arduino-Temperature-Control-Library.git
 *    RTC DS3231 (Часы реального времени)
 *      https://github.com/rodan/ds3231.git
 *    
 *    
 *    
 *    
 *****************************************************************************
 * В чем суть скетча
 *****************************************************************************
 *  Мониторинг и контроль температуры.
 *  Датчики:
 *   1)Есть датчик для улицы (просто контроль температуры, влажности, давления)
 *   2)Есть датчик температуры воздуха в помещении и датчик температуры теплых полов.
 *   
 *  Исполнительные механизмы (Есть два нагревателя):
 *   1)Тепловая пушка (Условно назовем ее так, для того что бы в случае чего резко нагреть помещение)
 *   2)Теплый пол
 *   
 *  Логика "Тепловой пушки": 
 *    Задаем температуру Старт и температуру Стоп нагрев. 
 *    Если температура стала ниже старт то включится нагрев и будет греть пока температура не
 *    станет выше Стоп. Нагрев включится опять толь тогда когда температура станет опять ниже Старт.
 *  Логика "Теплого пола"
 *    Есть 4 таймера(Сутки разбиты на 4 части). Каждый таймер включает свою уставку Температуры.
 */
#include "Settings.h"

#include "wifi_credentials.h" // Личные настройки
#include "RegulatorT.h" // Функция регулятора температуры
#include "F_EEPROM.h" // Функции Энергонезависимой памяти
#include "Sensors.h" // Функции датчиков

#include "BlynkF.h" // Функции Blynk


#ifdef OTA_UPDATES      //Если OTA включена
#include <ArduinoOTA.h> //подключаем OTA Библиотека для OTA-прошивки
#endif


void setup() {

 Serial.begin(9600);
 WiFi.mode(WIFI_STA);
 
// Функция чтения данных из Энергонезовисимой памяти EEPROM
  EEPROM_READ();
  
// Включаем модуль DS3231 (RTC - Часы реального времени)
  clock.begin();

// Настраиваем выхода
 pinMode (RELAY1PIN,OUTPUT);
 pinMode (RELAY2PIN,OUTPUT);

// Запускаем DS18B20
  setSetupDS18B20();
 
  // CONNECT TO BLYNK
#ifdef USE_LOCAL_SERVER // Если используются локальный сервер
  Blynk.begin(AUTH, WIFI_SSID, WIFI_PASS, SERVER);
#else // Иначе конектимся постаринке к блинку
  Blynk.begin(AUTH, WIFI_SSID, WIFI_PASS);
#endif

// До бесконечности будем оставаться в цикле while
// пока не установим связь с сервером
//  while (Blynk.connect() == false) {} 

  
#ifdef OTA_UPDATES // Если Прошивка по воздуху подключена
  // SETUP OVER THE AIR UPDATES Настройки для прошивки по воздуху
  ArduinoOTA.setHostname(OTA_HOSTNAME); //OTA Задаем имя сетевого порта
  //ArduinoOTA.setPassword((const char *)"0000"); //OTA Задаем пароль доступа для удаленной прошивки
  ArduinoOTA.begin();//OTA Инициализируем OTA
#endif
  
// Запускаем DHT датчик 
   dht.begin(); 

// Настраиваем таймеры   
   timer.setInterval(TintervalSensor1, Sensor1); // Раз в 2 секунду выполняем Sensor1 (DHT11)Для улицы
   timer.setInterval(1000, updateTemp); // Раз в секунду опрашиваем датчики DS18B20
   
   //timer.setInterval(1000, RTC_Time);  // Раз в секунду считываем с часов реального времени Время (RTC_Blynk)
   timer.setInterval(1000,RTCgetTime); // Раз в секунду считываем с часов реального времени Время (RTC_DS3231)
      
   timerTable_ID=timer.setInterval(1000, table);
   timer.setInterval(1000, logic);
   
   timerTimeString_ID=timer.setInterval(100,TimeString);
   timer.disable(timerTimeString_ID);

/*
   Так можно включать и выключать таймеры
   timer.disable(timerID);
   timer.enable(timerID);
*/

//Создаем меню
  Blynk.setProperty(VPIN_MENU, "label", "Меню");// Меняем название виджета
  Blynk.setProperty(VPIN_MENU, "labels", "Режим работы", "Настройки", "Установка времени");

//Настраиваем Time Input
  Blynk.setProperty(VPIN_TIME_INPUT, "Label", ".");
  Blynk.virtualWrite(VPIN_TIME_INPUT, 0, 0, TimeZone); // Обнуляем виджет 

//Настраиваем Step Control
  Blynk.setProperty(VPIN_STEP_CONTROL, "Label", "Step Control");
  Blynk.virtualWrite(VPIN_STEP_CONTROL, 0); // Обнуляем виджет

//Настраиваем кнопку
  Blynk.setProperty(VPIN_BUTTON, "Label", ".");
  Blynk.setProperty(VPIN_BUTTON, "onLabel", "Запись значения");
  Blynk.setProperty(VPIN_BUTTON, "offLabel", "Записать значение");

// Запускаем Часы реального времени
  rtc.begin(); // запускаем виджет часов реального времени
  
} //setup

void loop() {
ArduinoOTA.handle(); // OTA Всегда готовы к прошивке

Blynk.run();
timer.run();
}
