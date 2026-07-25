/*
  该程序的作用是实现 PS2 摇杆控制 LED 移动
  在线文档：https://docs.geeksman.com/esp32/Arduino/22.esp32-arduino-led-matrix.html
*/
#define PS2_X   15
#define PS2_Y   35

// 定义行引脚数组
int row_array[8] = {13, 25, 2, 27, 23, 4, 22, 18};
// 定义列引脚数组
int col_array[8] = {26, 21, 19, 12, 5, 14, 33, 32};

// 初始化 LED 开始位置
int led_pos[2] = {1, 2};

// 初始化摇杆信号变量
int x_value;
int y_value;


void setup() {
  // 配置 PS2 引脚模式
  pinMode(PS2_X, OUTPUT);
  pinMode(PS2_Y, OUTPUT);
  
  // 配置行引脚，初始化为高电平
  for (int i=0;i<8;i++){
    pinMode(row_array[i], OUTPUT);
    digitalWrite(row_array[i], HIGH);  
  }
  
  // 配置列引脚，初始化为低电平
  for (int i=0;i<8;i++) {
    pinMode(col_array[i], OUTPUT);
    digitalWrite(col_array[i], LOW);  
  }
}


void loop() {
  // 读取遥感信号
  x_value = analogRead(PS2_X);
  y_value = analogRead(PS2_Y);

  // 清除之前位置的 LED
  digitalWrite(row_array[led_pos[0]], HIGH);
  digitalWrite(col_array[led_pos[1]], LOW);
  
  // 检测 x 轴是否移动
  if (x_value > 4095 / 2 + 300 && led_pos[0] < 7){
    led_pos[0] += 1;
  }else if (x_value < 4095 / 2 - 300 && led_pos[0] > 0) {
    led_pos[0] -= 1;
  }

  // 检测 y 轴是否移动
  if (y_value > 4095 / 2 + 300 && led_pos[1] > 0){
    led_pos[1] -= 1;
  }else if (y_value < 4095 / 2 - 300 && led_pos[1] < 7) {
    led_pos[1] += 1;
  }

  // 显示 LED 的位置
  digitalWrite(row_array[led_pos[0]], LOW);
  digitalWrite(col_array[led_pos[1]], HIGH);

  delay(50);
  
}
