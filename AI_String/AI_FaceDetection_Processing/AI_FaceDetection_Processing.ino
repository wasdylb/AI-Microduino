#include "msgComputer.h"

#define SSID      "ylb"
#define PASSWORD  "ylb12345"
#define ID        "59dda5d31a12950001d7fa4f"
#define TOKEN     "XWLAzNiaRQjx"

#define TIMEOUT 30*1000
int32_t timerNew;

boolean dataSta = false;

#define string_target  F("\"roll\":")
#define string_targetF  F("\"glasses_probability\":")
#define string_targetG  F("\"faceshape\":")

static const char string_head[5][8] = {
  "age",
  "beauty",
  "glasses",
  "race",
  "gender"
};

int Data[3];
char updataDate[] = "------";
char genderDate[] = "------";

long timer;
boolean wifista;

void setup() {
  Serial.begin(57600);
  Serial.println(Sendprocessing(wifista, updataDate, Data[0], Data[1], Data[2], genderDate));
  wifista = wifimCottonbegin(SSID, PASSWORD, ID, TOKEN);
  if (wifista) {
    wifi.mqttPublishM(Sendmsg("voice", " "));
    wifi.mqttPublishM(Sendmsg("voice", "设备连接成功"));
#if DEBUG
    Serial.print(F("Wifi is online\r\n"));
#endif
  }
  Serial.println(Sendprocessing(wifista, updataDate, Data[0], Data[1], Data[2], genderDate));
}
void loop() {
  String mCottenData = wifi.getMqttJson();
  mCottenData.trim();
  if (mCottenData != "") {
    timerNew = millis();
#if DEBUG
    Serial.println(mCottenData);
#endif
    if (mCottenData.equals("WIFI DISCONNECT")) {
      wifi.setWiFiconnected(false);
      wifi.setMqttConnected(false);
      wifista = false;
      Serial.println(Sendprocessing(wifista, "", 0, 0, 0, ""));
    } else if (mCottenData.equals("WIFI Connected")) {
      wifi.setWiFiconnected(true);
    } else if (mCottenData.equals("MQTT: Disconnected")) {
      wifi.setMqttConnected(false);
    } else if (mCottenData.equals("MQTT: Connected")) {
      wifi.setMqttConnected(true);
      wifimCottonagain(ID, TOKEN);
      wifi.mqttPublishM(Sendmsg("voice", " "));
      wifi.mqttPublishM(Sendmsg("voice", "设备连接成功"));
      wifista = true;
      Serial.println(Sendprocessing(wifista, "", 0, 0, 0, ""));
    }

    //freeRam();
    if (available(mCottenData))
      dataSta = true;
    else
      dataSta = false;

    if (dataSta)
    {
      for (uint8_t i = 0; i < 3; i++) {
        Data[i] = findAndFilter(string_target, "\"" + String(string_head[i]) + "\":", ",", mCottenData, 24).toInt();
#if DEBUG
        Serial.print(F("["));
        Serial.print(string_head[i]);
        Serial.print(F(":"));
        Serial.print(Data[i]);
        Serial.print(F("] "));
        Serial.print(F("\r\n"));
#endif
      }
      findAndFilter(string_targetF, "\"" + String(string_head[3]) + "\":\"", "\",", mCottenData, 24).toCharArray(updataDate, 10);
      findAndFilter(string_targetG, "\"" + String(string_head[4]) + "\":\"", "\",", mCottenData, 24).toCharArray(genderDate, 10);
#if DEBUG
      Serial.print(F("[race:"));
      Serial.print(updataDate);
      Serial.print(F("]\r\n"));
      Serial.print(F("[gender:"));
      Serial.print(genderDate);
      Serial.print(F("]\r\n"));
#endif
      if (Data[0] > 0 && Data[1] > 0) {
        Serial.println(Sendprocessing(wifista, updataDate, Data[0], Data[1], Data[2], genderDate));
        if (Data[2]) {
          if (strcmp(genderDate, "male") == 0)
            wifi.mqttPublishM(Sendmsg("voice", "年龄" + String(Data[0]) + "颜值" + String(Data[1]) + "%性别男肤色" + updataDate + "戴了眼镜"));
          else if (strcmp(genderDate, "female") == 0)
            wifi.mqttPublishM(Sendmsg("voice", "年龄" + String(Data[0]) + "颜值" + String(Data[1]) + "%性别女肤色" + updataDate + "戴了眼镜"));
        }
        else {
          if (strcmp(genderDate, "male") == 0)
            wifi.mqttPublishM(Sendmsg("voice", "年龄" + String(Data[0]) + "颜值" + String(Data[1]) + "%性别男肤色" + updataDate + "没戴眼镜"));
          else if (strcmp(genderDate, "female") == 0)
            wifi.mqttPublishM(Sendmsg("voice", "年龄" + String(Data[0]) + "颜值" + String(Data[1]) + "%性别女肤色" + updataDate + "没戴眼镜"));
        }
        Data[0] = 0;
        Data[1] = 0;
        mCottenData = "";
      }
    }
  }
  
  if (timerNew > millis())timerNew = millis();
  if (millis() - timerNew > TIMEOUT) {
    wifi.mqttPublishM(Sendmsg("voice", " "));
    timerNew = millis();
  }
}
