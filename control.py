import socket
import threading
import tkinter as tk
import keyboard
import time

class TCPClient:
    def __init__(self, host, port):
        self.server_address = (host, port)
        self.sock = None
        self.is_running = False
        self.keys_down = set()  # 存储当前按下的键

    def connect(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect(self.server_address)
        self.is_running = True
        print("连接到服务器:", self.server_address)
        self.send_key_status()

    def send_key_status(self):
        while self.is_running:
            if self.keys_down:
                keys_list = list(self.keys_down)
                allowed_keys = {'w', 's', 'a', 'd'}
                
                # 过滤出有效按键
                filtered_keys = [key for key in keys_list if key in allowed_keys]

                if len(filtered_keys) == 1:
                    # 如果只按下一个允许的键，发送该键的两次
                    key_to_send = filtered_keys[0] * 2  # 例如 'w' 发送 'ww'
                elif len(filtered_keys) == 2:
                    # 检查是否是无效组合
                    if {'w', 's'}.issubset(filtered_keys) or {'a', 'd'}.issubset(filtered_keys):
                        key_to_send = 'n' *2  # 发送0表示无效组合
                    else:
                        key_to_send = ''.join(filtered_keys)  # 发送有效组合的字符，如 'wa'
                else:
                    key_to_send = 'n' *2  # 如果没有有效按键，则发送0
            else:
                key_to_send = 'n' *2  # 如果没有按键，则发送0

            self.sock.sendall(str(key_to_send).encode())
            time.sleep(0.1)  # 每100毫秒发送一次

    def close(self):
        self.is_running = False
        if self.sock:
            self.sock.close()
            print("断开与服务器的连接")

    def on_key_event(self, e):
        if e.event_type == keyboard.KEY_DOWN:
            self.keys_down.add(e.name)  # 添加按下的键
        elif e.event_type == keyboard.KEY_UP:
            self.keys_down.discard(e.name)  # 移除抬起的键


class Application(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("REMOTE CONTROL")
        self.geometry("300x150")
        self.label = tk.Label(self, text='Please press "W", "A", "S", "D"')
        self.label.pack(pady=10)
        self.client = None

        self.start_button = tk.Button(self, text="Start TCP Client", command=self.toggle_client)
        self.start_button.pack(pady=20)

        self.protocol("WM_DELETE_WINDOW", self.on_closing)

    def toggle_client(self):
        if self.client is None:
            host = "192.168.137.217"  # 替换为目标服务器的 IP
            port = 8080               # 替换为目标服务器的端口
            self.client = TCPClient(host, port)
            threading.Thread(target=self.client.connect, daemon=True).start()
            keyboard.hook(self.client.on_key_event)  # 监听键盘事件
            self.start_button.config(text="End TCP Client")  # 按钮文本更新
        else:
            self.client.close()
            self.client = None
            self.start_button.config(text="Start TCP Client")  # 按钮文本更新

    def on_closing(self):
        if self.client:
            self.client.close()
        self.destroy()

if __name__ == "__main__":
    app = Application()
    app.mainloop()
