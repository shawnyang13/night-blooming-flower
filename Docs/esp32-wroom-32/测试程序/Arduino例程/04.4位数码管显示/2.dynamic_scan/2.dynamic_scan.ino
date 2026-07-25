/*
  该程序的作用是通过动态扫描的方法实现 4 位数字显示的功能。
  在线文档：https://docs.geeksman.com/esp32/Arduino/08.esp32-arduino-4-digits-7segment.html
*/

// 定义位选线引脚
int seg_1 = 5;
int seg_2 = 18;
int seg_3 = 19;
int seg_4 = 21;

// 定义位选线数组;
int seg_array[4] = {seg_1, seg_2, seg_3, seg_4};

// 定义段选线引脚
int a = 32;
int b = 25;
int c = 27;
int d = 12;
int e = 13;
int f = 33;
int g = 26;
int dp = 14;

// 定义段选线数组
int led_array[8] = {a, b, c, d, e, f, g, dp};

// 定义共阴极数码管不同数字对应逻辑电平的二维数组
int logic_array[10][8] = {
 //a, b, c, d, e, f, g, dp
  {1, 1, 1, 1, 1, 1, 0, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1, 0}, // 2
  {1, 1, 1, 1, 0, 0, 1, 0}, // 3
  {0, 1, 1, 0, 0, 1, 1, 0}, // 4
  {1, 0, 1, 1, 0, 1, 1, 0}, // 5
  {1, 0, 1, 1, 1, 1, 1, 0}, // 6
  {1, 1, 1, 0, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1, 0}, // 8
  {1, 1, 1, 1, 0, 1, 1, 0}, // 9
  };


// 清屏函数
void clear(){
  for (int i=0;i<4;i++) {
    digitalWrite(seg_array[i], HIGH);
  }
  
  for (int i=0;i<8;i++){
    digitalWrite(led_array[i], LOW);
  }
}


// 显示数字的函数
void display_number(int order, int number) {

  // 清屏
  clear();

  // 把对应位选线的电平拉低
  digitalWrite(seg_array[order], LOW);

  // 显示数字
  for (int i=0;i<8;i++) {
    digitalWrite(led_array[i], logic_array[number][i]);
    }
}


// 4 位数码管显示函数
void display_4_number(int number) {
  // 把输入的数字转化为 4 位数的数组
  if (number < 10000){
    // 获取每一位对应的数字
    /*
    4567
    4567 % 10 = 7
    4567 / 10 = 456
    456 % 10 = 6
    456 / 10 = 45
    45 % 10 = 5
    45 / 10 = 4
    4 % 10 = 4

    1 
    1 % 10 = 1
    1 / 10 = 0
    
    */
    // 定义格式化数组
    int number_array[4];

    // 使用循环获取数组元素
    for (int i=3;i>=0;i--) {
      number_array[i] = number % 10;
      number /= 10;
      }

    // 显示 4 位数字
    for (int i=0;i<4;i++) {
      display_number(i, number_array[i]);
      delay(5);
      }
    }

}


void setup() {
  // 设置所有的位选线引脚为输出模式，初始化所有的位选线引脚为高电平
  for (int i=0;i<4;i++) {
    pinMode(seg_array[i], OUTPUT);
    digitalWrite(seg_array[i], HIGH);
    }

  // 设置所有的段选线引脚为输出模式，并初始化为低电平
  for (int i=0;i<8;i++){
    pinMode(led_array[i], OUTPUT);
    digitalWrite(led_array[i], LOW);
    }
}

void loop() {
  display_4_number(9876);
}
