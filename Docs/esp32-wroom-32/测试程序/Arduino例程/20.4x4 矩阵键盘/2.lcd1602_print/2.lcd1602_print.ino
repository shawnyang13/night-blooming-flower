/*
  该程序作用是使用矩阵薄膜键盘与 LCD1602 液晶屏实现输入密码效果
  在线文档：https://docs.geeksman.com/esp32/Arduino/25.esp32-arduino-keyboard.html
*/
#include "LiquidCrystal_I2C.h"

// 定义行引脚数组
int row_pins[4] = {13, 12, 14, 27};
// 定义列引脚数组
int col_pins[4] = {26, 25, 33, 32};

// 创建按键变量
char key;

// 定义密码与输入密码变量
const char password[5] = "1234";
char entered_password[5] = "";

// 创建 LCD1602 对象
LiquidCrystal_I2C   lcd(0x27, 16, 2);


void setup() {

  // 行引脚设置为输入模式
  for (int i=0;i<4;i++) {
    pinMode(row_pins[i], INPUT_PULLUP);  
  }

  // 列引脚设置为输出模式
  for (int i=0;i<4;i++) {
    pinMode(col_pins[i], OUTPUT);  
    // 初始化为高电平
    digitalWrite(col_pins[i], HIGH);
  }

  // 初始化 lcd 对象
  lcd.init();
  // 开启背光
  lcd.backlight();

  // 设置光标
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}


// 获取按键值函数
char read_keypad(){
  // 定义键盘按键的布局
  char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'},
    };

  // 行列扫描法
  for (int j=0;j<4;j++) {
    // 将当前列设置为低电平
    digitalWrite(col_pins[j], LOW);

    for (int i=0;i<4;i++) {
      // 检测行输入引脚状态,检测到低电平，说明按键被按下，则返回按键值
      if (!digitalRead(row_pins[i])) {
          // 将当前列恢复为高电平
          digitalWrite(col_pins[j], HIGH);
          return keys[i][j];
      }
    }
    // 将当前列恢复为高电平
    digitalWrite(col_pins[j], HIGH);
  }

  return NULL;
}


// 校验显示密码
void check_password(char key) {
  if (key) {
    if (key == '#') {
      // 校验密码
      if (strcmp(entered_password, password) == 0) {
        lcd.clear();
        lcd.print("Access Granted");  
      } else {
        
        lcd.clear();
        lcd.print("Access Denied");  
      }

      delay(2000);
      lcd.clear();
      lcd.print("Enter Password:");

      // 清空已输入的密码
      memset(entered_password, 0, sizeof(entered_password));
    }else {
      // 输入密码，添加输入的字符到密码字符串中  
      if (strlen(entered_password) < 4) {
        lcd.setCursor(strlen(enter_password), 1);
        lcd.print("*");
        entered_password[strlen(entered_password)] = key;
      }
    }
  }

  delay(500);
}

void loop() {
  // 保存读取到的按键值
  key = read_keypad();

  check_password(key);
}
