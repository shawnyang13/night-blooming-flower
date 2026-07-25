/*
  该程序的作用是点亮一颗 LED
  在线文档：https://docs.geeksman.com/esp32/Arduino/05.esp32-arduino-LED.html
*/


// 设置 LED 引脚
int led_pin = 12;

void setup() {
  // 设定引脚为输出模式
  pinMode(led_pin, OUTPUT);
}

void loop() {
  // 点亮 LED
  digitalWrite(led_pin, HIGH);
}
