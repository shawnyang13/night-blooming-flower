/*
  该程序作用是实现自动播放音乐
  在线文档：https://docs.geeksman.com/esp32/Arduino/24.esp32-arduino-buzzer.html
*/
#define BUZZER      23
#define CHANNEL     0
#define RESOLUTION  8
#define FREQ        20000


// 定义音符频率数组
int tone_array[8] = {0, 262, 294, 330, 350, 393, 441, 495};

// 乐谱
int music[] = {1, 1, 5, 5, 6, 6, 5, 0,
               4, 4, 3, 3, 2, 2, 1, 0,
               5, 5, 4, 4, 3, 3, 2, 0,
               5, 5, 4, 4, 3, 3, 2, 0, 
               1, 1, 5, 5, 6, 6, 5, 0,
               4, 4, 3, 3, 2, 2, 1, 0};

// 初始化发声频率
int tone_value = 0;

void setup() {
  // 配置引脚模式
  pinMode(BUZZER, OUTPUT);

  // LEDC 外设
  ledcSetup(CHANNEL, FREQ, RESOLUTION);
  ledcAttachPin(BUZZER, CHANNEL);
}

void loop() {
  for (int i=0; i<sizeof(music)/sizeof(music[0]);i++) {
    ledcWriteTone(CHANNEL, tone_array[music[i]]);
    delay(500);
  }

  delay(2000);
}
