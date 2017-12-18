#include <Microduino_ColorLED.h>

ColorLED strip = ColorLED(6, A0);

String uartMsg;
uint8_t uartStep = 0;

String msg;
int mode;

String solution(String _sta, char *c)
{
  String data;
  if (_sta.startsWith("{") && _sta.endsWith("}"))
  {
    _sta = _sta.substring(1, _sta.length() - 1);
    _sta.replace("\"", "");
    uint8_t _length = _sta.length();
    char buf[_length];
    char c_all[30] = "";
    char data1[] = ":%s";
    strcat(c_all, c);
    strcat(c_all, data1);
    sscanf(_sta.c_str(), c_all, &buf);
    data = String(buf);
  }
  if (data != NULL)
    return data;
}

void BLESenddata(String _st, String _data)
{
  String send_data;
  int8_t number;
  send_data = "{\"";
  send_data += _st;
  send_data += "\":\"";
  send_data += _data;
  send_data += "\"}";
  number = send_data.length() / 17;

  if (number == 0)
  {
    Serial.println(send_data);
    delay(30);
  }
  else
  {
    while (number >= 0)
    {
      Serial.print(send_data.substring(0, 17));
      send_data = send_data.substring(17, send_data.length());
      delay(30);
      number--;
    }
    Serial.print("\n");
  }
}

String readserail()  {

  char inByte = Serial.read();
  switch (uartStep)
  {
    case 0:
      uartMsg = "";
      if (inByte == '{') {
        uartMsg += inByte;
        uartStep = 1;
      }
      break;
    case 1:
      uartMsg += inByte;
      if (inByte == '}') {
        uartStep = 0;
        return uartMsg;
      }
      break;
    default:
      break;
  }
  return "";
}


void setup() {
  strip.begin();
  strip.setBrightness(60);       //设置彩灯亮度
  Serial.begin(9600);
}

void loop() {
  String  BLE_Receive = readserail();
  if (BLE_Receive != "") {
    msg = solution(BLE_Receive, "show_S1");
    mode = msg.toInt();
    if ((mode) == 0) {
      BLESenddata("control_C1", String("OFF"));
      strip.setAllLED(COLOR_NONE);
      strip.show();
    }
    if ((mode) == 1) {
      BLESenddata("control_C1", String("RED"));
      strip.setAllLED(COLOR_RED);
      strip.show();
    }
    if ((mode) == 2) {
      BLESenddata("control_C1", String("GREEN"));
      strip.setAllLED(COLOR_GREEN);
      strip.show();
    }
    if ((mode) == 3) {
      BLESenddata("control_C1", String("BLUE"));
      strip.setAllLED(COLOR_BLUE);;
      strip.show();
    }
  }
  delay(10);
}

