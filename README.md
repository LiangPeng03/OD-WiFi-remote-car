# OD-WiFi-remote-car

[English](README.md) | [中文](README_zh.md)



A Wi-Fi remote-control car controlled by computer keyboard input, with Wi-Fi image transmit and simple obstacle avoidance functions. This project is the final project of our embedded systems course, due to time constraints we could not fully realize our expected results, the smoothness of the current driving as well as object detection versatility and accuracy need to be improved.

## Hardware

* **The frame of the cart as well as the motor**: purchased on e-commerce platform, we chose a three-wheeled cart, two of which are driving wheels, the front side is a gimbal wheel, and the motor type is tt-motor.

* **Power bank**: Support more than 12V2A, which has at least a USB output interface and a Type-C output interface.

* **Motor driver module**: purchased on e-commerce platform, L298N, support 12V input 5V output.

* **Development board**: TI TM4C123GH6PM series, allocated on course.

* **Camera**: esp32-cam.

* **Wire**: a number DuPont wire, the motor's drive line needs to be soldered to connect to the driver board.

* **Docking station**: Type-C to USB adapter, separately connected to the power bank to ensure the power supply of the camera.

* **Computer**: Contains Pytorch CUDA version.



## Development Environment

* CCS: For debugging and burning motor PWM control program.
* VS Code: For debugging and running remote control program and object detection program.



## Project Structure

.
├── car/                                      # Cart movement module
│   ├── Debug/                                # Compile output
│   │   ├── car.out                           
│   │   └── ...                               # Other compilation intermediate files
│   ├── motor.c                               # driver of motors (PWM)
│   ├── motor.h
│   ├── WIFI.c                                # esp8266WiFi communication module
│   ├── WIFI.h
│   ├── main.c                                # main
│   ├── main.h
│   └── ...                                   # Other TivaWare library files
├── esp32cam-pytorch-thread-mjpeg_yolo.py     # ESP32-CAM image processing and object detection program
├── control.py                     	   # Remote control program



## Run

* The first thing to do is ensure sufficient power supply so that every piece of hardware is functioning properly.

  

* Open the car folder in CCS and connect the TM4C123 development board to the development computer to compile and burn (need to modify the duty cycle of PWM in motor.c according to the actual motor speed and the name and password of hotspot in WIFI.c).

  ```c
  if(Wifi_Send_Cmd("AT+CWJAP=\"Name\",\"Password\"\r\n","OK",10000)== 1)
  ```



* Run control.py and change host and port to the LAN IP address corresponding to the esp8266 Wi-Fi and the port number set in WIFI.c respectively. Press the wasd keys on your computer to test if the motor is functioning properly. Try restarting the board if you get a connection refused error.

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



* Run esp32cam.py and change the url to the IP address of esp32cam in LAN. The required drivers and debugging steps for the camera can be found at https://gitee.com/lceda/esp32cam-pytorch-thread, the program is also modified from the above reference project. If the returned video is all black or has ripples, it may mean that the power supply is insufficient or the network connection is disconnected, please check the power supply or network problems; if the video is not updated after a few seconds, it may be because the development computer did not call CUDA correctly, please check whether the CUDA version of Pytorch is installed correctly.

  ```python
  url='http://192.168.137.120//'
  ```



## Details

For a more detailed explanation of the project design and code please read the Project Report document.
