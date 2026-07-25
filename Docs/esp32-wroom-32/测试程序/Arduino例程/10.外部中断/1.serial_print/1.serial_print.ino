/*
  该程序的作用是实现在串口屏中显示中断触发信息
  在线文档：https://docs.geeksman.com/esp32/Arduino/15.esp32-arduino-interrupt.html
*/

#define BUTTON 14


// 定义可以在外部中断函数内部使用的变量
volatile bool flag = false;

void handle_interrupt() {
  flag = true;
}


void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLDOWN);

  // 配置中断引脚
  attachInterrupt(digitalPinToInterrupt(BUTTON), handle_interrupt, FALLING);
}


void loop() {
  if (flag) {
    Serial.println("外部中断触发了");
    flag = false;
    }
  
}
