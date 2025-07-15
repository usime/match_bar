import cv2
import numpy as np
import time
from UART import Usart
from detector import Detector


# 创建回调函数（实际上不需要，但我们使用全局变量）
def nothing(x):
    pass
Flag_valid_roi=1

x_target_roi=None ##初始矩形框左上角x坐标
y_target_roi=None ##初始矩形框左上角y坐标
w_target_roi=None ##初始矩形框w坐标
h_target_roi=None ##初始矩形框h坐标
if __name__ == '__main__':
    # 创建窗口和滑条
    cv2.namedWindow('Trackbars')
    cv2.createTrackbar('Canny Low', 'Trackbars', 20, 255, nothing)
    cv2.createTrackbar('Red H_min', 'Trackbars', 0, 180, nothing)
    cv2.createTrackbar('Red H_max', 'Trackbars', 6, 180, nothing)
    cv2.createTrackbar('Red S_min', 'Trackbars', 60, 255, nothing)
    cv2.createTrackbar('Red S_max', 'Trackbars', 255, 255, nothing)
    cv2.createTrackbar('Red V_min', 'Trackbars', 60, 255, nothing)
    cv2.createTrackbar('Red V_max', 'Trackbars', 255, 255, nothing)
    cv2.createTrackbar('Canny High', 'Trackbars', 180, 255, nothing)
    color_dist = {
        'red': {'Lower': np.array([0, 60, 60]), 'Upper': np.array([6, 255, 255])},
    }

    # 初始检测器
    cap = cv2.VideoCapture(0)
    detect = Detector(maxperimeter=2000, minperimeter=1000)
##0和76是可以看到铅笔线框的上下限值
    while True:
        ret, frame = cap.read()
        img=cv2.resize(frame, (320, 240))
        img_laser=img.copy()
        if not ret:
            print("无法接收帧 (流结束或摄像头未打开)")
            break
        # 获取滑条当前值
        canny_low = cv2.getTrackbarPos('Canny Low', 'Trackbars')
        canny_high = cv2.getTrackbarPos('Canny High', 'Trackbars')
        r_H_min = cv2.getTrackbarPos('Red H_min', 'Trackbars')
        r_H_max = cv2.getTrackbarPos('Red H_max', 'Trackbars')
        r_S_min = cv2.getTrackbarPos('Red S_min', 'Trackbars')
        r_S_max = cv2.getTrackbarPos('Red S_max', 'Trackbars')
        r_V_min = cv2.getTrackbarPos('Red V_min', 'Trackbars')
        r_V_max = cv2.getTrackbarPos('Red V_max', 'Trackbars')
        # 处理图像（使用滑条调整的Canny阈值）
        color_dist['red']['Lower'] = np.array([r_H_min, r_S_min, r_V_min])
        color_dist['red']['Upper'] = np.array([r_H_max, r_S_max, r_V_max])
        x1,y1=detect.color_red_detect(frame=img_laser,color_dist=color_dist)
        ##img_process = detect.Process_img(frame=img, canny_low=canny_low, canny_high=canny_high)
        ##cnt,x,y,w,h=detect.max_rect_roi_get(img_process)
        ##if cnt==1:
        ##   x_target_roi=x
        ##    y_target_roi=y
        ##    w_target_roi =w
        ##    h_target_roi =h
        ##print(cnt,x_target_roi,y_target_roi,w_target_roi,h_target_roi)
        ##cv2.imshow('test', img_process)
        print(x1,y1)
        cv2.imshow('Original', img_laser)  # 可选：显示原始图像

        # 退出条件
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # 清理资源
    cap.release()
    cv2.destroyAllWindows()