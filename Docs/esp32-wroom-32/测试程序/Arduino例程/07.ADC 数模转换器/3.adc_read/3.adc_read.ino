/*
  该程序作用是使用 ADC 数模转换器控制 LED 亮度
  在线文档：https://docs.geeksman.com/esp32/Arduino/11.esp32-arduino-adc.html
*/
#define POT         26
#define LED         13
#define CHANNEL     0
#define RESOLUTION  12
#define FREQ        1000

// 初始化模拟输入值
int pot_value;


void setup() {
  // 设置 ADC 分辨率
  analogReadResolution(RESOLUTION);
  // 配置输入衰减
  analogSetAttenuation(ADC_11db);
  // 建立 LEDC 通道，配置 LEDC 分辨率
  ledcSetup(CHANNEL, FREQ, RESOLUTION);
  // 关联 GPIO 与 LEDC 通道
  ledcAttachPin(LED, CHANNEL);

}

void loop() {
  // 获取模拟信号输入值
  pot_value = analogRead(POT);
  // 输出 PWM
  ledcWrite(CHANNEL, pot_value);
  delay(50);

}
