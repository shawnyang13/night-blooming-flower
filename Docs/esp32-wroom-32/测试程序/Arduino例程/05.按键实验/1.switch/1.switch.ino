/*
  该程序作用是使用按键实现开关灯效果
  在线文档：https://docs.geeksman.com/esp32/Arduino/09.esp32-arduino-button.html
*/

// 定义 LED 和按键引脚
int led_pin = 2;
int button_pin = 14;

// 定义 LED 逻辑值
int led_logic = 0;
// 定义记录 LED 状态是否更改过的值
bool status = false;

void setup() {
  // 配置引脚模式
  pinMode(led_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLDOWN);
}


void loop() {
  // 按键消抖
  if (digitalRead(button_pin)) {
    // 睡眠 10ms, 如果依然是高电平，说明是按下并非抖动
    delay(10);
    if (digitalRead(button_pin) && !status) {
      led_logic = !led_logic;
      digitalWrite(led_pin, led_logic);
      // 修改 status 
      status = !status;
      } else if (!digitalRead(button_pin)) {
        status = false;
        }
    }
}
