/*
  DS3231: Real-Time Clock. Simple example
  Read more: www.jarzebski.pl/arduino/komponenty/zegar-czasu-rzeczywistego-rtc-ds3231.html
  GIT: https://github.com/jarzebski/Arduino-DS3231
  Web: http://www.jarzebski.pl
  (c) 2014 by Korneliusz Jarzebski
*/

// Подключаем SCL к D1
// Подключаем SDA к D2

#include <Wire.h>
#include <DS3231.h>

DS3231 clock;
RTCDateTime dt;

////////////////////////////////////////
// ЧАСЫ

// Забераем данные из Модуля часов реального времени и сохраняем в переменные
// для последующей работы с ними
void RTCgetTime()
{
  dt = clock.getDateTime();

//  Serial.print("Raw data: ");
//  Serial.print(dt.year);   Serial.print("-");
//  Serial.print(dt.month);  Serial.print("-");
//  Serial.print(dt.day);    Serial.print(" ");
//  Serial.print(dt.hour);   Serial.print(":");
//  Serial.print(dt.minute); Serial.print(":");
//  Serial.print(dt.second); Serial.println("");
  
  T_hourRTC=dt.hour;
  T_minRTC=dt.minute;
  TimerRTC = T_hourRTC*3600 + T_minRTC*60;
}

// сохраняет занчение времени от RTC Blynk в RTC модуль DS3231
void RTCsetTime()
{
  // Manual (YYYY, MM, DD, HH, II, SS
  // clock.setDateTime(2014, 4, 13, 19, 21, 00);
  
  clock.setDateTime(year(), month(), day(), hour(), minute(), second());
}

// Функция собирает время в формате строки что бы отправить в меню Blynk
void TimeString ()
{
  currentTimeRTC = String(dt.year)+ "/" + dt.month + "/" + dt.day + " " + dt.hour + ":" + dt.minute;
  currentTimeBlynk = String(year())+ "/" + month()+ "/" + day() + " " + hour() + ":" + minute();
}

// Функция по нажатию кнопки, вызывает функцию которая сохраняет занчение времени от RTC Blynk
// в RTC модуль DS3231 и переходит назад по Меню
void RTC_BlynkForDS3231()
{
    if (Button == HIGH){
    timer.disable(timerTimeString_ID);
    RTCsetTime();
    timer.deleteTimer(ID_timer_RTS);
    Blynk.setProperty(VPIN_MENU, "labels", "Режим работы", "Настройки", "Установка времени");
    Index=0; Blynk.virtualWrite(VPIN_MENU, 0);
    timer.enable(timerTable_ID);
    Blynk.virtualWrite(VPIN_STEP_CONTROL, 0); // Обнуляем виджет
    Blynk.virtualWrite(VPIN_TIME_INPUT, 0, 0, TimeZone); // Обнуляем виджет
  }
}  
// ЧАСЫ
////////////////////////////////////////

