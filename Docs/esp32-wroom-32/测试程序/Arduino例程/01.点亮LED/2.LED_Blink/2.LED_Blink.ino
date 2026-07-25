/*
  该程序的作用是实现 LED 闪烁效果。
  在线文档：https://docs.geeksman.com/esp32/Arduino/05.esp32-arduino-LED.html
*/


// 设置 LED 引脚
int ledPin = 15;

void setup() {
  // 设定引脚为输出模式
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 点亮 LED
  digitalWrite(ledPin, HIGH);
  // 等待一段时间
  delay(1000);
  // 关闭 LED
  digitalWrite(ledPin, LOW);
  // 等待一段时间
  delay(1000);
}
