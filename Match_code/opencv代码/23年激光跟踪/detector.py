import cv2
import numpy as np

class Detector:
    def __init__(self, maxperimeter=99999, minperimeter=1):
        # 初始化参数
        self.maxperimeter = maxperimeter
        self.minperimeter = minperimeter

        # 图像处理相关变量
        self.img = None
        self.processed_img = None

        # 大矩形检测相关变量
        self.top_left = None
        self.top_right = None
        self.bottom_right = None
        self.bottom_left = None
        self.center_big_square = None
        self.angle = None
        self.width = None
        self.height = None
        self.rotation_matrix = None
        self.neijie_radius = 0

        # 小正方形相关变量
        self.sorted_box = None
        self.squares_center = []
        self.square_points = []

        #滤波函数
        self.stable_threshold=20
        self.valid_frame_count = 0
        self.target_roi = None
        self.prev_contour = None
    def Process_img(self, frame,canny_low,canny_high):
        """处理输入图像，返回边缘检测结果"""
        self.img = frame
        if self.img is None:
            return None

        # 转换为灰度图
        gray_img = cv2.cvtColor(self.img, cv2.COLOR_BGR2GRAY)

        # 高斯滤波去噪
        gray_blur_img = cv2.GaussianBlur(gray_img, (3, 3), 0)
        # Canny边缘检测
        img_canny = cv2.Canny(gray_blur_img, canny_low, canny_high)
        ##形态学闭运算，先膨胀后腐蚀 清空内部噪点和外部粘连
        img_closed = cv2.morphologyEx(img_canny, cv2.MORPH_CLOSE,kernel=cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3)), iterations=1)
        self.processed_img = img_canny
        return self.processed_img

    def max_rect_roi_get(self, process_img, frame):
        # 获取处理后的图像
        if self.processed_img is None:
            return 0, 0, 0, 0, 0

        # 查找轮廓
        contours, _ = cv2.findContours(self.processed_img, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

        # 初始化变量
        valid_contour = None
        found_valid = False
        return_value = 0

        # 默认坐标值
        x_default = y_default = w_default = h_default = 0
        if self.target_roi is not None:
            x_default, y_default, w_default, h_default = self.target_roi

        # 遍历所有轮廓
        x, y, w, h = x_default, y_default, w_default, h_default
        for cnt in contours:
            # 获取边界矩形
            x_temp, y_temp, w_temp, h_temp = cv2.boundingRect(cnt)

            # 跳过无效尺寸轮廓
            if w_temp <= 0 or h_temp <= 0:
                continue

            # 检查长宽比
            aspect_ratio = float(w_temp) / h_temp
            min_ratio = 0.97
            max_ratio = 1.03

            if not (min_ratio <= aspect_ratio <= max_ratio):
                continue

            # 计算中心点
            center = (x_temp + w_temp // 2, y_temp + h_temp // 2)

            # 位置稳定性检查
            if self.prev_contour is not None:
                prev_x, prev_y, prev_w, prev_h = self.prev_contour
                prev_center = (prev_x + prev_w // 2, prev_y + prev_h // 2)

                # 计算距离
                distance = np.sqrt((center[0] - prev_center[0]) ** 2 +
                                   (center[1] - prev_center[1]) ** 2)

                # 位置波动过大则跳过
                if distance > 200:
                    continue

            # 找到有效轮廓
            valid_contour = (x_temp, y_temp, w_temp, h_temp)
            found_valid = True
            break

        # 处理有效轮廓
        if found_valid:
            self.prev_contour = valid_contour
            self.valid_frame_count += 1
            x, y, w, h = valid_contour

            # 检查是否达到稳定帧数
            if self.valid_frame_count >= self.stable_threshold:
                self.target_roi = valid_contour
                return_value = 1
        else:
            # 重置计数器
            self.valid_frame_count = 0
            # 使用上一帧位置（如果可用）
            if self.prev_contour is not None:
                x, y, w, h = self.prev_contour

        # 绘图逻辑
        if frame is not None:
            if found_valid:
                cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
                cv2.putText(
                    frame,
                    f"Tracking: {self.valid_frame_count}/{self.stable_threshold}",
                    (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.7,
                    (0, 255, 0),
                    2
                )

            if self.target_roi is not None:
                tx, ty, tw, th = self.target_roi
                cv2.rectangle(frame, (tx, ty), (tx + tw, ty + th), (0, 0, 255), 2)
                cv2.putText(
                    frame,
                    "TARGET LOCKED",
                    (tx, ty - 5),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.7,
                    (0, 0, 255),
                    2
                )

        return return_value, x, y, w, h

    def color_red_detect(self, color_dist,frame):
        """
        找到图片中的绿色激光点与红色激光点并定位中心
        :param img: 需要处理的图片
        :param color_dist: 色系下限上限表
        :return: 绿色激光点中心（x1, y1）;红色激光点中心（x2, y2)
        """
        cX2, cY2 = None, None
        # 灰度图像处理
        blurred = cv2.GaussianBlur(frame, (11, 11), 0)
        kernel = np.ones((1, 1), np.uint8)
        opening = cv2.morphologyEx(blurred, cv2.MORPH_OPEN, kernel)
        thresh = cv2.threshold(opening, 230, 255, cv2.THRESH_BINARY)[1]
        # 转化成HSV图像
        hsv = cv2.cvtColor(thresh, cv2.COLOR_BGR2HSV)
        # 颜色二值化筛选处理
        inRange_hsv_red = cv2.inRange(hsv, color_dist['red']['Lower'], color_dist['red']['Upper'])
        # 找绿色激光
        # 找红色激光点
        try:
            cnts2 = cv2.findContours(inRange_hsv_red.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
            c2 = max(cnts2, key=cv2.contourArea)
            M = cv2.moments(c2)
            cX2 = int(M["m10"] / M["m00"])
            cY2 = int(M["m10"] / M["m00"])
            # 绘制红色激光点的圆形标记
            cv2.circle(frame, (cX2, cY2), 5, (0, 0, 255), 2)
        except:
            print('没有找到红色的激光')
        return cX2, cY2

    def Find_max_countour(self):
        """在大矩形中寻找最大轮廓"""
        if self.processed_img is None:
            return None, None

        # 寻找轮廓
        contours, _ = cv2.findContours(self.processed_img, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        if not contours:
            return None, None

        # 存储符合条件的四边形
        valid_quads = []
        for cnt in contours:
            epsilon = 0.02 * cv2.arcLength(cnt, True)
            approx = cv2.approxPolyDP(cnt, epsilon, True)

            # 检查是否为四边形
            if len(approx) == 4:
                perimeter = cv2.arcLength(approx, True)
                if self.minperimeter <= perimeter <= self.maxperimeter:
                    valid_quads.append((approx, perimeter))

        # 如果没有符合条件的四边形
        if not valid_quads:
            return None, None

        # 找到周长最大的四边形
        best_approx, max_perimeter = max(valid_quads, key=lambda x: x[1])

        # 处理角点
        top_corners = best_approx.reshape(4, 2)

        # 对角点排序: [左上, 右上, 右下, 左下]
        sorted_corners = sorted(top_corners, key=lambda p: p[1])
        top_row = sorted(sorted_corners[:2], key=lambda p: p[0])
        bottom_row = sorted(sorted_corners[2:], key=lambda p: p[0])

        self.sorted_box = np.array([top_row[0], top_row[1], bottom_row[1], bottom_row[0]])

        # 设置大矩形的四个顶点
        self.top_left = tuple(self.sorted_box[0])
        self.top_right = tuple(self.sorted_box[1])
        self.bottom_right = tuple(self.sorted_box[2])
        self.bottom_left = tuple(self.sorted_box[3])

        # 绘制轮廓
        cv2.drawContours(self.img, [self.sorted_box.astype(int)], 0, (0, 255, 0), 2)

        return self.sorted_box, self.img

    def Prepare_center_detection(self, sorted):
        """准备中心点检测所需的各种参数"""
        self.sorted_box = sorted
        if sorted is None:
            return False

        # 计算大矩形的旋转角度
        self.angle = np.arctan2(self.bottom_right[1] - self.top_right[1],
                                self.bottom_right[0] - self.top_right[0])

        # 创建旋转矩阵
        self.rotation_matrix = np.array([
            [np.cos(self.angle), -np.sin(self.angle)],
            [np.sin(self.angle), np.cos(self.angle)]
        ])

        # 计算大矩形的中心点
        self.center_big_square = np.array([
            (self.top_left[0] + self.bottom_right[0]) / 2,
            (self.top_left[1] + self.bottom_right[1]) / 2
        ])

        # 计算旋转矩阵的宽度和高度
        self.width = max(self.distance(self.top_left, self.top_right),
                         self.distance(self.bottom_left, self.bottom_right))

        self.height = max(self.distance(self.top_left, self.bottom_left),
                          self.distance(self.top_right, self.bottom_right))

        # 计算内接圆半径
        min_dimension = min(self.width, self.height)
        self.neijie_radius = min_dimension / 6  # 假设为小正方形边长的1/2

        return True

    def Calculate_square_centers(self):
        """计算每个小正方形的中心点坐标"""
        if self.rotation_matrix is None or self.center_big_square is None:
            return None

        self.squares_center = []

        # 小正方形的顺序定义
        square_order = [
            (0, 0), (0, 1), (0, 2),
            (1, 0), (1, 1), (1, 2),
            (2, 0), (2, 1), (2, 2)
        ]

        # 计算每个小正方形的中心点
        for row, col in square_order:
            # 计算小正方形的中心点相对于大矩形中心点的坐标
            relative_center = np.array([
                (2 * col - 2) * self.width / 6,
                (2 * row - 2) * self.height / 6
            ])

            # 使用旋转矩阵旋转小正方形的中心点
            rotated_center = np.dot(self.rotation_matrix, relative_center)

            # 计算绝对坐标
            absolute_center = self.center_big_square + rotated_center

            # 存储中心点坐标和编号
            self.squares_center.append((absolute_center, row * 3 + col + 1))

            # 绘制内接圆
            cv2.circle(self.img,
                       (int(absolute_center[0]), int(absolute_center[1])),
                       int(self.neijie_radius),
                       (255, 0, 0), 2)

        return self.squares_center

    def Detect_centers(self):
        """完整的中心检测流程"""
        # 处理图像
        processed_img = self.Process_img(self.img)
        if processed_img is None:
            return None

        # 查找轮廓
        sorted_box, _ = self.Find_max_countour()

        # 准备中心检测参数
        if not self.Prepare_center_detection(sorted_box):
            return None

        # 计算中心点
        centers = self.Calculate_square_centers()
        return centers