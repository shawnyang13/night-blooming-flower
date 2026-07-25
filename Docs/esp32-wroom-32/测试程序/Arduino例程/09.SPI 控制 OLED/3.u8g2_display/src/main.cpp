#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_2_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/18, /* data=*/13, /* cs=*/4, /* dc=*/2, /* reset=*/15);

void setup(void)
{
  // 初始化 OLED 控制对象
  u8g2.begin();
  // 开启中文字符集支持
  u8g2.enableUTF8Print();
  // 设置字体
  u8g2.setFont(u8g2_font_wqy12_t_gb2312b);
  // 清除缓存
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("Hello GeeksMan!");
  u8g2.setCursor(0, 40);
  u8g2.print("你好, 极客侠实验室");
  // 显示
  u8g2.sendBuffer();
}

void loop(void)
{
}