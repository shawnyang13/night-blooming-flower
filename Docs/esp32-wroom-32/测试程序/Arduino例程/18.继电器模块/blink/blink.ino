/*
  该程序作用是使用定时器中断实现继电器开关灯效果
  在线文档：https://docs.geeksman.com/esp32/Arduino/24.esp32-micropython-relay.html
*/
#define RELAY_PIN   15


// 初始化定时器
hw_timer_t * timer = NULL;


void timer_interrupt() {
  digitalWrite(RELAY_PIN, !digitalRead(RELAY_PIN));
  }

void setup() {
  // 设置为输出模式
  pinMode(RELAY_PIN, OUTPUT);

  // 初始化定时器
  timer = timerBegin(0, 80, true);
  // 注册中断处理函数
  timerAttachInterrupt(timer, timer_interrupt, true);

  // 设置定时器模式
  timerAlarmWrite(timer, 500000, true);

  // 启动定时器
  timerAlarmEnable(timer);
}

void loop() {

}
