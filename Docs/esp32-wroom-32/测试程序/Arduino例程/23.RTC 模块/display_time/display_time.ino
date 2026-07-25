#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#define NTP "ntp.aliyun.com"


// 初始化 LCD1602
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 定义时间信息
struct tm timeinfo;

// WiFi 账号密码
const char *ssid = "你的 WiFi 名称";
const char *password = "你的 WiFi 密码";

// 连接 WiFi
void wifi_init(){
  // 设置 ESP32 工作模式
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("正在连接 WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);  
  }
  Serial.println("WiFi 连接成功");
}

// 设置并获取本地时间
void time_init() {
  // 获取本地时间
  if (!getLocalTime(&timeinfo)) {
    Serial.println("获取本地时间失败");
    // 连接 WiFi
    wifi_init();
    // 从 NTP 服务器获取时间并设置
    configTime(8*3600, 0, NTP); 
    return;
  }

  // 输出时间
//  Serial.println(timeinfo.tm_year)
  Serial.println(&timeinfo, "%F %A %T");

  // 清屏
  lcd.clear();
  // 显示内容
  lcd.setCursor(0, 0);
  lcd.print(&timeinfo, "%F");
  lcd.setCursor(13, 0);
  lcd.print(&timeinfo, "%a");
  lcd.setCursor(8,1);
  lcd.print(&timeinfo, "%T");
  
}


void setup() {
  Serial.begin(115200);
  wifi_init();
  // 初始化 LCD 对象
  lcd.init();
  lcd.backlight();

}

void loop() {
  time_init();
  delay(1000);
}
