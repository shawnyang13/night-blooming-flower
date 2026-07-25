/*
  该程序作用是通过第三方库驱动步进电机
  在线文档：https://docs.geeksman.com/esp32/Arduino/20.esp32-arduino-step-motor.html
*/

#include <CheapStepper.h>

// 创建对象
CheapStepper stepper (13, 12, 14, 27); 


bool moveClockwise = true;

void setup() {
  // 设置转速
  stepper.setRpm(10); 

  // 根据步数旋转
  stepper.moveTo (moveClockwise, 2048);

  delay(1000);

  // 根据角度旋转
  stepper.moveDegrees (moveClockwise, 90);
}

void loop() {
  // put your main code here, to run repeatedly:

}
