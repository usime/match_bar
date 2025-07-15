import cv2
import numpy as np
import time
import serial
from serial.tools import list_ports


class Usart:
    def __init__(self, port=None, baudrate=115200, timeout=1):
        self.ser = None
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout

        # 如果指定了端口，尝试自动连接
        if port:
            self.connect()

    def connect(self):
        """连接到指定的串口设备"""
        if self.ser and self.ser.is_open:
            self.disconnect()

        try:
            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=self.timeout
            )
            print(f"成功连接到串口: {self.port} @ {self.baudrate} bps")
            return True
        except serial.SerialException as e:
            print(f"连接失败: {e}")
            return False

    def disconnect(self):
        """关闭串口连接"""
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("串口连接已关闭")

    def send_data(self, data):
        """发送数据到串口"""
        if not self.ser or not self.ser.is_open:
            print("错误：串口未连接")
            return False

        try:
            if isinstance(data, str):
                self.ser.write(data.encode())
            elif isinstance(data, bytes):
                self.ser.write(data)
            else:
                print("错误：不支持的数据类型")
                return False

            return True
        except serial.SerialException as e:
            print(f"发送失败: {e}")
            return False

    def receive_data(self, size=1):
        """从串口接收数据"""
        if not self.ser or not self.ser.is_open:
            print("错误：串口未连接")
            return None

        try:
            data = self.ser.read(size)
            return data.decode() if data else ""
        except serial.SerialException as e:
            print(f"接收失败: {e}")
            return None

    def receive_line(self):
        """接收一行数据（以换行符结束）"""
        if not self.ser or not self.ser.is_open:
            print("错误：串口未连接")
            return None

        try:
            line = self.ser.readline()
            return line.decode().strip()
        except serial.SerialException as e:
            print(f"接收失败: {e}")
            return None

    def get_available_ports(self):
        """获取所有可用串口列表"""
        ports = list_ports.comports()
        return [port.device for port in ports]

    def __del__(self):
        self.disconnect()
