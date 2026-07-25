#include "LiquidCrystal_I2C.h"


// 构造 LCD 对象
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // 初始化 lcd 对象
  lcd.init();
  // 开启背光
  lcd.backlight();
  // 设置串口波特率
  Serial.begin(9600);
}

void loop() {
  // 检测是否有串口输入
  if (Serial.available()){
    // 延时 100ms，等待所有的数据传输完成
    delay(100);
    // 清屏
    lcd.clear();
    // 反复读取串口缓冲区的数据，并显示在屏幕上
    while(Serial.available()) {
      lcd.write(Serial.read());
      }
    
    }

}
