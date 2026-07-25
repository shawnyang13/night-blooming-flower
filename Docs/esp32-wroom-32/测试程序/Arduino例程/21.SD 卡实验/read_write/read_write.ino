/*
  该程序作用是使用 SD 卡模块对内存卡执行文件操作
  在线文档：https://docs.geeksman.com/esp32/Arduino/26.esp32-arduino-sd.html
*/
#include <SD.h>


// 初始化文件对象
File my_file;

void setup() {
  Serial.begin(9600);

  delay(1000);

  if (!SD.begin()){
    Serial.println("SD 卡初始化失败");
    return;  
  }

  Serial.println("SD 卡初始化成功");

  list_files("/");

  // 创建一个新的文件并写入数据
  my_file = SD.open("/test.txt", FILE_WRITE);

  if (my_file) {
    my_file.println("你好，SD 卡");
    my_file.close();   
  }else {
    Serial.println("无法打开文件");  
  }

  delay(1000);

  // 读取文件内容
  my_file = SD.open("/test.txt");
  while (my_file.available()) {
    Serial.write(my_file.read());
  }
  my_file.close();
  Serial.println("\n文件读取完成");

  delay(1000);

  // 追加文件内容
  my_file = SD.open("/test.txt", FILE_APPEND);
  my_file.println("修改文件内容");
  my_file.close();
  Serial.println("文件修改成功");

  delay(1000);

  // 读取文件内容
  my_file = SD.open("/test.txt");
  while (my_file.available()) {
    Serial.write(my_file.read());
  }
  my_file.close();
  Serial.println("\n文件读取完成");

  delay(1000);

  list_files("/");

  // 删除文件
  if (SD.remove("/test.txt")) {
    Serial.println("文件删除成功"); 
  }else {
    Serial.println("文件删除失败");  
  }

}


void list_files(const char * path) {
  Serial.println("文件列表：");

  // 打开文件路径
  File root = SD.open(path);

  // 遍历当前目录的文件列表
  File file = root.openNextFile();
  while (file) {
    // 判断文件类型
    if (file.isDirectory()) {
      Serial.print("目录：");  
    } else {
      Serial.print("文件：");    
    }
    Serial.println(file.name());
    // 遍历下一个文件
    file = root.openNextFile();
  }
  // 关闭文件操作
  file.close();
  root.close();
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
