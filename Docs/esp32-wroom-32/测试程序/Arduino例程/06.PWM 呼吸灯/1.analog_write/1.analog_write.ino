/*
  该程序作用是使用 AnalogWrite() 函数实现呼吸灯效果
  在线文档：https://docs.geeksman.com/esp32/Arduino/10.esp32-arduino-pwm.html
*/
// LED 引脚
#define LED_PIN   12

void setup() {
  // 配置 GPIO 输出
  pinMode(LED_PIN, OUTPUT);
}


void loop() {
  // 实现渐亮效果
  for (int i=0;i<256;i++) {
    analogWrite(LED_PIN, i);
    delay(10);
    }
  // 实现渐灭效果
  for (int i=255;i>=0;i--) {
    analogWrite(LED_PIN, i);
    delay(10);
    }
}
