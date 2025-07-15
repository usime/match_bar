from maix import camera,display,image,time
import struct
from maix import app, uart
import sys
from maix import time
from maix import app, uart, pinmap
class python_usart:
    def __init__(self, Pin_1, Pin_2, Rx, Tx, bitrate, device):
        pinmap.set_pin_function(Pin_1, Rx)
        pinmap.set_pin_function(Pin_2, Tx)
        self.device = device
        self.serial = uart.UART(self.device, bitrate)
        self.USART_REC_LEN = 200
        self.USART_RX_BUF = [0] * 200
        self.USART_RX_STA = 0
        self.USART_NewData = None
        self.RxState = 0
        self.mubiao_x = None
        self.mubiao_BF = None
    def usart_receive(self):
        rx_data = self.serial.read()
        return rx_data
    def uasrt_tranfrom(self,str):
        tx_data=self.serial.write(str)
Pin_1 = "A18"
Pin_2 = "A19"
Rx = "UART1_RX"
Tx = "UART1_TX"
device = "/dev/ttyS1" 
bitrate = 115200
Usart1 = python_usart(Pin_1, Pin_2, Rx, Tx, bitrate, device)

rx_data=Usart1.usart_receive()


while not app.need_exit():
    rx_str = rx_data.decode('utf-8')
    rx_pro_data=rx_str.rstrip()
    print(rx_pro_data)