// PIN - Пин выходного устройства
// Tin - Температрура от датчика
// Tset - Какую темпратуру поддерживать
// Tdelta - Дельта или Гистерезис

// Основной терморегулятор
void RegulatorT(int PIN, int Tin, int Tset, int Tdelta)
{
  //Если Текущая температура меньше Уставки-Дельта
  if (Tin <= Tset-Tdelta)
  {
  digitalWrite(PIN, HIGH); // Включить реле
  flag_RELAY1=true;
  //Serial.print(" RelayT1 ON ");
  }

  // иначе если Текущая температура выше Устави+Дельта
  else if (Tin >= Tset-Tdelta)
  {
  digitalWrite(PIN, LOW); // Выключить реле
  flag_RELAY1=false;
  //Serial.print(" RelayT1 OFF ");
  }
}//RegulatorT

// Аварийный нагреватель
void AlarmTemp(int PIN, int Tin, int TempStart, int TempStop)
{
  // Если текущая температура меньше или равно Температуры Включения
  if (Tin <= TempStart)
  {
    digitalWrite(PIN, HIGH); // Включить реле
    flag_RELAY2_Alarm=true;
    //Serial.print(" RelayT Alarm 2 ON ");
  }

  // Если текущая температура больше или равно Температура Выключения
  if (Tin >=TempStop)
  {
    digitalWrite(PIN, LOW); // Выключить реле
    flag_RELAY2_Alarm=false;
    //Serial.print(" RelayT Alarm 2 OFF ");
  }  
}

// Логика работы устройства
// Мы просто цыклично вызываем эту функцию
void logic()
{
  if (flag_AutoManual == true) // Если включен автоматический режим
   { 
    int i=0;
   // Проверяем какой сейчас таймер Активен и не равен ли он "нулю"
   // и выбираем соответствующий номер уставки для регулятора температуры
   // Условия вызываются последовательно и в случае верности изменят i
    if (TimerRTC>0 && TimerTuning[3]!=0){i=3;}
    if (TimerRTC>TimerTuning[0] && TimerTuning[0]!=0){i=0;}
    if (TimerRTC>TimerTuning[1] && TimerTuning[1]!=0){i=1;}
    if (TimerRTC>TimerTuning[2] && TimerTuning[2]!=0){i=2;}
    if (TimerRTC>TimerTuning[3] && TimerTuning[3]!=0){i=3;}

      // Проверяем не равны ли все Таймеры 0
      uint32_t check=0;
      for(int j=0; j<4; j++){check=check+TimerTuning[j];}
      if (check!=0)
        { flag_A_logicRegulatorT=false;
          // PIN - Пин выходного устройства // Tin - Температрура от датчика
          // Tset - Какую темпратуру поддерживать // Tdelta - Дельта или Гистерезис
          RegulatorT(RELAY1PIN, TempDS18B20[0], TempTuning[i], Delta);
        }
        else
          {digitalWrite(RELAY1PIN, LOW); flag_A_logicRegulatorT=true;} // Выключить реле
   }
    else
      {digitalWrite(RELAY1PIN, LOW); flag_RELAY1=false;} // Выключить реле

// Аварийный нагреватель нельзя выключить,
// только если уравнять значения AlarmTempStart и AlarmTempStop
  AlarmTemp(RELAY2PIN, TempDS18B20[1], AlarmTempStart, AlarmTempStop);
  
}//logic

