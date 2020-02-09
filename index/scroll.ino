// ========================
//   ===== SETTINGS =====
//     ================

// 同名、同サイズのフォントをSDカード直下に入れてください
String fontName = "RictyDiminishedRegular40";
String fontExtension = ".vlw";
int fontSize = 40;

unsigned int colorBackground = TFT_ORANGE; //background color
unsigned int colorFill = TFT_WHITE;        //fill color
unsigned int colorStroke = TFT_BLACK;      //stroke color

int scrollInterval = 4; // スクロールの間隔
int scrollSpeed = 3;    //スクロールスピード

int spritePosY = 200; // sprite描画位置Y

//     ================
//   ===== SETTINGS =====
// ========================

boolean isScroll = false; // 文字列が長くてスクロールしなければならないかどうか

// M5.Lcdをラッパーしている
TFT_eSprite img = TFT_eSprite(&M5.Lcd);

String currentText;                  // 表示するテキスト
std::vector<int> textCharWidthArray; // textCharArray の各文字に対応する文字幅
std::vector<String> textCharArray;   // テキストを一文字ずつStringに入れる配列

int currentPos = 0;
int currentCharIndex = 0;

// String文字列を1文字ずつのStringにする関数
std::vector<String> textToCharArray(String str)
{
  // その文字が何バイト文字か判定して、そのバイト分のcharをStringに変換してvector<String>に格納する
  std::vector<String> result;
  char* s = const_cast<char *>(str.c_str());

  const char BIT_MASK_1 = 0b01111111; // or -> 01111111 => 1byte
  const char BIT_MASK_2 = 0b00011111; // or -> 11011111 => 2byte
  const char BIT_MASK_3 = 0b00001111; // or -> 11101111 => 3byte

  while (*s != '\0')
  {
    char oneChar[5];
    char a = *s;

    if ((a | BIT_MASK_1) == 0b01111111)
    {
      oneChar[0] = *(s++);
      oneChar[1] = '\0';
    }
    else if ((a | BIT_MASK_2) == 0b11011111)
    {
      oneChar[0] = *(s++);
      oneChar[1] = *(s++);
      oneChar[2] = '\0';
    }
    else if ((a | BIT_MASK_3) == 0b11101111)
    {
      oneChar[0] = *(s++);
      oneChar[1] = *(s++);
      oneChar[2] = *(s++);
      oneChar[3] = '\0';
    }
    else
    {
      oneChar[0] = *(s++);
      oneChar[1] = *(s++);
      oneChar[2] = *(s++);
      oneChar[3] = *(s++);
      oneChar[4] = '\0';
    }
    result.push_back(oneChar);
  }
  return result;
}

// setup()で呼び出す
void setupText()
{
  // 判定はSD.existsのみでもよい
  if (!SD.begin())
  {
    Serial.println("SDカードがない");
  }
  String fontFile = "/" + fontName + fontExtension;
  if (!SD.exists(fontFile))
  {
    Serial.println("FontFileがない");
  }
  img.loadFont(fontName, SD);
}

// 表示するテキストを変える度に呼び出す
void updateText(String text)
{
  img.deleteSprite();
  currentText = text;

  // テキストがディスプレイに収まるか判定
  if (img.textWidth(currentText) > M5.Lcd.width())
  {
    isScroll = true;

    textCharArray = textToCharArray(currentText);
    for (int i = 0; i < scrollInterval; i++)
    {
      textCharArray.push_back("  ");
    }
    textCharWidthArray.clear();
    for (int i = 0; i < textCharArray.size(); i++)
    {
      String str = textCharArray[i];
      Serial.println(str);
      textCharWidthArray.push_back(img.textWidth(str));
    }
    img.createSprite(M5.Lcd.width() + fontSize * 2, fontSize); //  fontSize * 2 は次の文字を描画するための遊びを設けるため
    img.fillSprite(colorBackground);
    img.setTextColor(colorFill, colorBackground);
    img.setScrollRect(0, 0, img.width(), img.height(), colorBackground);
    currentPos = 100; // スクロールの描画開始位置
    currentCharIndex = 0;
  }
  else
  {
    isScroll = false;

    img.createSprite(M5.Lcd.width(), 40);
    img.fillSprite(colorBackground);
    img.setTextColor(colorFill, colorBackground);
    img.setCursor(0, 0);
    img.setTextDatum(MC_DATUM);
    img.drawString(currentText, M5.Lcd.width() / 2, M5.Lcd.height() - 20);
    img.pushSprite(0, spritePosY);
  }
}

// loop()の度に呼び出す
void drawText()
{
  if (isScroll)
  {
    // SpriteのcontentsをscrollSpeed px分シフトする
    img.scroll(-scrollSpeed, 0);
    currentPos -= scrollSpeed;

    // Spriteに文字を書き込む
    while (currentPos < M5.Lcd.width() + textCharWidthArray[(currentCharIndex) % textCharWidthArray.size()])
    {
      img.setCursor(currentPos, 5);
      img.println(textCharArray[(currentCharIndex) % textCharWidthArray.size()]);
      currentPos += textCharWidthArray[(currentCharIndex) % textCharWidthArray.size()];
      currentCharIndex++;
    }
  }
  // Spriteを描画
  img.pushSprite(0, spritePosY);
}
