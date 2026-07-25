/*
  该程序的作用是使用外部中断检测按键触发，并点亮 LED，2 秒后熄灭
  在线文档：https://docs.geeksman.com/esp32/Arduino/15.esp32-arduino-interrupt.html
*/

#define BUTTON  14
#define LED     2


// 定义可以在外部中断中使用的变量
volatile bool flag = false;


void ISR() {
  flag = true;
  
}


void setup() {
  pinMode(BUTTON, INPUT_PULLDOWN);
  pinMode(LED, OUTPUT);

  // 配置中断引脚
  attachInterrupt(digitalPinToInterrupt(BUTTON), ISR, FALLING);
}


void loop() {
  if (flag) {
    digitalWrite(LED, HIGH);
    delay(2000);
    digitalWrite(LED, LOW);

    // 重置中断标志位
    flag = false;
    }
}
