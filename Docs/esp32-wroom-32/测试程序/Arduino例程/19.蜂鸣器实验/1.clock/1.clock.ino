/*
  该程序作用是使用定时器中断与外部中断实现有源蜂鸣器闹钟
  在线文档：https://docs.geeksman.com/esp32/Arduino/24.esp32-arduino-buzzer.html
*/
#define BUTTON  13
#define BUZZER  22


hw_timer_t * timer = NULL;

// 定义闹钟触发时间
int second = 3;


// 定时器中断处理函数
void timer_interrupt() {
  digitalWrite(BUZZER, HIGH);  
}

// 外部中断处理函数
void handle_interrupt() {
  digitalWrite(BUZZER, LOW);
}


void setup() {
  // 配置引脚模式
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  // 初始化定时器
  timer = timerBegin(0, 80, true);
  // 配置定时器
  timerAttachInterrupt(timer, timer_interrupt, true);
  // 定时模式
  timerAlarmWrite(timer, second*1000*1000, false);
  // 启动定时器
  timerAlarmEnable(timer);

  // 配置外部中断
  attachInterrupt(digitalPinToInterrupt(BUTTON), handle_interrupt, FALLING);

}

void loop() {

}
