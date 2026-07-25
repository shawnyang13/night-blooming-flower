/*
  该程序的作用是实现 LED 移动效果。
  在线文档：https://docs.geeksman.com/esp32/Arduino/06.esp32-arduino-water-lamp.html
*/


// 定义 GPIO 引脚数组
int pin_list[5] = {13, 12, 14, 27, 26};
// 获取数组长度
int size = sizeof(pin_list) / sizeof(pin_list[0]);

void setup() {
  // 设定 GPIO 引脚为输出模式
  for (int i=0; i<size;i++) {
    pinMode(pin_list[i], OUTPUT);
    }
}

void loop() {
  // 将所有引脚设置为高电平
  for (int i=0;i<size;i++) {
    digitalWrite(pin_list[i], HIGH);
    if (i > 0){
      digitalWrite(pin_list[i-1], LOW);
      }else {
      digitalWrite(pin_list[size-1], LOW);
      }
    delay(250);
    
    }
}
