/*
  该程序的作用是实现任意一位数码管显示任意数字
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


void setup() {
  // 设置所有的位选线引脚为输出模式，初始化所有的位选线引脚为高电平
  for (int i=0;i<4;i++) {
    pinMode(seg_array[i], OUTPUT);
    digitalWrite(seg_array[i], HIGH);
    }
  // 设置所有的段选线引脚为输出模式，初始化所有的段选线引脚为低电平
  for (int i=0;i<8;i++){
    pinMode(led_array[i], OUTPUT);
    digitalWrite(led_array[i], LOW);
    }
}

void loop() {
//  display_number(2, 4);
//  delay(1000);

  // 按顺序让所有位置显示0-9
  for (int i=0;i<4;i++) {
    for (int j=0;j<10;j++) {
      display_number(i, j);
      delay(200);
      }
    }
}


void display_number(int order, int number) {

  // 清屏
  clear();
  
  // 设置对应的位选线引脚为低电平
  digitalWrite(seg_array[order], LOW);

  // 给对应的段选线引脚设置对应的电平
  for (int i=0;i<8;i++) {
    digitalWrite(led_array[i], logic_array[number][i]);
  }
  }


void clear(){
  // 设置所有的位选线引脚为输出模式，初始化所有的位选线引脚为高电平
  for (int i=0;i<4;i++) {
    pinMode(seg_array[i], OUTPUT);
    digitalWrite(seg_array[i], HIGH);
    }
  // 设置所有的段选线引脚为输出模式，初始化所有的段选线引脚为低电平
  for (int i=0;i<8;i++){
    pinMode(led_array[i], OUTPUT);
    digitalWrite(led_array[i], LOW);
    }
}
