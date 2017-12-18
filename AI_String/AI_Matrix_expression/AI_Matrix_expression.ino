#include "magExpression.h"
#include <Microduino_Matrix.h>

uint8_t Addr[MatrixPix_X][MatrixPix_Y] = {  //1x1
  {64}      //点阵IIC地址
};

Matrix display = Matrix(Addr, TYPE_COLOR); //TYPE_COLOR or TYPE_S2

static const uint8_t logoA[] PROGMEM = {   //低位在前 逐行
  0x00, 0x00, 0x14, 0x2A, 0x2A, 0x2A, 0x00, 0x00
};
static const uint8_t happiness[] PROGMEM = {   //低位在前 逐行
  0x00, 0x44, 0xAA, 0x00, 0x00, 0x44, 0x38, 0x00
};
static const uint8_t neutral[] PROGMEM = {   //低位在前 逐行
  0x00, 0x44, 0xAA, 0x00, 0x00, 0x00, 0x38, 0x00
};
static const uint8_t sadness[] PROGMEM = {   //低位在前 逐行
  0x00, 0x44, 0xAA, 0x00, 0x00, 0x38, 0x44, 0x00
};
static const uint8_t surprise[] PROGMEM = {   //低位在前 逐行
  0x00, 0x44, 0xAA, 0x44, 0x00, 0x00, 0x38, 0x00
};

boolean dataSta = false;
uint8_t small;

#define SSID      "ylb"
#define PASSWORD  "ylb12345"
#define ID        "59e041fd4f9dd3000141b127"
#define TOKEN     "mQ8sFJn4fXLL"

#define MSGNUM 8

#define string_target F("\"scores\":")
#define string_body F(",")

#define TIMEOUT 30*1000
int32_t timer;

static const char string_head[MSGNUM][10] = {
  "anger",     //愤怒
  "contempt",  //鄙视
  "disgust",   //厌恶
  "fear",      //恐惧
  "happiness", //快乐
  "neutral",   //中性
  "sadness",   //悲伤
  "surprise"   //惊讶
};

float Data[MSGNUM];

uint8_t expressionshow(float a, float b, float c, float d)
{
  float max;
  max = a > b ? a : b;
  max = max > c ? max : c;
  max = max > d ? max : d;
  if (max == 0)
    return 0;
  else if (max == a)
    return 1;
  else if (max == b)
    return 2;
  else if (max == c)
    return 3;
  else if (max == d)
    return 4;
}

void setup() {
  Wire.begin();
#if DEBUG
  Serial.begin(115200);
#endif
  display.clearDisplay();
  delay(3000);
  boolean wifista;
  wifista = wifimCottonbegin(SSID, PASSWORD, ID, TOKEN);
  if (wifista) {
#if DEBUG
    wifi.mqttPublishM(Sendmsg("voice", " "));
    wifi.mqttPublishM(Sendmsg("voice", "设备连接成功"));
    Serial.print(F("Wifi is online\r\n"));
    display.clearDisplay();
    display.setColor(0, 255, 0);
    display.drawBMP(0, 0, 8, 8, logoA);  //x,y,w,h,data
    delay(2000);
    display.clearDisplay();
#endif
  }
}

void loop() {
  String mCottenData = wifi.getMqttJson();

  if (mCottenData != "") {
    timer = millis();
    mCottenData.trim();
    mCottenData.replace("}}]}", ",}}]}");
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

    if (wifi.isMqttConnected())
    {
      if (available(mCottenData))
        dataSta = true;
      else
        dataSta = false;

      if (dataSta)
      {
        for (int a = 0; a < MSGNUM; a++) {
          Data[a] = findAndFilter(string_target, "\"" + String(string_head[a]) + "\":", string_body, mCottenData, 32).toFloat();
#if DEBUG
          Serial.print(F("["));
          Serial.print(string_head[a]);
          Serial.print(F(":"));
          Serial.print(Data[a], 12);
          Serial.print(F("] "));
          Serial.print(F("\r\n"));
#endif
        }
        small = expressionshow(Data[4], Data[5], Data[6], Data[7]);
#if DEBUG
        Serial.print(small);
        Serial.print(F("\r\n"));
#endif
        switch (small)
        {
          case 1:
            wifi.mqttPublishM(Sendmsg("voice", "开心"));
            display.clearDisplay();
            display.setColor(0, 255, 0);
            display.drawBMP(0, 0, 8, 8, happiness);
            break;
          case 2:
            wifi.mqttPublishM(Sendmsg("voice", "平常"));
            display.clearDisplay();
            display.setColor(255, 255, 0);
            display.drawBMP(0, 0, 8, 8, neutral);
            break;
          case 3:
            wifi.mqttPublishM(Sendmsg("voice", "伤心"));
            display.clearDisplay();
            display.setColor(255, 0, 0);
            display.drawBMP(0, 0, 8, 8, sadness);
            break;
          case 4:
            wifi.mqttPublishM(Sendmsg("voice", "惊讶"));
            display.clearDisplay();
            display.setColor(255, 0, 255);
            display.drawBMP(0, 0, 8, 8, surprise);
            break;
        }
        mCottenData = "";
      }
    }
  }
  
  if (timer > millis())timer = millis();
  if (millis() - timer > TIMEOUT) {
    wifi.mqttPublishM(Sendmsg("voice", " "));
    timer = millis();
  }
}
