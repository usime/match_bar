import time

import cv2
import numpy as np
from detector import Detector
from UART import Usart

# 回调函数，用于滑条的回调
def nothing(x):
    pass



def create_trackbars():
    """创建所有滑条"""
    # 创建滑条窗口
    cv2.namedWindow('Trackbars')

    # Canny边缘检测滑条
    cv2.createTrackbar('Canny Low', 'Trackbars', 20, 255, nothing)
    cv2.createTrackbar('Canny High', 'Trackbars', 80, 255, nothing)

    # 红色激光HSV阈值滑条
    cv2.createTrackbar('Red H_min1', 'Trackbars', 0, 180, nothing)
    cv2.createTrackbar('Red H_max1', 'Trackbars', 10, 180, nothing)
    cv2.createTrackbar('Red H_min2', 'Trackbars', 170, 180, nothing)
    cv2.createTrackbar('Red H_max2', 'Trackbars', 180, 180, nothing)
    cv2.createTrackbar('Red S_min', 'Trackbars', 60, 255, nothing)
    cv2.createTrackbar('Red S_max', 'Trackbars', 255, 255, nothing)
    cv2.createTrackbar('Red V_min', 'Trackbars', 60, 255, nothing)
    cv2.createTrackbar('Red V_max', 'Trackbars', 255, 255, nothing)

    # 绿色激光HSV阈值滑条
    cv2.createTrackbar('Green H_min', 'Trackbars', 35, 180, nothing)
    cv2.createTrackbar('Green H_max', 'Trackbars', 90, 180, nothing)
    cv2.createTrackbar('Green S_min', 'Trackbars', 60, 255, nothing)
    cv2.createTrackbar('Green S_max', 'Trackbars', 255, 255, nothing)
    cv2.createTrackbar('Green V_min', 'Trackbars', 60, 255, nothing)
    cv2.createTrackbar('Green V_max', 'Trackbars', 255, 255, nothing)


def get_trackbar_values():
    """获取所有滑条的值"""
    # Canny边缘检测参数
    canny_low = cv2.getTrackbarPos('Canny Low', 'Trackbars')
    canny_high = cv2.getTrackbarPos('Canny High', 'Trackbars')
    # 红色激光参数
    r_h_min1 = cv2.getTrackbarPos('Red H_min1', 'Trackbars')
    r_h_max1 = cv2.getTrackbarPos('Red H_max1', 'Trackbars')
    r_h_min2 = cv2.getTrackbarPos('Red H_min2', 'Trackbars')
    r_h_max2 = cv2.getTrackbarPos('Red H_max2', 'Trackbars')
    r_s_min = cv2.getTrackbarPos('Red S_min', 'Trackbars')
    r_s_max = cv2.getTrackbarPos('Red S_max', 'Trackbars')
    r_v_min = cv2.getTrackbarPos('Red V_min', 'Trackbars')
    r_v_max = cv2.getTrackbarPos('Red V_max', 'Trackbars')
    return {
        'canny': {'low': canny_low, 'high': canny_high},
        'red': {
            'Lower1': np.array([r_h_min1, r_s_min, r_v_min]),
            'Upper1': np.array([r_h_max1, r_s_max, r_v_max]),
            'Lower2': np.array([r_h_min2, r_s_min, r_v_min]),
            'Upper2': np.array([r_h_max2, r_s_max, r_v_max])
        },
    }
def Task(Flag_task,red_point,target_index):
    if Flag_task in ['MODE2', 'MODE3']:
        return None, None, None, None
    if Flag_task == 'MODE1':
        try:
            # 确保角点数据有效且格式正确
            if (corners_reshaped is None or
                    len(corners_reshaped) != 4 or
                    any(len(corner) != 2 for corner in corners_reshaped)):
                print("错误：无效的角点数据格式")
                return None, None, None, None
            # 检查red_point是否有效
            if (red_point is None or
                    len(red_point) != 2 or
                    red_point[0] is None or
                    red_point[1] is None):
                print("警告：无效的红色激光点坐标")
                return None, None, None, None
            # 根据当前目标索引获取目标点坐标
            target_point = corners_reshaped[target_index]
            print(f"当前目标点索引: {target_index}, 坐标: ({target_point[0]}, {target_point[1]})")

            # 处理可能的numpy数组类型
            red_x = float(red_point[0]) if not isinstance(red_point[0], float) else red_point[0]
            red_y = float(red_point[1]) if not isinstance(red_point[1], float) else red_point[1]

            # 计算激光点到当前目标点的距离
            distance_x = red_x - target_point[0]-10
            distance_y = red_y - target_point[1]-2

            # 打印调试信息
            print(f"红点位置: ({red_x}, {red_y}), 目标位置: ({target_point[0]}, {target_point[1]})")
            print(f"X距离: {distance_x:.1f}px, Y距离: {distance_y:.1f}px")

            # 当距离小于阈值时，标记已到达
            if abs(distance_x) <=3 and abs(distance_y)<=3:
                arrived = True
                print(f"已到达目标点 {target_index}")
                # 更新到下一个目标点（顺序：3->0->1->2->3...）
                if target_index == 3:
                    target_index = 0
                    print(target_index)
                elif target_index == 0:
                    target_index = 1
                    print(target_index)
                elif target_index == 1:
                    target_index = 2
                    print(target_index)
                elif target_index == 2:
                    target_index = 3

            else:
                arrived = False

            # 返回相对于目标点的误差
            error_x = distance_x
            error_y = distance_y

            # 确定方向标志（根据误差方向）
            dir_x = 0 if error_x > 0 else 1
            dir_y = 0 if error_y > 0 else 1

            return error_x, error_y, (dir_x, dir_y),target_index

        except Exception as e:
            import traceback
            print(f"计算误差时出错: {str(e)}")
            traceback.print_exc()  # 打印详细的错误堆栈
            return None, None, None, None
    if Flag_task == 'MODE4':
        try:
            # 确保角点数据有效且格式正确
            if (corners_reshaped is None or
                    len(corners_reshaped) != 4 or
                    any(len(corner) != 2 for corner in corners_reshaped)):
                print("错误：无效的角点数据格式")
                return None, None,None

            # 检查red_point是否有效
            if (red_point is None or
                    len(red_point) != 2 or
                    red_point[0] is None or
                    red_point[1] is None):
                print("警告：无效的红色激光点坐标")
                return None, None, None, None

            top_left, top_right, bottom_right, bottom_left = corners_reshaped

            # 计算中心点坐标
            center_x = (top_left[0] + top_right[0] + bottom_right[0] + bottom_left[0]) / 4.0
            center_y = (top_left[1] + top_right[1] + bottom_right[1] + bottom_left[1]) / 4.0

            # 处理可能的numpy数组类型
            red_x = float(red_point[0]) if not isinstance(red_point[0], float) else red_point[0]
            red_y = float(red_point[1]) if not isinstance(red_point[1], float) else red_point[1]
            # 计算坐标差值
            error_x = red_x - center_x
            error_y = red_y - center_y
            # 打印调试信息
            print(f"红点位置: ({red_x}, {red_y}), 中心位置: ({center_x:.1f}, {center_y:.1f})")
            print(f"X误差: {error_x:.1f}px, Y误差: {error_y:.1f}px")
            if error_x>0:
                num1=0
            else:
                num1=1
            if error_y>0:
                num2=0
            else:
                num2=1
            return error_x, error_y,(num1,num2),target_index

        except Exception as e:
            import traceback
            print(f"计算误差时出错: {str(e)}")
            traceback.print_exc()  # 打印详细的错误堆栈
            return None, None,None, None
##变量接收
rect_status=0
num_detect=()
sorted_corners=[]
target_index=3
if __name__ == '__main__':
    # 打开摄像头
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("无法打开摄像头")
        exit()

    # 创建检测器实例
    detector = Detector(maxperimeter=2000, minperimeter=1000)
    usart1=Usart(port='COM3',timeout=200,baudrate=9600)
    # 创建滑条
    create_trackbars()

    # 主循环
    while True:
        # 读取一帧图像
        ret, frame = cap.read()
        if not ret:
            print("无法接收帧 (流结束或摄像头未打开)")
            break

        # 调整图像大小
        img = cv2.resize(frame, (640, 480))
        red_img=img.copy()

        rect_roi_img=img.copy()
        # 获取滑条值
        trackbar_values = get_trackbar_values()

        # 处理图像（使用滑条调整的Canny阈值）
        canny_low = trackbar_values['canny']['low']
        canny_high = trackbar_values['canny']['high']
        img_process = detector.Process_img(img.copy(), canny_low, canny_high)
        # 检测矩形框
        if rect_status==0:
            rect_status, x, y, w, h = detector.max_rect_roi_get(rect_roi_img,detect_value=rect_status)
        # 检测激光点
        red_point, mask_red= detector.color_laser_red_detect(
            red_img, trackbar_values
        )

        cv2.imshow('Original1', rect_roi_img)  # 原始图像
        cv2.imshow('Canny Edges', img_process)  # Canny边缘检测结果

        if rect_status==1:
            x1 = max(0, x - 40)
            y1 = max(0, y - 40)
            x2 = min(frame.shape[1], x + w + 40)
            y2 = min(frame.shape[0], y + h + 40)
            roi = frame[y1:y2, x1:x2]
            roi_processd_img=detector.Process_img(roi,canny_low, canny_high)
            sorted_corners,x_center,y_center,roi_img=detector.Find_max_countour(roi=roi.copy(),processed_roi_img=roi_processd_img,detected_corners=sorted_corners)

            if not sorted_corners:  # 检查列表是否为空
                corners_reshaped = np.empty((0, 2), dtype=np.int32)  # 创建空数组
            else:
                if isinstance(sorted_corners[0], np.ndarray):
                    corners_array = sorted_corners[0]
                    corners_reshaped = corners_array.reshape(-1, 2)  # 重塑形状
                else:
                    corners_reshaped = np.array(sorted_corners, dtype=np.int32).reshape(-1, 2)
            print(corners_reshaped)
           # cv2.circle(roi_img, (int(round(x_center)), int(round(y_center))), 5, (0, 0, 255), 2)
            cv2.imshow('roi2', roi_img)
            cv2.imshow('roi1', roi_processd_img)
            cv2.imshow('red', red_img)

            result = Task('MODE1',target_index=target_index,red_point=red_point)  # 先获取整个返回值
            #time.sleep(0.01)
            # 检查返回值是否有效
            if result is not None and all(item is not None for item in result):
                error_x, error_y, num_detect,new_target_index = result
                target_index=new_target_index
                print(error_x, error_y,corners_reshaped)
                # 转换所有值为字符串
                error_x_str = f"{abs(int(error_x)):03d}"  # 绝对值保留1位小数
                error_y_str = f"{abs(int(error_y)):03d}"
                dir_x_str = str(num_detect[0])
                dir_y_str = str(num_detect[1])
                # 构建发送字符串
                if abs(int(error_x))<1 and abs(int(error_y))<1:
                    usart1.send_data(data='###0000$$$')
                else:
                    data_str = f"#{dir_x_str}{error_x_str}{dir_y_str}{error_y_str}$"
                    print(f"发送数据: {data_str}")
                    usart1.send_data(data=data_str)
                # 调试信息
                if result is None:
                    print("Task函数返回None")
                else:
                    print(f"Task返回部分无效数据: {result}")
        if red_point[0] is not None:
            print(f"Red Laser: ({red_point[0]}, {red_point[1]})")

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # 清理资源
    cap.release()
    cv2.destroyAllWindows()