/**************************************************************

                            Settings
                            Настройки

 **************************************************************/


/*
   Over The Air Hostname
*/
#define OTA_UPDATES
#define OTA_HOSTNAME              "DHT-MONITOR"
/*
   Local Server Settings (uncomment to use local server)
   Настройки Локального Сервера (разкоментируйте если нужно использовать)
*/
//#define USE_LOCAL_SERVER
#define SERVER                      IPAddress(192, 168, 1, 2) // Свой IP пишите

/*
   База - Цветов для светодиодов и цвета букв
*/
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"
#define BLYNK_WHITE     "#FFFFFF"

/*
   База - GPIO
*/
// пин к которому ципляем датчик
#define DHTPIN D7           // D7 и 13(GPIO) это одно и то же. 
#define ONE_WIRE_BUS D3 // PIN куда подключена шина ONE_WIRE (DS18B20)

#define RELAY1PIN D0  // PIN реле 1 Теплый пол
#define RELAY2PIN D4  // PIN реле 2 Аварийный нанреватель

// Для RTC DS3231
// Подключаем SCL к D1
// Подключаем SDA к D2

/*
   Virtual Pins - Base
   База - Виртуальных пинов
*/

#define VPIN_TABLE V10 // Задаем виртуальный пин виджета Value Display
#define VPIN_MENU V11 // Задаем виртуальный пин виджета Value Display
#define VPIN_TIME_INPUT V12 // Задаем виртуальный пин виджета Time Input
#define VPIN_STEP_CONTROL V13 // Задаем виртуальный пин виджета Step Control
#define VPIN_BUTTON V14 // Задаем виртуальный пин виджета Button

/*
   База - Переменных
*/
////////////////////////////////////////////////////
//            Данные хранятся в EEPROM            //

// Данные об Уставках
  // Массив уставок температуры
    int16_t TempTuning[4]; //= {10, 15, 20, 25};
  // Массив уставок температуры
    uint32_t TimerTuning[4]; //= {36000, 43200, 54000, 82800};
  // Гистерезис Дельта температуры
    int8_t Delta; //=2;
  // Настройки Аварийного нагревателя
    int8_t AlarmTempStart; //Температура Включения  
    int8_t AlarmTempStop;  //Температура Выключения
// Флаги
  uint8_t flag_AutoManual;
  uint8_t flag_TableStop=true;

//            Данные хранятся в EEPROM            //
////////////////////////////////////////////////////

// Данные от датчиков
  int8_t Temp1; // Температура от датчика 1
  int8_t Humid1; // Влажность от датчика 1

  #define TEMP_SUM 2 // Количество датчиков
  int8_t TempDS18B20[TEMP_SUM];
  // 0 Датчик в помещении
  // 1 Датчик в полу

  uint32_t TimerBlynk; // Текущее время в секундах от Blynk
  int8_t T_hourBlynk; // Текущее время сколько часов
  int8_t T_minBlynk; // Текущее время сколько минут

  uint32_t TimerRTC; // Текущее время в секундах от DS3231
  int8_t T_hourRTC; // Текущее время сколько часов
  int8_t T_minRTC; // Текущее время сколько минут

  String currentTimeRTC;
  String currentTimeBlynk;
  
// Флаги выходных устройств
   boolean flag_RELAY1=false;
   boolean flag_A_logicRegulatorT=false;
   boolean flag_RELAY2_Alarm=false;

  
//timezone Временная зона
  //full list of supported timezones could be found here
  //Полный список поддерживаемых часовых поясов можно найти здесь
  //https://www.mkyong.com/java/java-display-list-of-timezone-with-gmt/
  char TimeZone[] = "Asia/Krasnoyarsk";

// Переменные для настройки параметров
  long TimerInput;  // Тут хранятся данные от TimerInput (Ввод времени)
  int StepControl; // Тут хранятся данные от StepControl (Шаговые кнопки)
  boolean Button; // Тут храниться данные от Нажата ли кнопка

  int Tuning; // служит для настройки всех уставок через StepControl (Шаговые кнопки)
  long TunTime; // служит для настройки всех уставок через TimerInput (Ввод времени)

/*
   База - Таймеров
*/
int timerTable_ID; // Номер таймера Таблица
int timerTimeString_ID; // Номер таймера TimeString
int ID_timer_RTS; // Номер Таймера для установки времени

#define TintervalSensor1 2000L





