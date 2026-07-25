/*
  该程序作用是使用 analogWrite 控制 LED 亮度
  在线文档：https://docs.geeksman.com/esp32/Arduino/11.esp32-arduino-adc.html
*/
// 定义 LED 和电位计引脚
#define LED 13
#define POT 26

// 初始化 LED 与电位计模拟信号
int pot_value;
int led_value;

void setup() {
  pinMode(POT, INPUT);
  pinMode(LED, OUTPUT);
}


void loop() {
  // 读取电位计模拟输入
  pot_value = analogRead(POT);
  // 把电位计模拟输入值转换为 LED 模拟输出
  led_value = pot_value / 16;
  analogWrite(LED, led_value);
  delay(50);
  
}
