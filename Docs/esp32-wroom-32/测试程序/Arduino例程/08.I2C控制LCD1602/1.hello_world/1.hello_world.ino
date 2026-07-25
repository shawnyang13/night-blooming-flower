#include "LiquidCrystal_I2C.h"


// 构造 LCD 对象，设置地址，列数，行数
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // 初始化 LCD 对象
  lcd.init();

  // 打印内容
  lcd.backlight();
  lcd.print("Hello, world!");

}

void loop() {

}
