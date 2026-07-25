/*
  该程序的作用是使用函数显示动态数码管实验。
  在线文档：https://docs.geeksman.com/esp32/Arduino/07.esp32-arduino-7segment.html
*/


// 定义所有用到的引脚
int pin_a = 4;
int pin_b = 5;
int pin_c = 19;
int pin_d = 21;
int pin_e = 22;
int pin_f = 2;
int pin_g = 15;
int pin_dp = 18;

// 定义存放所有输出引脚的数组
int pin_array[8] = {pin_a, pin_b, pin_c, pin_d, pin_e, pin_f, pin_g, pin_dp};

// 定义数字显示逻辑的二维数组
int number_array[][8] = {
 //a, b, c, d, e, f, g, dp
  {0, 0, 0, 0, 0, 0, 1, 1}, // 0
  {1, 0, 0, 1, 1, 1, 1, 1}, // 1
  {0, 0, 1, 0, 0, 1, 0, 1}, // 2
  {0, 0, 0, 0, 1, 1, 0, 1}, // 3
  {1, 0, 0, 1, 1, 0, 0, 1}, // 4
  {0, 1, 0, 0, 1, 0, 0, 1}, // 5
  {0, 1, 0, 0, 0, 0, 0, 1}, // 6
  {0, 0, 0, 1, 1, 1, 1, 1}, // 7
  {0, 0, 0, 0, 0, 0, 0, 1}, // 8
  {0, 0, 0, 0, 1, 0, 0, 1}, // 9
};


// 定义显示数字的函数
void display_number(int num) {
  for (int i=0;i<8;i++) {
    digitalWrite(pin_array[i], number_array[num][i]);
    }
  }
  

void setup() {
  // 设置所有引脚为输出引脚,初始化所有引脚为高电平
  for (int i=0;i<8;i++){
    pinMode(pin_array[i], OUTPUT);
    digitalWrite(pin_array[i], HIGH);
    }
}


void loop(){
  // 显示数字
  for (int i=0;i<10;i++) {
    display_number(i);
    delay(500);
    }
}
