/*
  该程序作用是使用宏定义按键实现开关灯效果
在线文档：https://docs.geeksman.com/esp32/Arduino/09.esp32-arduino-button.html
*/
// 宏定义 LED 和 按键引脚
#define LED_PIN       2
#define BUTTON_PIN    14


// 定义 LED 逻辑值
int led_logic = 0;
// 定义记录 LED 状态是否更改过的值
bool status = false;

void setup() {
  // 配置引脚模式
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
}


void loop() {
  // 按键消抖
  if (digitalRead(BUTTON_PIN)) {
    // 睡眠 10ms, 如果依然是高电平，说明是按下并非抖动
    delay(10);
    if (digitalRead(BUTTON_PIN) && !status) {
      led_logic = !led_logic;
      digitalWrite(LED_PIN, led_logic);
      // 修改 status 
      status = !status;
      } else if (!digitalRead(BUTTON_PIN)) {
        status = false;
        }
    }
}
