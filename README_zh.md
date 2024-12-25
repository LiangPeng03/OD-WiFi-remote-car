# OD-WiFi-remote-car

[English](README.md) | [中文](README_zh.md)



一个通过电脑键盘输入进行操控的Wi-Fi遥控小车，具备图传和简单的避障功能。这个项目是我们嵌入式系统的课程期末项目，由于时间关系没有完全实现我们预期的效果，目前行驶的平稳性以及物体识别的通用性和准确度均有待提升。

## 硬件

* 小车框架以及马达：电商平台购买，选用了三轮小车，其中两个为驱动轮，前侧为万向轮，马达类型为tt马达。

* 充电宝：能够支持12V2A以上即可，至少需要一个USB输出接口和一个Type-C输出接口。

* 电机驱动模块：电商平台购买，L298N，支持12V输入5V输出。

* 开发板：TI TM4C123GH6PM系列，课程统一分配的主板。

* 摄像头：esp32-cam。

* 线材：若干公母头和双母头杜邦线，马达的驱动线需要进行焊接连接到驱动板。

* 拓展坞：Type-C转USB转接器，单独连接充电宝保证摄像头的供电。

* 电脑：能够使用Pytorch CUDA版本即可。

  

## 开发环境

* CCS：用于调试烧录马达PWM控制程序。
* VS Code：用于调试运行遥控程序和物体识别程序



## 项目结构

.
├── car/                                      # 车辆行驶模块
│   ├── Debug/                                # 编译输出目录
│   │   ├── car.out                           
│   │   └── ...                               # 其他编译中间文件
│   ├── motor.c                               # 电机驱动（PWM）
│   ├── motor.h
│   ├── WIFI.c                                # esp8266WiFi 通信模块
│   ├── WIFI.h
│   ├── main.c                                # 主程序
│   ├── main.h
│   └── ...                                   # 其他TivaWare库文件
├── esp32cam-pytorch-thread-mjpeg_yolo.py     # ESP32-CAM 图像处理和物体识别程序
├── control.py                     	   # 遥控程序

  

## 运行

* 首先需要保证供电，确保每个硬件正常运作。

  

* 在CCS中打开car文件夹，并且连接好TM4C123开发板到开发电脑，进行编译烧录（需要根据实际的马达转速在motor.c中修改PWM的占空比，以及在WIFI.c中修改热点的名称密码等）。

  ```c
  if(Wifi_Send_Cmd("AT+CWJAP=\"Name\",\"Password\"\r\n","OK",10000)== 1)
  ```

  

* 运行control.py文件，host和port分别修改为esp8266 Wi-Fi对应的局域网IP地址以及WIFI.c中设置的端口号。按下电脑的wasd键位测试马达是否正常运作。遇到拒绝连接的报错可以尝试重启开发板。

  ```c
  if(Wifi_Send_Cmd("AT+CIPSERVER=1,8080\r\n","OK",2000)== 1) //port number is 8080 here
  {
       Wifi_Send_Cmd("AT+CIFSR\r\n","OK",2000);
       return 1;
   }
  ```

  ```python
  if self.client is None:
      host = "192.168.137.217"  
      port = 8080               
  ```



* 运行esp32cam.py，修改url为esp32cam在局域网中的IP地址。摄像头所需的驱动以及调试步骤可以参考：https://gitee.com/lceda/esp32cam-pytorch-thread，该程序也修改自上述的参考项目，特此感谢。若返回的视频为全黑或有波纹可能说明供电不足或网络连接断开，请排查供电或网络问题；若视频返回几秒后不再更新可能是因为开发电脑没有正确调用CUDA，请检查是否正确安装CUDA版本的Pytorch。

  ```python
  url='http://192.168.137.120//'
  ```

  

## 详细说明

更详细的项目设计和代码解释请阅读Project Report文档。