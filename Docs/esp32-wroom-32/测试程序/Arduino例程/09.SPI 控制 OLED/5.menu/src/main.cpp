#include <Arduino.h>
#include <U8g2lib.h>


void display_menu(unsigned int index);

U8G2_SSD1306_128X64_NONAME_2_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/18, /* data=*/13,
                                            /* cs=*/4, /* dc=*/2, /* reset=*/15);

#define MENU_SIZE 4
char* menu[MENU_SIZE] = {"Item 1", "Item 2", "Item 3", "Item 4"};

#define BUTTON_UP 12
#define BUTTON_DOWN 14

// 定义当前选项
unsigned int  order = 0;

void setup()
{
  // 初始化 OLED 对象
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x12_tr);
  
  // 配置输入按键
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
}

void loop()
{
  // 判断按键是否按下，并记录当前箭头位置
  if(!digitalRead(BUTTON_UP)) 
  {
    order = (order - 1) % 4;
  }else if (!digitalRead(BUTTON_DOWN))
  {
    order = (order + 1) % 4;
  }
  
  // 显示菜单
  display_menu(order);

  // 延时
  delay(100);
}

void display_menu(unsigned int index)
{
  // 进入第一页
  u8g2.firstPage();
  do
  {
    // 绘制页面内容
    u8g2.drawStr(0, 12, "Menu");
    u8g2.drawHLine(0, 14, 128);
    for (int i = 0; i < MENU_SIZE; i++)
    {
      if (i == index)
      {
        u8g2.drawStr(5, (i + 2) * 12 + 2, ">");
        u8g2.drawStr(20, (i + 2) * 12 + 2, menu[i]);
      }
      else
      {
        u8g2.drawStr(5, (i + 2) * 12 + 2, menu[i]);
      }
    }
  } while (u8g2.nextPage()); // 进入下一页，如果还有下一页则返回 True.
}