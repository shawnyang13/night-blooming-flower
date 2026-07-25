/*
  该程序作用是实现 6 键电子琴
  在线文档：https://docs.geeksman.com/esp32/Arduino/24.esp32-arduino-buzzer.html
*/
#define BUZZER      23
#define CHANNEL     0
#define RESOLUTION  8
#define FREQ        20000


// 定义按键数组
int button_array[6] = {25, 26, 27, 14, 12, 13};

// 定义音符频率数组
int tone_array[7] = {262, 294, 330, 350, 393, 441, 495};

// 初始化发声频率
int tone_value = 0;

void setup() {
  // 配置引脚模式
  pinMode(BUZZER, OUTPUT);

  for (int i=0;i<6;i++) {
    pinMode(button_array[i], INPUT_PULLUP);  
  }

  // LEDC 外设
  ledcSetup(CHANNEL, FREQ, RESOLUTION);
  ledcAttachPin(BUZZER, CHANNEL);
}

void loop() {
  // 检测按键是否按下
  for (int i=0;i<6;i++) {
    if (!digitalRead(button_array[i])) {
      tone_value = tone_array[i]; 
    }  
  }

  // 蜂鸣器发声
  ledcWriteTone(CHANNEL, tone_value);

  tone_value = 0;
  delay(10);
}
