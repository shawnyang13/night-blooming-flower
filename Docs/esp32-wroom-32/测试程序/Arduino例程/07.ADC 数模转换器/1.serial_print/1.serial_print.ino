/*
  该程序作用是在串口屏中打印电位计的模拟输入信号
  在线文档：https://docs.geeksman.com/esp32/Arduino/11.esp32-arduino-adc.html
*/
// 定义 GPIO 引脚
#define POT 26


// 初始化电位计输入信号
int pot_value;


void setup() {
  // 设置串口通信波特率 9600；
  Serial.begin(9600);
  pinMode(POT, INPUT);
}


void loop() {
  // 读取电位计模拟输入值
  pot_value = analogRead(POT);
  // 打印模拟输入信号在串口监视器中
  Serial.println(pot_value);
  delay(50);
}
