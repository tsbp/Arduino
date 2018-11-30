#include <ESP8266WiFi.h> //Библиотека для работы с WIFI 
//#include <ESP8266mDNS.h>
//#include <WiFiUdp.h>

//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <BlynkSimpleEsp8266.h>



// Библиотеки для часов реального времени
#include <TimeLib.h>
#include <WidgetRTC.h>

WidgetRTC rtc; // инициализируем виджет часов реального времени
BlynkTimer timer;         //инициализируем таймер


// Переменные для Меню
  
  int Item=0;     // Тут храним индекс получаемый от Виджета Menu
  int Index=0;    // Дополнительный индекс нужен для навигации по вкладкам
  int MenuIndex=0;  // Индекс для перехода по меню при помощи switch case
// Переменные что бы действие выполнялось один раз
  int MenuIndexLast=0; // Тут Храним последнее значение Индекса
  boolean one = false; // Переменная что бы выполнить условие один раз

#include "RTC_DS3231.h" // Таймер реального времени RTC (DS3231)

// Преобразование секунд (int) в Строку (String) в формате времени
String SecInTime (int Sec)
{
  String Time; // Создаем переменную Строка Time
  int Hour = Sec/3600; // Вычисляем количество часов
  int Min = (Sec-Hour*3600)/60; // Вычисляем количество минут
  String SHour; // Создаем переменную Строка SHour
  String SMin;  // Создаем переменную Строка SMin
  
  if (Hour<10){SHour = "0" + String(Hour);} else {SHour=String(Hour);} // Если часов меньше 10 то добовляем 0 в строку
  if (Min<10){SMin = "0" + String(Min);} else {SMin=String(Min);} // Если минут меньше 10 то добовляем 0 в строку
  
  Time = SHour + ":" + SMin; // Собераем строку в формате время
  return Time; // Возвращаем результат
}

void SelectSetTwo (int Number)
//    (int Number) //    Number(Номер уставки)
{
      // Тут нужно запустить обработчик нажатий Time Input и Step Control
      // Причем перерисовать таблицу и вывести изменяемое значение в таблицу
      // и по нажатию кнопки Button записать значение и выйти
      int i = Number-1;
      String SNumber = String (Number);
      String STime = "Время " + SNumber;
      String STemp = "Уставка " + SNumber;
      
      if (MenuIndex != MenuIndexLast){one = false;MenuIndexLast=MenuIndex;}
      
      
      if (one == false) // Если ложь
       {  one = true; // Запрещаем выполнение следующих строк еще раз
          Blynk.virtualWrite(VPIN_STEP_CONTROL, TempTuning[i]); // Передаем старые данные в виджет 
          StepControl = TempTuning[i]; // Передаем старые данные в переменную котораяф хранит данные
          Blynk.virtualWrite(VPIN_TIME_INPUT, TimerTuning[i], 0, TimeZone); // Передаем старые данные в виджет
          TimerInput = TimerTuning[i]; // Передаем старые данные в переменную котораяф хранит данные
       } 
      
      if (TunTime != TimerInput)
      {TunTime = TimerInput;Blynk.virtualWrite(VPIN_TABLE, "update", Number*2, STime, SecInTime(TunTime));}
      
      if (Tuning != StepControl)
      {Tuning = StepControl; Blynk.virtualWrite(VPIN_TABLE, "update", Number*2+1, STemp, Tuning);}
      
      if (Button == HIGH){
        //one = false;
        TempTuning[i] = Tuning;
        Blynk.virtualWrite(VPIN_STEP_CONTROL, 0); // Обнуляем виджет
        
        TimerTuning[i] = TimerInput;
        Index=(MenuIndex/10)*10; Blynk.virtualWrite(VPIN_MENU, 0);
        Blynk.virtualWrite(VPIN_TIME_INPUT, 0, 0, TimeZone); // Обнуляем виджет 

        EEPROM.begin(512);
        EEPROM_writeAnything((100+i*10), TempTuning[i]); // записали число Temp по адресу addr
        EEPROM_writeAnything((200+i*10), TimerTuning[i]); // записали число Time по адресу addr        
        EEPROM.end();        // закончили 
        }     
}//SelectSetTwo

void SelectSetOne (String SVariable)
// (String SVariable)
//  SVariable(Название переменной в виде строки)
{ 
/*
 
Указатель – переменная особого типа, которая численно равна адресу
            в памяти той переменной, на которую она(эта переменная-указатель) указывает.
            Два основных оператора для работы с указателями – это оператор & взятия адреса, и оператор * разыменования. 
[&] Операция взятия адреса является одной из важнейших,
    связанных с указателями. Она обозначается знаком &, является унарной операцией
    и ее операндом может быть только переменная или элемент массива,
    причем тип этой переменной может быть любым допустимым типом данных Си.

[*] Для обращения к объекту, на который указывает указатель (адрес которого хранится в указателе),
    используется оператор * (звёздочка), называемый оператором разыменования или косвенным обращением.

*/
       
      int8_t *Variable; // Создаем указатель на адресс переменной
      int addr; // Переменная для хранения адреса EEPROM
      int LineNumber;  // Номер строки в табилице. (Куда выводить данные)
      String Name;     // Название переменной. (Для таблицы)
  if (SVariable == "Delta")
    {Variable = &Delta; addr = 20; Name = "Дельта (Гистерезис)"; LineNumber = 1;}
  else if (SVariable =="AlarmTempStart")
    {Variable = &AlarmTempStart; addr =24; Name = "Температура Включения"; LineNumber = 1;}
  else if (SVariable =="AlarmTempStop")
    {Variable = &AlarmTempStop; addr =28; Name = "Температура Выключения"; LineNumber = 2;}

      if (MenuIndex != MenuIndexLast){one = false;MenuIndexLast=MenuIndex;}

      if (one == false) // Если ложь
       {  one = true; // Запрещаем выполнение следующих строк еще раз
          Blynk.virtualWrite(VPIN_TIME_INPUT, 0, 0, TimeZone); // Обнуляем виджет
          Blynk.virtualWrite(VPIN_STEP_CONTROL, *Variable); // Передаем старые данные в виджет 
          StepControl = *Variable; // Передаем старые данные в переменную которая хранит данные
       } 
      Tuning = StepControl;
      Blynk.virtualWrite(VPIN_TABLE, "update", LineNumber, Name, Tuning);
      if (Button == HIGH){
        //one = false;
        *Variable = Tuning;
        Index=(MenuIndex/10)*10; Blynk.virtualWrite(VPIN_MENU, 0);
        EEPROM.begin(512);
        EEPROM.write(addr, *Variable); // записали число Variable по адресу addr 
        delay(100);       
        EEPROM.end();        // закончили 
        }
}//SelectSetOne

 

/////////////////
//     Меню

BLYNK_WRITE(VPIN_MENU)
{
  
  Item = param.asInt();
  MenuIndex = Item+Index;
  
 
  switch (MenuIndex)
  {
    case 1: // Режим работы
      Blynk.setProperty(VPIN_MENU, "labels", "Назад", "Выключить", "Автоматический");
      Index=10; // Вперед на уровень 1
      Blynk.virtualWrite(VPIN_MENU, 0); // Это нужно что бы ничего не было выбрано
      break;
    case 2: // Настройки
      Blynk.setProperty(VPIN_MENU, "labels", "Назад", "Теплый пол", "Аварийный нагреватель");
      Index=20; // Вперед на уровень 2
      Blynk.virtualWrite(VPIN_MENU, 0);
      break;
    case 3: // Настройки Времени
      
      Blynk.setProperty(VPIN_MENU, "labels", "Назад", "Установите время");
      Index=60; // Вперед на уровень 6
      Blynk.virtualWrite(VPIN_MENU, 0);
        timer.enable(timerTimeString_ID);
        timer.disable(timerTable_ID); // Останавливаем вывод даных в таблицу
        flag_TableStop = true;
       timer.setTimer(500, [](){
        Blynk.virtualWrite(VPIN_TABLE, "clr"); //clean table at start // сначала нужно отчистить
        Blynk.virtualWrite(VPIN_TABLE, "add", 1, "Установка времени", " ");
        Blynk.virtualWrite(VPIN_TABLE, "add", 2, "Время RTC", currentTimeRTC);
        Blynk.virtualWrite(VPIN_TABLE, "add", 3, "Время Blynk", currentTimeBlynk);
          ID_timer_RTS = timer.setInterval(1000,[](){
          Blynk.virtualWrite(VPIN_TABLE, "update", 1, "Установка времени", " ");
          Blynk.virtualWrite(VPIN_TABLE, "update", 2, "Время RTC", currentTimeRTC);
          Blynk.virtualWrite(VPIN_TABLE, "update", 3, "Время Blynk", currentTimeBlynk);   
          }); 
          
       }, 1);
      break;      
      
////////////////////////////////
//     Выбор режимов работы   Уровень1    
    case 11: // Назад на 1 уровень
      Blynk.setProperty(VPIN_MENU, "labels", "Режим работы", "Настройки", "Установка времени");
      Index=0; // Назад на уровень 1
      Blynk.virtualWrite(VPIN_MENU, 0);
      break;
    case 12: // Ручной режим OFF
      flag_AutoManual = false; // Система Выключена OFF
      EEPROM.begin(512);
      EEPROM.write(0, flag_AutoManual); // записали число Variable по адресу addr 
      EEPROM.end();        // закончили  
      Blynk.setProperty(VPIN_MENU, "labels", "Режим работы", "Настройки", "Установка времени");     
      Index=0; Blynk.virtualWrite(VPIN_MENU, 0);
      break;
    case 13: // Автоматический режим ON
      flag_AutoManual = true; // Система Включена ON
      EEPROM.begin(512);
      EEPROM.write(0, flag_AutoManual); // записали число Variable по адресу addr 
      EEPROM.end();        // закончили  
      Blynk.setProperty(VPIN_MENU, "labels", "Режим работы", "Настройки", "Установка времени");   
      Index=0; Blynk.virtualWrite(VPIN_MENU, 0);
      break;
//     Выбор режимов работы    Уровень1
////////////////////////////////

////////////////////////////////   
//     Меню настроек           Уровень2
    case 21: // Назад на уровень 1
      Blynk.setProperty(VPIN_MENU, "labels", "Режим работы", "Настройки", "Установка времени");
      Index=0; // Назад на уровень 1
      Blynk.virtualWrite(VPIN_MENU, 0);      
      break;
    case 22: // Теплый пол
      timer.disable(timerTable_ID); // Останавливаем вывод даных в таблицу
      flag_TableStop = true;
      Blynk.setProperty(VPIN_MENU, "labels", "Назад", "Дельта", "Время и Температура");
      Index=30; // Вперед на уровень 3
      Blynk.virtualWrite(VPIN_MENU, 0); 
        // Создаем таблицу с данными Теплого пола
          Blynk.virtualWrite(VPIN_TABLE, "clr"); //clean table at start // сначала нужно отчистить
          Blynk.virtualWrite(VPIN_TABLE, "add", 1, "Дельта (Гистерезис)", Delta);
          Blynk.virtualWrite(VPIN_TABLE, "add", 2, "Время 1", SecInTime(TimerTuning[0]));
          Blynk.virtualWrite(VPIN_TABLE, "add", 3, "Уставка 1", TempTuning[0]);
          Blynk.virtualWrite(VPIN_TABLE, "add", 4, "Время 2", SecInTime(TimerTuning[1]));
          Blynk.virtualWrite(VPIN_TABLE, "add", 5, "Уставка 2", TempTuning[1]);  
          Blynk.virtualWrite(VPIN_TABLE, "add", 6, "Время 3", SecInTime(TimerTuning[2]));
          Blynk.virtualWrite(VPIN_TABLE, "add", 7, "Уставка 3", TempTuning[2]); 
          Blynk.virtualWrite(VPIN_TABLE, "add", 8, "Время 4", SecInTime(TimerTuning[3]));
          Blynk.virtualWrite(VPIN_TABLE, "add", 9, "Уставка 4", TempTuning[3]);                            
                
      break;
    case 23: // Аварийный нагреватель
      timer.disable(timerTable_ID);
      flag_TableStop = true;
      Blynk.setProperty(VPIN_MENU, "labels", "Назад", "Температура включения", "Температура выключения");
      Index=50; Blynk.virtualWrite(VPIN_MENU, 0); // Вперед на уровень 5
       // Создаем таблицу с данными Аварийного нагревателя
          Blynk.virtualWrite(VPIN_TABLE, "clr"); //clean table at start // сначала нужно отчистить
          Blynk.virtualWrite(VPIN_TABLE, "add", 1, "Температура Включения", AlarmTempStart);
          Blynk.virtualWrite(VPIN_TABLE, "add", 2, "Температура Выключения", AlarmTempStop);              
      break; 
//     Меню настроек           Уровень2
//////////////////////////////// 


////////////////////////////////   
//     Меню теплого пола       Уровень3   
    case 31: // Назад
      Blynk.setProperty(VPIN_MENU, "labels", "Назад", "Теплый пол", "Аварийный нагреватель");
      Index=20; Blynk.virtualWrite(VPIN_MENU, 0);
      timer.enable(timerTable_ID);      
      break;
    case 32: // Дельта
      SelectSetOne ("Delta");
      // (String SVariable, int indexBack)
      //  SVariable(Название переменной в виде строки)
      break;
    case 33: // Время и температура
      Blynk.setProperty(VPIN_MENU, "labels", "Назад", "Время и Уставка 1", "Время и Уставка 2", "Время и Уставка 3", "Время и Уставка 4");
      Index=40; Blynk.virtualWrite(VPIN_MENU, 0);         
      break; 
//     Меню теплого пола       Уровень3                 
////////////////////////////////   

               
////////////////////////////////
//     Меню уставок Времени и Температуры       Уровень4  
    case 41: // Назад
      Blynk.virtualWrite(VPIN_MENU, 0); Index=30; // Назад на уровень 3
      Blynk.setProperty(VPIN_MENU, "labels", "Назад", "Дельта", "Время и Температура");
      Blynk.virtualWrite(VPIN_STEP_CONTROL, 0); // Обнуляем виджет
      Blynk.virtualWrite(VPIN_TIME_INPUT, 0, 0, TimeZone); // Обнуляем виджет
      break; 
    case 42: // Время и Уставка 1
      SelectSetTwo (1);
      //    (int Number) //    Number(Номер уставки)
      break; 
    case 43: // Время и Уставка 2
      SelectSetTwo (2);
      //    (int Number) //    Number(Номер уставки)
      break; 
    case 44: // Время и Уставка 3
      SelectSetTwo (3);
      //    (int Number) //    Number(Номер уставки)
      break; 
    case 45: // Время и Уставка 4
      SelectSetTwo (4);
      //    (int Number) //    Number(Номер уставки)
      break; 
//     Меню уставок Времени и Температуры       Уровень4      
////////////////////////////////
  
      
////////////////////////////////  
//     Меню уставок Авариного нагревателя       Уровень5    
    case 51: // Назад
      Blynk.setProperty(VPIN_MENU, "labels", "Назад", "Теплый пол", "Аварийный нагреватель");
      Index=20; Blynk.virtualWrite(VPIN_MENU, 0);
      timer.enable(timerTable_ID); 
      break;
    case 52: // Температура Включения
      SelectSetOne ("AlarmTempStart");
      // (String SVariable, int indexBack)
      //  SVariable(Название переменной в виде строки)
      break;
    case 53: // Температура Выключения
      SelectSetOne ("AlarmTempStop");
      // (String SVariable, int indexBack)
      //  SVariable(Название переменной в виде строки)
      break;      
//     Меню уставок Авариного нагревателя       Уровень5      
//////////////////////////////// 

////////////////////////////////
//     Меню уставок Времени                     Уровень6      
    case 61: // Назад
      timer.deleteTimer(ID_timer_RTS);
      Blynk.setProperty(VPIN_MENU, "labels", "Режим работы", "Настройки", "Установка времени");
      Index=0; Blynk.virtualWrite(VPIN_MENU, 0);
      timer.enable(timerTable_ID); 
      break;
    case 62: // Температура Включения
      RTC_BlynkForDS3231();

      break;
 
//     Меню уставок Времени                     Уровень6      
////////////////////////////////                              
    default:
      Serial.println("Unknown item selected");
      break;
  }
}


//     Меню
/////////////////

/////////////////
//Таблица

void table()
{
  String StatusName;
  String StatusValue;
  
 // Отображают статусы работы
  if (flag_AutoManual==true && flag_RELAY1==true) {StatusName = "Режим работы Автоматический"; StatusValue = "Нагрев";}
  else if (flag_AutoManual==true && flag_RELAY1==false) {StatusName = "Режим работы Автоматический"; StatusValue = "Норма";}
  else if (flag_AutoManual==false){StatusName = "Режим работы Выключен"; StatusValue = "Выключено";}

 // Выводит в строку что все таймеры отключены
  if (flag_A_logicRegulatorT==true){StatusName = "Все таймеры отключены"; StatusValue = "Выключено";}

 // Добавляет в строку данные включен ли аварийный нагреватель
  if (flag_RELAY2_Alarm==true){StatusName+="/Аварийный нагреватель"; StatusValue += "/Включен Авар.Нагрев.";}

  // Если вывод основной таблицы останавливали то создаем таблицу заново
  if (flag_TableStop)
  {
    flag_TableStop = false;
    // Создаем таблицу
      Blynk.virtualWrite(VPIN_TABLE, "clr"); //clean table at start // сначала нужно отчистить
  
      Blynk.virtualWrite(VPIN_TABLE, "add", 1, StatusName, StatusValue);
      
      Blynk.virtualWrite(VPIN_TABLE, "add", 2, "Время", SecInTime(TimerRTC)); 
       
      Blynk.virtualWrite(VPIN_TABLE, "add", 3, "Температура на улице", Temp1);
      Blynk.virtualWrite(VPIN_TABLE, "add", 4, "Влажность на улице", Humid1);
      Blynk.virtualWrite(VPIN_TABLE, "add", 5, "Давление на улице", "745 мм.ст.");

      Blynk.virtualWrite(VPIN_TABLE, "add", 6, "Температура в помещении", TempDS18B20[1]);
      Blynk.virtualWrite(VPIN_TABLE, "add", 7, "Температура теплого пола", TempDS18B20[0]);
  }

// Выводим данные в основную таблицу
  Blynk.virtualWrite(VPIN_TABLE, "update", 1, StatusName, StatusValue);
  Blynk.virtualWrite(VPIN_TABLE, "update", 2, "Время", SecInTime(TimerRTC));   
  Blynk.virtualWrite(VPIN_TABLE, "update", 3, "Температура на улице", Temp1);
  Blynk.virtualWrite(VPIN_TABLE, "update", 4, "Влажность на улице", Humid1);
  Blynk.virtualWrite(VPIN_TABLE, "update", 5, "Давление на улице", "745 мм.ст.");

  Blynk.virtualWrite(VPIN_TABLE, "update", 6, "Температура в помещении", TempDS18B20[1]);
  Blynk.virtualWrite(VPIN_TABLE, "update", 7, "Температура теплого пола", TempDS18B20[0]);
      
}

//Таблица
/////////////////


// Обработка TIME_INPUT
BLYNK_WRITE(VPIN_TIME_INPUT)
{

  TimerInput = param[0].asLong(); // Принимаем значение от TIME_INPUT в секундах
  Blynk.syncVirtual(VPIN_MENU); // Вызываем синхронизацию с виджетом Меню (обновляем данные в нем)
  
//  Serial.print("Timer1: ");
//  Serial.print(Timer1);
//  Serial.print("   ");
//  int T1_hour = Timer1/3600;
//  int T1_min = (Timer1-T1_hour*3600)/60;
//  Serial.print(T1_hour);Serial.print(":");Serial.print(T1_min);


}

// Обработка STEP_CONTROL
BLYNK_WRITE(VPIN_STEP_CONTROL)
{
  StepControl = param.asInt(); // Принимаем значение от STEP_CONTROL
  Blynk.syncVirtual(VPIN_MENU); // Вызываем синхронизацию с виджетом Меню (обновляем данные в нем)
}

// Обработка BUTTON
BLYNK_WRITE(VPIN_BUTTON)
{
  static int Button_last = 0;
  static int ButtonSW = 0;
  ButtonSW = param.asInt(); // Принимаем значение от BUTTON
  
if (ButtonSW==1 && Button_last==0) // Это Если сделанно для того что бы при нажатии на кнопку
                                   // Обновление Меню произошло один раз
  {Button_last = ButtonSW;
  Button=HIGH; // Кнопка нажата
  Blynk.syncVirtual(VPIN_MENU); // Вызываем синхронизацию с виджетом Меню (обновляем данные в нем)
  }
  
if (ButtonSW==0)
  {Button_last=0; Button=LOW;} // Кнопка отжата
}

/* 
 *  Если нет модуля RTC то можно обойтись виджетом.
void RTC_Time()
{
//      String currentTime = String(hour()) + ":" + minute();
//      Serial.print(" RTC ");Serial.println(currentTime);
            
      T_hourBlynk = hour();
      T_minBlynk = minute();
      TimerBlynk = T_hourBlynk*3600 + T_minBlynk*60;
//      Serial.print(" Timer: ");
//      Serial.print(Timer);
//      Serial.print("   ");      
//      Serial.print(" RTC: ");Serial.print(T_hourBlynk);Serial.print(":");Serial.println(T_minBlynk);
      
}
*/

// При подключении нашей ESP к серверу Blynk 
// выполним настройку виджетов в приложении на Планшете
BLYNK_CONNECTED() {

      flag_TableStop=true;
      timer.enable(timerTable_ID); 
      Index=0; Blynk.virtualWrite(VPIN_MENU, 0);
      Blynk.setProperty(VPIN_MENU, "labels", "Режим работы", "Настройки", "Установка времени");
      Blynk.virtualWrite(VPIN_STEP_CONTROL, 0); // Обнуляем виджет
      Blynk.virtualWrite(VPIN_TIME_INPUT, 0, 0, TimeZone); // Обнуляем виджет 

}
