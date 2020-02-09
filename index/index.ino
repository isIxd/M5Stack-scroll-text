#include <M5Stack.h>
#include <vector>
#include <stdio.h>

String str[] = {"マルチバイト文字", "表示OK", "長い文字はスクロールするよ", "短い文字は", "スクロール", "しないよ", "M5Stackでマルチバイト文字をスクロール(^-^)"};

void setup()
{
  M5.begin();
  M5.Lcd.setBrightness(200);
  M5.Lcd.clear();
  Serial.begin(115200);
  setupText();
}

int btnCnt = 0;

void loop()
{
  drawText();
  delay(30);
  M5.update();
  if (M5.BtnA.wasReleased())
  {
    Serial.println("Press");
    int idx = btnCnt % (sizeof(str) / sizeof(*str));
    updateText(str[idx]);
    Serial.println(idx);
    btnCnt++;
  }
}
