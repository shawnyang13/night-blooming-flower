#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED 显示屏宽度
#define SCREEN_HEIGHT 64 // OLED 显示屏高度

// 软件SPI总线
#define OLED_MOSI 13
#define OLED_CLK 18
#define OLED_DC 2
#define OLED_CS 4
#define OLED_RESET 15
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT,
                      OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// 初始化进度条变量
int progress = 0;

void setup()
{
  oled.begin();
  oled.setTextSize(2);              // 设置字体大小
  oled.setTextColor(SSD1306_WHITE); // 设置文本颜色
  oled.display();                   // 显示内容
}

void loop()
{
  // 清空屏幕
  oled.clearDisplay();

  // 设置光标位置
  oled.setCursor(25, 40);
  // 显示文字
  oled.println("Process");

  // 显示进度条边框
  oled.drawRoundRect(0, 10, 128, 20, 5, SSD1306_WHITE);
  // 显示进度
  oled.fillRoundRect(5, 15, progress, 10, 2, SSD1306_WHITE);

  // 进度递增
  if (progress < 118)
  {
    progress++;
  }
  else
  {
    progress = 0;
  }

  // 刷新屏幕
  oled.display();

  delay(50); // 延迟一段时间后更新显示
}