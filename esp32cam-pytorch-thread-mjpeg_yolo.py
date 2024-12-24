# 多线程
import threading
import keyboard

# loop1 解析mjpeg http流 https://blog.csdn.net/Barry_J/article/details/101280263
import requests
import numpy as np
import builtins
import urllib.request

# loop3 modelscope框架神经网络 https://modelscope.cn/models/damo/cv_convnext-base_image-classification_garbage/summary
from modelscope.models import Model
from modelscope.pipelines import pipeline
from modelscope.utils.constant import Tasks
import time

# loop2 opencv
import cv2
from vis import vis_det_img, cv2AddChineseText

import socket
import json

def loop1():
    '''esp32cam image mjpeg'''
    global img
    r = requests.get(url + 'cam.mjpeg', stream=True)
    print('test')

    if r.status_code == 200:
        bytes = builtins.bytes()
        for chunk in r.iter_content(chunk_size=1024):
            if chunk:  # 确保获取到的块不为空
                bytes += chunk
            
            a = bytes.find(b'\xff\xd8')
            b = bytes.find(b'\xff\xd9')
            
            if a != -1 and b != -1:
                # print('find jpg')
                jpg = bytes[a:b + 2]
                bytes = bytes[b + 2:]
                
                # 确保jpg不为空
                if len(jpg) > 0:
                    img = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
                    if img is None:
                        print("Image decoding failed, created an empty image.")
                else:
                    print("No valid JPEG data found.")
                
            if exit_flag:
                break
    else:
        print("Received unexpected status code {}".format(r.status_code))
    print('loop1 exit normally')

# def loop1():
#     '''esp32cam image jpg'''
#     global img
#     while not exit_flag:
#         try:
#             imgResp=urllib.request.urlopen(url+'cam-hi.jpg')
#             imgNp=np.array(bytearray(imgResp.read()),dtype=np.uint8)
#             img=cv2.imdecode(imgNp,-1)
#         except urllib.error.URLError as e:
#             print(e.reason)
#             pass
#     print('loop1 exit normally')

def loop2():
    '''opencv processing'''
    while not exit_flag:
        img_puttext = img.copy()
        result_c_puttext = result_c.copy()
        result_d_puttext = result_d.copy()
        if model_sel == 'g' or model_sel == 'd' or model_sel == 'r':
            '''image classification'''
            if model_sel == 'g':
                img_puttext = cv2AddChineseText(img_puttext, '垃圾分类', (4, 6), (0, 255, 0), 20)
            elif model_sel == 'd':
                img_puttext = cv2AddChineseText(img_puttext, '日常用品', (4, 6), (0, 255, 0), 20)
            elif model_sel == 'r':
                img_puttext = cv2AddChineseText(img_puttext, '万物识别', (4, 6), (0, 255, 0), 20)
            for i in range(len(result_c_puttext['labels'])):
                text = str(result_c_puttext['labels'][i]) + ' ' + str(result_c_puttext['scores'][i])
                img_puttext = cv2AddChineseText(img_puttext, text, (4, 26 + 20 * i), (0, 255, 0), 20)
        elif model_sel == 'o' or model_sel == 'h' or model_sel == 'f' or model_sel == 'p':
            '''object detection'''
            if model_sel == 'o':
                img_puttext = cv2AddChineseText(img_puttext, '通用目标检测', (4, 6), (0, 255, 0), 20)
            elif model_sel == 'h':
                img_puttext = cv2AddChineseText(img_puttext, '人头检测', (4, 6), (0, 255, 0), 20)
            elif model_sel == 'f':
                img_puttext = cv2AddChineseText(img_puttext, '口罩检测', (4, 6), (0, 255, 0), 20)
            elif model_sel == 'p':
                img_puttext = cv2AddChineseText(img_puttext, '手机检测', (4, 6), (0, 255, 0), 20)
            try:
                img_puttext = vis_det_img(img_puttext, result_d_puttext)
            except cv2.error:
                pass
        cv2.imshow('esp32cam-test',img_puttext)
        if ord('q')==cv2.waitKey(10):
            break
    cv2.destroyAllWindows()
    print('loop2 exit normally')
    
def loop3():
    '''modelscope processing'''
    global result_c
    global result_d
    global stop
    while not exit_flag:
        if model_sel == 'g' or model_sel == 'd' or model_sel == 'r':
            '''image classification'''
            if model_sel == 'g':
                result_c = garbage_classification(img)
            elif model_sel == 'd':
                result_c = dailylife_classification(img)
            elif model_sel == 'r':
                result_c = general_recognition(img)
            print(result_c)
        elif model_sel == 'o' or model_sel == 'h' or model_sel == 'f' or model_sel == 'p':
            '''object detection'''
            if model_sel == 'o':
                result_d = object_detection(img)
            elif model_sel == 'h':
                result_d = head_detection(img)
            elif model_sel == 'f':
                result_d = facemask_detection(img)
            elif model_sel == 'p':
                result_d = phone_detection(img)
            print(result_d)
            if result_d['labels'] == ['bottle']and result_d['boxes'][0][2]-result_d['boxes'][0][0]>80:
                # 发送信号“nn”到服务器
                try:
                    # 创建一个 TCP/IP socket
                    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

                    # 连接到服务器
                    server_address = ('192.168.137.217', 8080)  # 使用服务器地址和端口
                    sock.connect(server_address)

                    # 发送 HTTP 请求                    
                    tt = "ss"                  
                    
                    sock.sendall(tt.encode())
                    time.sleep(0.1)
                    # 接收响应
                    # response = sock.recv(4096)  # 接收4096字节的响应
                    # print(f"Signal sent to server, response: {response.decode()}")
                    
                except Exception as e:
                    print(f"Failed to send signal: {e}")
                
    print('loop3 exit normally')

def keyboard_callback(x):
    global model_sel
    print(x)
    if x.name == 'g':
        print('garbage_classification')
        model_sel = 'g'
    elif x.name == 'd':
        print('dailylife_classification')
        model_sel = 'd'
    elif x.name == 'r':
        print('general_recognition')
        model_sel = 'r'
    elif x.name == 'o':
        print('object_detection')
        model_sel = 'o'
    elif x.name == 'h':
        print('head_detection')
        model_sel = 'h'
    elif x.name == 'f':
        print('facemask_detection')
        model_sel = 'f'
    elif x.name == 'p':
        print('phone_detection')
        model_sel = 'p'
    
if __name__ == '__main__':
    '''loop1 initialization'''
    # 初始化esp32cam url
    url='http://192.168.137.120//' # 改成串口收到的ESP32-CAM内网url，检查ESP32-CAM和上位机是不是在一个局域网网段上。

    # 初始化global img变量，存储esp32cam捕获到的图片（opencv格式）
    img = np.zeros((296, 400, 3), np.uint8)

    '''loop3 initialization'''
    # 导入模型和pipeline（loop3）
    '''image classification'''
    garbage_classification = pipeline(Tasks.image_classification, model='damo/cv_convnext-base_image-classification_garbage')
    dailylife_classification = pipeline(Tasks.image_classification, model='damo/cv_vit-base_image-classification_Dailylife-labels')
    general_recognition = pipeline(Tasks.general_recognition, model='damo/cv_resnest101_general_recognition')
    
    '''object detection'''
    object_detection = pipeline(Tasks.image_object_detection,model='damo/cv_tinynas_object-detection_damoyolo')
    #head_detection = pipeline(Tasks.domain_specific_object_detection, model='damo/cv_tinynas_head-detection_damoyolo')
    #facemask_detection = pipeline(Tasks.domain_specific_object_detection, model='damo/cv_tinynas_object-detection_damoyolo_facemask')
    phone_detection = pipeline(Tasks.domain_specific_object_detection, model='damo/cv_tinynas_object-detection_damoyolo_phone')

    # 初始化global result_c和global result_d变量，用于存储模型推理的结果（loop3）
    result_c = {'scores': [0,0,0,0,0], 'labels': ['','','','','']}
    result_d = {'scores': [], 'labels': [], 'boxes': []}

    stop = False
    
    '''keyboard_callback initialization'''
    # 初始化global model_sel变量，用于存储模型选择的标识符（keyboard_callback）
    model_sel = 'o'

    # 创建按键监听线程，按下任何按键时（包括长按），都会调用keyboard_callback，并传入键盘事件作为参量
    #skeyboard.on_press(keyboard_callback)

    # 初始化控制线程正常退出的标志位
    exit_flag = False

    # 创建线程
    t1 = threading.Thread(target=loop1)
    t2 = threading.Thread(target=loop2)
    t3 = threading.Thread(target=loop3)
    # 启动线程
    t1.start()
    t2.start()
    t3.start()
    # 等待用户输入 q （阻塞等待）
    keyboard.wait('q')
    # 如果用户输入 q 则置标志位为True，告知线程退出循环
    exit_flag = True
    # 等待线程结束
    t1.join()
    t2.join()
    t3.join()
    print('main thread exit normally')