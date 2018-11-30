/////////////////////////////////////////
//              DHT  Датчик №1        //

#include <SPI.h> // DHT работает по SPI
#include <DHT.h> // библиотека датчика

// Выбираем конкретный тип датчика
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE); //инициализируем датчик

void Sensor1()
{
  
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // или dht.readTemperature(true) для вывода по Фаренгейту
  
  
  if (isnan(h) || isnan(t)) {      // видно, что тут, проверяется полученно ли значение
    Serial.println("Failed to read from DHT sensor!");
    return; //Прекращает вычисления в функции, то есть ждем когда следующий раз обратимся к функции Sensor
  }
  Humid1 = int(h);
  Temp1 = int(t);


  
}//Sensor1

//              DHT  Датчик №1        //
/////////////////////////////////////////

/////////////////////////////////////////
//       DS18B20  Датчики №2-3        //

// Подключаем библиотеки
#include <OneWire.h>
#include <DallasTemperature.h>


//#define ONE_WIRE_BUS D3 //PIN куда подключена шина ONE_WIRE (DS18B20)(Определено в  Settings.h)
#define TEMPERATURE_PRECISION 9 // Точность температуры
// #define TEMP_SUM 2 // Количество датчиков (Определено в  Settings.h)

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
// Настройка экземпляра oneWire для взаимодействия с любыми устройствами OneWire (а не только с температурными микросхемами Maxim/Dallas)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
// Передаем нашу ссылку oneWire на Даллас Температуру.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
// массивы для хранения адресов устройств
DeviceAddress Thermometer[TEMP_SUM];

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void setSetupDS18B20()
{
  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // Задаем адреса через индекс датчика
  for (int i=0; i < TEMP_SUM; i++)
  {if (!sensors.getAddress(Thermometer[i], i)) Serial.print("Unable to find address for Device "); Serial.println(i);}
  
  // Задаем точность датчика
  for (int i=0; i < TEMP_SUM; i++)
  {sensors.setResolution(Thermometer[i], TEMPERATURE_PRECISION);}

  // Показываем адреса датчиков
  for (int i=0; i < TEMP_SUM; i++)
  {Serial.print("Device ");Serial.print(i); Serial.print(" : ");printAddress(Thermometer[i]);Serial.println();}

  // Показываем точность датчиков
  for (int i=0; i < TEMP_SUM; i++)
  {Serial.print("Resolution Device ");Serial.print(i);Serial.print(" : ");
  Serial.print(sensors.getResolution(Thermometer[i]), DEC);
  Serial.println();}
}//setSetupDS18B20

void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
//  Serial.print(" Temp F: ");
//  Serial.print(DallasTemperature::toFahrenheit(tempC));
}

void updateTemp()
{
  sensors.requestTemperatures();

  for (int i=0; i < TEMP_SUM; i++)
  {TempDS18B20[i]= int(sensors.getTempC(Thermometer[i]));}
  
}
//       DS18B20  Датчики №2-3        //
/////////////////////////////////////////
