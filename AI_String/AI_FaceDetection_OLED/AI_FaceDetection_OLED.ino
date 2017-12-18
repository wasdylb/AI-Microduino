#include <U8glib.h>
#include "msgComputer.h"

#define SSID      "ylb"
#define PASSWORD  "ylb12345"
#define ID        "59dda5d31a12950001d7fa4f"
#define TOKEN     "XWLAzNiaRQjx"

#define TIMEOUT 30*1000
int32_t timerMsg;

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

//-------字体设置，大、中、小
#define setFont_L u8g_font_timB14
#define setFont_M u8g_font_9x15
#define setFont_S u8g_font_fixed_v0r

boolean dataSta = false;

#define string_target  F("\"roll\":")
#define string_targetF  F("\"glasses_probability\":")

static const char string_head[4][8] = {
  "age",
  "beauty",
  "glasses",
  "race"
};

int Data[3];
char updataDate[] = "      ";

long timer;

void draw() {
  for (uint8_t i = 0; i < 4; i++) {
    u8g.setFont(setFont_S);
    u8g.setPrintPos(0, 15 + i * 15);
    u8g.print(string_head[i]);
    u8g.print(":");
    u8g.setFont(setFont_L);
    if (i < 3) {
      u8g.print(Data[i]);
      if (i == 1)
        u8g.print("%");
    }
    else {
      u8g.print(updataDate);
    }
  }
}

void setup() {
  Serial.begin(115200);
  //u8g.setRot90();
  boolean wifista;
  wifista = wifimCottonbegin(SSID, PASSWORD, ID, TOKEN);
  if (wifista) {
    wifi.mqttPublishM(Sendmsg("voice", ""));
    wifi.mqttPublishM(Sendmsg("voice", "设备连接成功"));
#if DEBUG
    Serial.print(F("Wifi is online\r\n"));
#endif
  }
}
void loop() {
  String mCottenData = wifi.getMqttJson();
  mCottenData.trim();
  if (mCottenData != "") {
    timerMsg = millis();
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
      wifi.mqttPublishM(Sendmsg("voice", " "));
      wifi.mqttPublishM(Sendmsg("voice", "设备连接成功"));
      wifi.setMqttConnected(true);
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
#if DEBUG
      Serial.print(F("[race:"));
      Serial.print(updataDate);
      Serial.print(F("]\r\n"));
#endif
      if (Data[0] > 0 && Data[1] > 0) {
        if (Data[2])
          wifi.mqttPublishM(Sendmsg("voice", "年龄" + String(Data[0]) + "颜值" + String(Data[1]) + "%肤色" + String(updataDate) + "戴了眼镜"));
        else
          wifi.mqttPublishM(Sendmsg("voice", "年龄" + String(Data[0]) + "颜值" + String(Data[1]) + "%肤色" + String(updataDate) + "没戴眼镜"));
        mCottenData = "";
      }
    }
  }
  if (millis() - timer > 2000)
  {
    u8g.firstPage();
    do {
      draw();
    } while ( u8g.nextPage());
    timer = millis();
  }

  if (timerMsg > millis())timerMsg = millis();
  if (millis() - timerMsg > TIMEOUT) {
    wifi.mqttPublishM(Sendmsg("voice", " "));
    timerMsg = millis();
  }
}
