/*
  该程序的作用是循环遍历所有 LED，检测一下是否所有的 LED 都可以正常工作
  在线文档：https://docs.geeksman.com/esp32/Arduino/22.esp32-arduino-led-matrix.html
*/

// 定义行引脚数组
int row_array[8] = {13, 25, 2, 27, 23, 4, 22, 18};
// 定义列引脚数组
int col_array[8] = {26, 21, 19, 12, 5, 14, 33, 32};


void setup() {
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
  // 遍历所有的 LED
  for (int i=0;i<8;i++) {
    digitalWrite(row_array[i], LOW);
    for (int j=0;j<8;j++) {
      digitalWrite(col_array[j], HIGH);
      delay(100);
      digitalWrite(col_array[j], LOW);
      }
    digitalWrite(row_array[i], HIGH);
  }
}
