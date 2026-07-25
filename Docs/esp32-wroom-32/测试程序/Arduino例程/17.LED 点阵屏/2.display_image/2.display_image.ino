/*
  该程序的作用是在点阵屏上显示图案
  在线文档：https://docs.geeksman.com/esp32/Arduino/22.esp32-arduino-led-matrix.html
*/

// 定义图案逻辑数组
int hex_array[8] = {0x00,0x66,0xFF,0xFF,0xFF,0x7E,0x3C,0x18};

// 定义行引脚数组
int row_array[8] = {13, 25, 2, 27, 23, 4, 22, 18};
// 定义列引脚数组
int col_array[8] = {26, 21, 19, 12, 5, 14, 33, 32};


void setup() {
  // 设置串口波特率
  Serial.begin(9600);
  
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
  for (int i=0;i<8;i++) {
    for (int j=0;j<8;j++) {
//      Serial.print(bitRead(hex_array[i], j));
      digitalWrite(col_array[j], bitRead(hex_array[i], j));
    }
    digitalWrite(row_array[i], LOW);
    delay(1);
    digitalWrite(row_array[i], HIGH);
//    Serial.println();
//    delay(1000);
  }
//  Serial.println();  
}
