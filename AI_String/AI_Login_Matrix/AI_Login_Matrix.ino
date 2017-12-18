#include "magFaceLogin.h"
#include <Microduino_ColorLED.h> //引用彩灯库
#include <Microduino_Matrix.h>

uint8_t Addr[MatrixPix_X][MatrixPix_Y] = {  //1x1
  {64}      //点阵IIC地址
};

Matrix display = Matrix(Addr, TYPE_COLOR); //TYPE_COLOR or TYPE_S2
ColorLED strip = ColorLED(1, A0);

#define SSID      "ylb"
#define PASSWORD  "ylb12345"
#define ID        "59dc2a581a12950001d7f46a"
#define TOKEN     "Z6UZhJVSHJoD"

#define TIMEOUT 30*1000
int32_t timerNew;

boolean dataSta = false;
String msg;
float scores;

#define string_target  F("result")

static const char updataDate[3][16] =
{
  "0123456789",
  "98.503601074219",
  "0123456789"
};

static const char string_head[3][14] = {
  "\"uid\":\"",
  "\"scores\":[",
  "\"user_info\":\""
};

static const char string_end[3][3] = {
  "\",",
  "],",
  "\"}"
};
static const char string_name[2][9] = {
  "Embedded",
  "YLB"
};

static const uint8_t logoA[] PROGMEM = {   //低位在前 逐行
  0x00, 0x00, 0x14, 0x2A, 0x2A, 0x2A, 0x00, 0x00
};

uint8_t ok[8][2] = {
  {0, 4},
  {1, 5},
  {2, 6},
  {3, 5},
  {4, 4},
  {5, 3},
  {6, 2},
  {7, 1}
};

uint8_t fail[12][2] = {
  {6, 1},
  {5, 2},
  {4, 3},
  {3, 4},
  {2, 5},
  {1, 6},
  {1, 1},
  {2, 2},
  {3, 3},
  {4, 4},
  {5, 5},
  {6, 6},
};

void loginOK()
{
  display.clearDisplay();
  for (uint8_t i = 0; i < 8; i++)
  {
    display.setLedColor(ok[i][0], ok[i][1], 0, 255, 0);
    delay(80);
  }
}

void loginFail()
{
  display.clearDisplay();
  for (uint8_t i = 0; i < 12; i++)
  {
    display.setLedColor(fail[i][0], fail[i][1], 255, 0, 0);
    delay(80);
  }
}

void setup() {

  Wire.begin();
  strip.begin();
  display.setBrightness(255);
#if DEBUG
  Serial.begin(115200);
#endif
  display.clearDisplay();
  strip.setPixelColor(0, COLOR_NONE);
  strip.show();
  delay(3000);
  boolean wifista;
  wifista = wifimCottonbegin(SSID, PASSWORD, ID, TOKEN);
  if (wifista) {
    wifi.mqttPublishM(Sendmsg("face", " "));
    wifi.mqttPublishM(Sendmsg("face", "设备连接成功"));
    strip.setPixelColor(0, COLOR_GREEN);
    strip.show();
    display.setColor(0, 255, 0);
    display.drawBMP(0, 0, 8, 8, logoA);  //x,y,w,h,data
    delay(2000);
    display.clearDisplay();
    strip.setPixelColor(0, COLOR_NONE);
    strip.show();
#if DEBUG
    Serial.print(F("Wifi is online\r\n"));
#endif
  }
}

void loop() {
  String mCottenData = wifi.getMqttJson();
  mCottenData.trim();
  if (mCottenData.equals("WIFI DISCONNECT")) {
    wifi.setWiFiconnected(false);
    wifi.setMqttConnected(false);
  } else if (mCottenData.equals("WIFI Connected")) {
    wifi.setWiFiconnected(true);
  } else if (mCottenData.equals("MQTT: Disconnected")) {
    wifi.setMqttConnected(false);
  } else if (mCottenData.equals("MQTT: Connected")) {
    wifimCottonagain(ID, TOKEN);
    wifi.mqttPublishM(Sendmsg("face", " "));
    wifi.mqttPublishM(Sendmsg("face", "设备连接成功"));
    wifi.setMqttConnected(true);
  }
  if (mCottenData.startsWith("{") && mCottenData.endsWith("}")) {
    timerNew = millis();
#if DEBUG
    Serial.println(mCottenData);
    freeRam();
#endif

    if (wifi.isMqttConnected())
    {
      if (available(mCottenData))
        dataSta = true;
      else
        dataSta = false;
      if (dataSta)
      {
        for (int a = 0; a < 3; a++) {
          findAndFilter(string_target, string_head[a], string_end[a], mCottenData, 24).toCharArray(updataDate[a], 20);
#if DEBUG
          Serial.print(F("updataDate["));
          Serial.print(a);
          Serial.print(F("]:"));
          Serial.print(updataDate[a]);
          Serial.print(F("\r\n"));
#endif
        }
        msg = String(updataDate[1]);
        scores = msg.toFloat();
#if DEBUG
        Serial.print("scores:");
        Serial.print(scores);
        Serial.print(F("\r\n"));
#endif
        if (strcmp(updataDate[0], string_name[0]) == 0 && strcmp(updataDate[2], string_name[1]) == 0 && scores > 90)
        {
          strip.setPixelColor(0, COLOR_GREEN);
          strip.show();
          wifi.mqttPublishM(Sendmsg("face", "您好立斌 欢迎回来"));
          loginOK();
          delay(3000);
          wifi.mqttPublishM(Sendmsg("face", " "));
#if DEBUG
          Serial.print(F("Face is OK  Welaome Libin!"));
          Serial.print(F("\r\n"));
#endif
        }
        else
        {
          strip.setPixelColor(0, COLOR_RED);
          strip.show();
          loginFail();
          wifi.mqttPublishM(Sendmsg("face", "请再刷一次"));
#if DEBUG
          Serial.print(F("NONE!"));
          Serial.print(F("\r\n"));
#endif
        }
      }
    }
  }

  if (timerNew > millis())timerNew = millis();
  if (millis() - timerNew > TIMEOUT) {
    wifi.mqttPublishM(Sendmsg("voice", " "));
    timerNew = millis();
  }
}
