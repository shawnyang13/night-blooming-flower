#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_2_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/18, /* data=*/13,
                                             /* cs=*/4, /* dc=*/2, /* reset=*/15);

int progress = 0;

void setup()
{
  // 初始化 OLED 对象
  u8g2.begin();
}

void loop()
{
  // 进入第一页
  u8g2.firstPage();
  do
  {
    // 显示进度条边框
    u8g2.drawFrame(0, 10, 128, 20);
    // 显示进度
    u8g2.drawBox(5, 15, progress, 10);

  } while (u8g2.nextPage());  // 进入下一页，如果还有下一页则返回true

  // 进度递增
  if (progress < 118)
  {
    progress++;
  }
  else
  {
    progress = 0;
  }
}
