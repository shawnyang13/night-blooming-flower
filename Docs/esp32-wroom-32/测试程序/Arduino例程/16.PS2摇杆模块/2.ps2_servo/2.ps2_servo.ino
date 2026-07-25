/*
  该程序作用是通过 PS2 摇杆模块控制舵机转动
  在线文档：https://docs.geeksman.com/esp32/Arduino/21.esp32-arduino-ps2.html
*/
#define PS2_X      15
#define PS2_Y      2
#define SW         4
#define SERVO      13
#define RESOLUTION 12
#define CHANNEL    0
#define FREQ       50


int min_width = 0.6 / 20 * pow(2, RESOLUTION);
int max_width = 2.5 / 20 * pow(2, RESOLUTION);

int value;

void setup() {
  // 配置输入模式
  pinMode(PS2_X, INPUT);
  pinMode(PS2_Y, INPUT);
  pinMode(SW, INPUT_PULLUP);

  // 配置串口通信波特率
  Serial.begin(9600);

  // 设置 LEDC 通道的频率和分辨率
  ledcSetup(CHANNEL, FREQ, RESOLUTION);
  // 关联通道号与 GPIO 引脚
  ledcAttachPin(SERVO, CHANNEL);
}

void loop() {

  value = map(analogRead(PS2_Y), 0, pow(2, RESOLUTION), min_width, max_width);
  
  // 读取数值
  Serial.printf("x: %d, y: %d, z: %d, 映射后的 y: %d\n", analogRead(PS2_X), 
                analogRead(PS2_Y), digitalRead(SW), value);

  // 输出PWM
  ledcWrite(CHANNEL, value);

  delay(100);
}
