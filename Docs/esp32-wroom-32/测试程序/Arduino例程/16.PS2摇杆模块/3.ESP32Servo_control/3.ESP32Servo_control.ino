/*
  该程序作用是通过 PS2 摇杆模块通过第三方库控制舵机转动
  在线文档：https://docs.geeksman.com/esp32/Arduino/21.esp32-arduino-ps2.html
*/
#include <ESP32Servo.h>


#define PS2_X      15
#define PS2_Y      2
#define SW         4
#define SERVO      13
#define RESOLUTION 12
#define FREQ       50


// 定义 Servo 对象
Servo my_servo;

int value;

void setup() {
  // 配置输入模式
  pinMode(PS2_X, INPUT);
  pinMode(PS2_Y, INPUT);
  pinMode(SW, INPUT_PULLUP);

  // 配置串口通信波特率
  Serial.begin(9600);

  // 分配硬件定时器
  ESP32PWM::allocateTimer(0);
  // 设置频率
  my_servo.setPeriodHertz(FREQ);
  // 关联 servo 对象与  GPIO 引脚，设置脉宽范围
  my_servo.attach(SERVO, 500, 2500);
  
}

void loop() {

  value = map(analogRead(PS2_Y), 0, pow(2, RESOLUTION), 0, 180);
  
  // 读取数值
  Serial.printf("x: %d, y: %d, z: %d, 映射后的 y: %d\n", analogRead(PS2_X), 
                analogRead(PS2_Y), digitalRead(SW), value);

  // 输出PWM
  my_servo.write(value);

  delay(100);
}
