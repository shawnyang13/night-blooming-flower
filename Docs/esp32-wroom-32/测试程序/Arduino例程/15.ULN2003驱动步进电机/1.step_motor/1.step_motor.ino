/*
  该程序作用是通过 ULN2003 驱动步进电机
  在线文档：https://docs.geeksman.com/esp32/Arduino/20.esp32-arduino-step-motor.html
*/
#define IN_1  13
#define IN_2  12
#define IN_3  14 
#define IN_4  27 


int delay_time = 5;

int degree = 180;
int count = 2048 / 4 * degree / 360;

void setup() {
  // 设置输出模式
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);

  // 单四拍模式
  for (int i=0;i<count;i++) {
    digitalWrite(IN_1, 1);
    digitalWrite(IN_2, 0);
    digitalWrite(IN_3, 0);
    digitalWrite(IN_4, 0);
    delay(delay_time);

    digitalWrite(IN_1, 0);
    digitalWrite(IN_2, 1);
    digitalWrite(IN_3, 0);
    digitalWrite(IN_4, 0);
    delay(delay_time);

    digitalWrite(IN_1, 0);
    digitalWrite(IN_2, 0);
    digitalWrite(IN_3, 1);
    digitalWrite(IN_4, 0);
    delay(delay_time);

    digitalWrite(IN_1, 0);
    digitalWrite(IN_2, 0);
    digitalWrite(IN_3, 0);
    digitalWrite(IN_4, 1);
    delay(delay_time);
    }

  delay(1000);

  // 双四拍模式
  for (int i=0;i<count;i++) {
      digitalWrite(IN_1, 1);
      digitalWrite(IN_2, 1);
      digitalWrite(IN_3, 0);
      digitalWrite(IN_4, 0);
      delay(delay_time);
  
      digitalWrite(IN_1, 0);
      digitalWrite(IN_2, 1);
      digitalWrite(IN_3, 1);
      digitalWrite(IN_4, 0);
      delay(delay_time);
  
      digitalWrite(IN_1, 0);
      digitalWrite(IN_2, 0);
      digitalWrite(IN_3, 1);
      digitalWrite(IN_4, 1);
      delay(delay_time);
  
      digitalWrite(IN_1, 1);
      digitalWrite(IN_2, 0);
      digitalWrite(IN_3, 0);
      digitalWrite(IN_4, 1);
      delay(delay_time);
      }

  delay(1000);

  // 双四拍模式，顺时针旋转
  for (int i=0;i<count;i++) {
      digitalWrite(IN_1, 1);
      digitalWrite(IN_2, 1);
      digitalWrite(IN_3, 0);
      digitalWrite(IN_4, 0);
      delay(delay_time);
  
      digitalWrite(IN_1, 1);
      digitalWrite(IN_2, 0);
      digitalWrite(IN_3, 0);
      digitalWrite(IN_4, 1);
      delay(delay_time);
  
      digitalWrite(IN_1, 0);
      digitalWrite(IN_2, 0);
      digitalWrite(IN_3, 1);
      digitalWrite(IN_4, 1);
      delay(delay_time);
  
      digitalWrite(IN_1, 0);
      digitalWrite(IN_2, 1);
      digitalWrite(IN_3, 1);
      digitalWrite(IN_4, 0);
      delay(delay_time);
      }

  delay(1000);

  // 八拍模式
  for (int i=0;i<count;i++) {
      digitalWrite(IN_1, 1);
      digitalWrite(IN_2, 0);
      digitalWrite(IN_3, 0);
      digitalWrite(IN_4, 0);
      delay(delay_time);
      
      digitalWrite(IN_1, 1);
      digitalWrite(IN_2, 1);
      digitalWrite(IN_3, 0);
      digitalWrite(IN_4, 0);
      delay(delay_time);

      digitalWrite(IN_1, 0);
      digitalWrite(IN_2, 1);
      digitalWrite(IN_3, 0);
      digitalWrite(IN_4, 0);
      delay(delay_time);
  
      digitalWrite(IN_1, 0);
      digitalWrite(IN_2, 1);
      digitalWrite(IN_3, 1);
      digitalWrite(IN_4, 0);
      delay(delay_time);

      digitalWrite(IN_1, 0);
      digitalWrite(IN_2, 0);
      digitalWrite(IN_3, 1);
      digitalWrite(IN_4, 0);
      delay(delay_time);
  
      digitalWrite(IN_1, 0);
      digitalWrite(IN_2, 0);
      digitalWrite(IN_3, 1);
      digitalWrite(IN_4, 1);
      delay(delay_time);

      digitalWrite(IN_1, 0);
      digitalWrite(IN_2, 0);
      digitalWrite(IN_3, 0);
      digitalWrite(IN_4, 1);
      delay(delay_time);
  
      digitalWrite(IN_1, 1);
      digitalWrite(IN_2, 0);
      digitalWrite(IN_3, 0);
      digitalWrite(IN_4, 1);
      delay(delay_time);
      }

  delay(1000);
  // 复位
  digitalWrite(IN_1, 0);
  digitalWrite(IN_2, 0);
  digitalWrite(IN_3, 0);
  digitalWrite(IN_4, 0);
}

void loop() {

}
