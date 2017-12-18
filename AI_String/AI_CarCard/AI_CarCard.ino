#include <Microduino_ColorLED.h> //引用彩灯库
#include "msgCarCard.h"

ColorLED strip = ColorLED(1, 6);

#define SSID      "ylb"
#define PASSWORD  "ylb12345"
#define ID        "59dc31e71a12950001d7f49f"
#define TOKEN     "4tyJCvn44lst"

#define TIMEOUT 30*1000
int32_t timerNew;

String mCottenData;
boolean dataSta = false;

boolean opendoor;
long timer;

#define string_target  F("result")
#define string_data    F("\",\"number\":\"")
char updataDate[] = "123456";

uint8_t num;

static const char card[2][12] = {
  "京A88888",
  "京A00000"
};

void setup() {
#if DEBUG
  Serial.begin(115200);
  strip.begin();
  delay(100);
#endif
  boolean wifista;
  wifista = wifimCottonbegin(SSID, PASSWORD, ID, TOKEN);
  if (wifista) {
    wifi.mqttPublishM(Sendmsg("card", ""));
    wifi.mqttPublishM(Sendmsg("card", "设备连接成功"));
    strip.setPixelColor(0, 0X00ff00);
    strip.show();
    delay(1000);
    strip.setPixelColor(0, 0X00000);
    strip.show();
#if DEBUG
    Serial.print(F("Wifi is online\r\n"));
#endif
  }
}

void loop() {
  mCottenData = wifi.getMqttJson();
  if (mCottenData != "") {
    timerNew = millis();
    mCottenData.trim();
#if DEBUG
    Serial.println(mCottenData);
#endif

    if (mCottenData.equals("WIFI DISCONNECT")) {
      wifi.setWiFiconnected(false);
      wifi.setMqttConnected(false);
    } else if (mCottenData.equals("WIFI Connected")) {
      wifi.setWiFiconnected(true);
    } else if (mCottenData.equals("MQTT: Disconnected")) {
      wifi.setMqttConnected(false);
    } else if (mCottenData.equals("MQTT: Connected")) {
      wifimCottonagain(ID, TOKEN);
      wifi.mqttPublishM(Sendmsg("card", " "));
      wifi.mqttPublishM(Sendmsg("card", "设备连接成功"));
      wifi.setMqttConnected(true);
    }

    if (wifi.isMqttConnected())
    {
      if (available(mCottenData))
        dataSta = true;
      else
        dataSta = false;

      if (dataSta)
      {
        findAndFilter(string_target, string_data, "\"}", mCottenData, 24).toCharArray(updataDate, 20);
#if DEBUG
        Serial.print(F("updataDate:"));
        Serial.print(updataDate);
        Serial.print(F("\r\n"));
#endif
        if (strcmp(updataDate, card[0]) == 0)
        {
          wifi.mqttPublishM(Sendmsg("card", card[0]));
          opendoor = true;
          timer = millis();
          strip.setPixelColor(0, 0X00ff00);
          strip.show();
#if DEBUG
          Serial.print(F("Card is OK Welaome!"));
          Serial.print(F("\r\n"));
#endif
        }
        else if (strcmp(updataDate, card[1]) == 0)
        {
          wifi.mqttPublishM(Sendmsg("card", card[1]));
          opendoor = true;
          timer = millis();
          strip.setPixelColor(0, 0Xff00ff);
          strip.show();
#if DEBUG
          Serial.print(F("Card is OK Welaome!"));
          Serial.print(F("\r\n"));
#endif
        }
        else
        {
#if DEBUG
          Serial.print(F("NONE!"));
          Serial.print(F("\r\n"));
#endif
        }
      }
    }
  }
  if (millis() - timer > 4000 && opendoor)
  {
    strip.setPixelColor(0, 0X00000);
    strip.show();
    opendoor = false;
  }

  if (timerNew > millis())timerNew = millis();
  if (millis() - timerNew > TIMEOUT) {
    wifi.mqttPublishM(Sendmsg("card", " "));
    timerNew = millis();
  }
}
