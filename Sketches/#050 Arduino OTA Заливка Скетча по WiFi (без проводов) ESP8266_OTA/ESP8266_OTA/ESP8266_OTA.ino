/** Для возможности прошивки по сетевому порту,
 *  необходимо установить последнюю версию python 
 *  Скачать по ссылке: https://www.python.org/downloads/
 *  
 *   Автор: Обушенков Алексей Андреевич (КОД не мой! Я Это.... Автор видео.)
 *   Группа в ВК https://vk.com/engineer24
     Канал на YouTube https://www.youtube.com/channel/UCih0Gcl9IEdkR8deDJCiodg
     Инженерка Engineering room

Ссылка которые стоит поситить. Там море инфы об беспроводной прошивке.
http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html

Ссылка на библиотеку Aduino OTA хотя у меня уже была.
https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA

     
**/
#include <ESP8266WiFi.h> //Библиотека для работы с WIFI 
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <ArduinoOTA.h> // Библиотека для OTA-прошивки

const char* ssid = "Solar"; //Имя точки доступа WIFI 
const char* password = "123454321"; //пароль точки доступа WIFI 

void setup() {
//  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //************************
  ArduinoOTA.setHostname("ESP8266-00001"); // Задаем имя сетевого порта
  //ArduinoOTA.setPassword((const char *)"0000"); // Задаем пароль доступа для удаленной прошивки
  ArduinoOTA.begin(); // Инициализируем OTA
  //***********************
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }

   pinMode(2,OUTPUT);
  }

void loop() {
ArduinoOTA.handle(); // Всегда готовы к прошивке

digitalWrite(2,1);
delay(100);
digitalWrite(2,0);
delay(100); 
}
