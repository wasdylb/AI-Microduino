/*
  LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)
  版权所有: @老潘orz  wasdpkj@hotmail.com
  本例程展示了一种占用内存资源极低的字符串处理办法
*/

#include <ESP8266.h>
#define DEBUG  0

/**
**CoreUSB UART Port: [Serial1] [D0,D1]
**Core+ UART Port: [Serial1] [D2,D3]
**/
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1284P__) || defined (__AVR_ATmega644P__) || defined(__AVR_ATmega128RFA1__)
#define EspSerial Serial1
#define UARTSPEED  115200
#endif

/**
**Core UART Port: [SoftSerial] [D2,D3]
**/
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); /* RX:D2, TX:D3 */

#define EspSerial mySerial
#define UARTSPEED  9600
#endif

ESP8266 wifi(&EspSerial);

bool wifimCottonbegin(const char* ssid, const char* pass, const char* id, const char* token)
{
  WifiInit(EspSerial, UARTSPEED);
  //Serial.print(F("Start Connection mCotton...Waiting for 5S...\r\n"));
  //--------------------1------------------//
  while (!wifi.setOprToStation());
#if DEBUG
  Serial.print(F("to station ok\r\n"));
#endif
  //--------------------2------------------//
  while (!wifi.joinAP(ssid, pass));
  wifi.setWiFiconnected(true);
#if DEBUG
  Serial.print(F("Join AP success\r\n"));
#endif
  //--------------------3------------------//
  while (!wifi.mqttSetServer("mCotton.microduino.cn", (1883)));
#if DEBUG
  Serial.print(F("mqtt set server ok\r\n"));
#endif
  //--------------------4------------------//
  while (!wifi.mqttConnect(id, id, token));
  wifi.setMqttConnected(true);
#if DEBUG
  Serial.print(F("mqtt connect ok\r\n"));
#endif
  //--------------------5------------------//
  while (!wifi.mqttSetDiveceIDToken(id, token));
#if DEBUG
  Serial.print(F("mqtt set device ID Token ok\r\n"));
#endif
  //--------------------6------------------//
  char SUBSCRIBTOPICA[30] = "";
  strcat(SUBSCRIBTOPICA, "ca/");
  strcat(SUBSCRIBTOPICA, id);
  while (!wifi.mqttSetSubscrib(SUBSCRIBTOPICA));
#if DEBUG
  Serial.print(F("mqtt set subscrib ca topic ok\r\n"));
#endif
  //--------------------7------------------//
  char SUBSCRIBTOPICP[30] = "";
  strcat(SUBSCRIBTOPICP, "cp/");
  strcat(SUBSCRIBTOPICP, id);
  while (!wifi.mqttSetSubscrib(SUBSCRIBTOPICP));
#if DEBUG
  Serial.print(F("mqtt set subscrib cp topic ok\r\n"));
#endif

  return true;
}

bool wifimCottonagain(const char* id, const char* token)
{
  while (!wifi.mqttSetDiveceIDToken(id, token));
#if DEBUG
  Serial.print(F("mqtt set device ID Token ok\r\n"));
#endif
  //--------------------6------------------//
  char SUBSCRIBTOPICA[30] = "";
  strcat(SUBSCRIBTOPICA, "ca/");
  strcat(SUBSCRIBTOPICA, id);
  while (!wifi.mqttSetSubscrib(SUBSCRIBTOPICA));
#if DEBUG
  Serial.print(F("mqtt set subscrib ca topic ok\r\n"));
#endif
  //--------------------7------------------//
  char SUBSCRIBTOPICP[30] = "";
  strcat(SUBSCRIBTOPICP, "cp/");
  strcat(SUBSCRIBTOPICP, id);
  while (!wifi.mqttSetSubscrib(SUBSCRIBTOPICP));
#if DEBUG
  Serial.print(F("mqtt set subscrib cp topic ok\r\n"));
#endif

  return true;
}

String uploadData(char* _st, int _data)
{
  String send_data = "";
  send_data = "{\"";
  send_data += _st;
  send_data += "\":\"";
  send_data += _data;
  send_data += "\"}";
  return send_data;
}

String uploadData(char* _st, char* _data)
{
  String send_data = "";
  send_data = "{\"";
  send_data += _st;
  send_data += "\":\"";
  send_data += _data;
  send_data += "\"}";
  return send_data;
}

void freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  Serial.print(F("freeRAM:"));
  Serial.print( (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval));
  Serial.print(F("\r\n"));
}

bool available(String _msg) {
  uint8_t numberf, numberb;
  for (uint16_t i = 0; i < _msg.length(); i++)
  {
    if (_msg[i] == '{') {
      numberf++;
    }
    else if (_msg[i] == '}') {
      numberb++;
    }
  }
  if (numberf > 0 && numberb > 0 && (numberf == numberb))
    return true;
  else
    return false;
}

String findAndFilter(String target, String begin, String end, String data, uint8_t bufferNum = 32) {
  if (data.indexOf(target) != -1) {
    int16_t index1 = data.indexOf(begin);
    if (index1 == -1) {
      return "NULL";
    }
    index1 += begin.length();
    data = data.substring(index1, index1 + bufferNum);

    int16_t index2 = data.indexOf(end);
    if (index2 != -1) {
      return data.substring(0, index2);
    }
  }
  return "NULL";
}

String Sendmsg(String _st, String _data)
{
  String send_data;
  send_data = "{\"";
  send_data += _st;
  send_data += "\":\"";
  send_data += _data;
  send_data += "\"\}";
  return send_data;
}

String Sendprocessing(boolean _wifi, String _race, int _age, int _beauty, int _glasses, String _gender)
{
  String processing = "{\"wifi\":";
  processing += _wifi;
  processing += ",\"age\":";
  processing += _age;
  processing += ",\"beauty\":";
  processing += _beauty;
  processing += ",\"race\":\"";
  processing += _race;
  processing += "\",\"glasses\":";
  processing += _glasses;
  processing += ",\"gender\":\"";
  processing += _gender;
  processing += "\"}";
  return processing;
}

