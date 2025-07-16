import cv2
import numpy as np
import time


class Detector:
    def __init__(self, maxperimeter=99999, minperimeter=1):
        # 初始化参数
        self.maxperimeter = maxperimeter
        self.minperimeter = minperimeter

        # 图像处理相关变量
        self.img = None
        self.processed_img = None
        self.target_locked = False  # 目标锁定标志
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

        # 滤波函数
        self.stable_threshold = 2
        self.valid_frame_count = 0
        self.target_roi = None
        self.prev_contour = None

    def Process_img(self, frame, canny_low, canny_high):
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
        img_closed = cv2.morphologyEx(img_canny, cv2.MORPH_CLOSE,
                                      kernel=cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3)), iterations=1)
        self.processed_img = img_canny
        return self.processed_img

    def max_rect_roi_get(self, frame, detect_value):
        # 获取处理后的图像
        if self.processed_img is None:
            return detect_value, 0, 0, 0, 0

        # 如果目标已经锁定，直接返回锁定值
        if self.target_locked:
            return detect_value, self.target_roi[0], self.target_roi[1], self.target_roi[2], self.target_roi[3]

        # 查找轮廓
        contours, _ = cv2.findContours(self.processed_img, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

        # 初始化变量
        valid_contour = None
        found_valid = False

        # 默认坐标值
        x_default = y_default = w_default = h_default = 0
        if self.prev_contour is not None:
            x_default, y_default, w_default, h_default = self.prev_contour

        # 遍历所有轮廓
        x, y, w, h = x_default, y_default, w_default, h_default
        for cnt in contours:
            # 计算轮廓周长
            perimeter = cv2.arcLength(cnt, True)

            # 检查周长是否在有效范围内
            if perimeter < self.minperimeter or perimeter > self.maxperimeter:
                continue

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
                # 第一次达到稳定帧数时锁定目标
                if not self.target_locked:
                    self.target_roi = valid_contour
                    self.target_locked = True  # 标记目标已锁定
                    detect_value = 1
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

        return detect_value, x, y, w, h
    def color_laser_red_detect(self, frame, color_dist):
        """
        找到图片中的红色和绿色激光点并定位中心
        :param frame: 需要处理的图片
        :param color_dist: 色系下限上限表
        :return: 红色激光点中心 (cX2, cY2), 绿色激光点中心 (cX1, cY1), 红色掩膜, 绿色掩膜
        """
        # 对原始图像进行高斯模糊，减少噪声
        blurred = cv2.GaussianBlur(frame, (11, 11), 0)

        # 将 BGR 图像转换为 HSV 色彩空间
        hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

        # 创建红色两个范围的掩膜并合并
        mask_red1 = cv2.inRange(hsv, color_dist['red']['Lower1'], color_dist['red']['Upper1'])
        mask_red2 = cv2.inRange(hsv, color_dist['red']['Lower2'], color_dist['red']['Upper2'])
        mask_red = cv2.bitwise_or(mask_red1, mask_red2)  # 合并两个红色范围的掩膜

        # 创建绿色的掩膜

        # 对掩膜进行一些形态学操作以减少噪声（开闭运算）
        kernel = np.ones((5, 5), np.uint8)
        mask_red_close = cv2.morphologyEx(mask_red, cv2.MORPH_CLOSE, kernel)
        # 寻找红色激光点的轮廓
        contours_red, _ = cv2.findContours(mask_red_close, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        # 寻找绿色激光点的轮廓

        # 初始化激光点位置

        cX2, cY2 = None, None  # 红色激光点

        # 绘制绿色激光点的轮廓并标记中心

        if contours_red:
            try:
                area_red = max(contours_red, key=cv2.contourArea)
                M = cv2.moments(area_red)
                cX2 = int(M["m10"] / M["m00"])
                cY2 = int(M["m01"] / M["m00"])
                cv2.circle(frame, (cX2, cY2), 5, (0, 0, 255), 2)
            except:
                print('无法找到红色激光点')

        return (cX2, cY2), mask_red_close

    def Find_max_countour(self, processed_roi_img, roi,detected_corners):
        """在大矩形中寻找最大轮廓并在roi图像上绘制轮廓与中心点"""
        self.processed_img = processed_roi_img
        if self.processed_img is None:
            return None, None, None, roi

        contours, _ = cv2.findContours(self.processed_img, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        max_contour = None
        max_perimeter = 0
        centroid_x = None
        centroid_y = None
        if not contours:
            return None, None, None, roi
        # 遍历所有轮廓
        for cnt in contours:
            epsilon = 0.02 * cv2.arcLength(cnt, True)
            approx = cv2.approxPolyDP(cnt, epsilon, True)
            area = cv2.contourArea(cnt)
            # 轮廓筛选条件：面积 > 2500 且为四边形
            if area > 2500 and len(approx) == 4:
                # 在roi上绘制蓝色轮廓
                cv2.drawContours(roi, [approx], -1, (255, 0, 0), 1)
                # 提取并排序角点
                corners = approx.reshape(-1, 2)
                assert len(corners) == 4, "应恰好有4个角点"
                center = np.mean(corners, axis=0)
                diff = corners - center
                angles = np.arctan2(diff[:, 1], diff[:, 0])
                sorted_idx = np.argsort(angles)
                sorted_idx = np.roll(sorted_idx, -np.argmin(angles[sorted_idx]))
                sorted_corners = corners[sorted_idx]

                # 按左上->右上->右下->左下排序
                ordered_corners = np.array([
                    sorted_corners[0],  # 左上
                    sorted_corners[1],  # 右上
                    sorted_corners[2],  # 右下
                    sorted_corners[3]  # 左下
                ])

                # 计算当前轮廓的中心点
                current_centroid_x = np.mean(ordered_corners[:, 0])
                current_centroid_y = np.mean(ordered_corners[:, 1])

                # 在roi上绘制红色中心点 (BGR格式: (0,0,255) = 红色)
                cv2.circle(roi,
                           (int(current_centroid_x), int(current_centroid_y)),
                           3, (0, 0, 255), -1)  # 半径3px, 实心圆

                detected_corners.append(ordered_corners)
                # 更新全局中心点（保留最后一个检测到的中心点）
                centroid_x, centroid_y = current_centroid_x, current_centroid_y

        return detected_corners, centroid_x, centroid_y, roi

    def sort_corners_clockwise(self,corners):
        """
        按顺时针顺序排序四边形的角点
        """
        # 计算质心
        center_x = np.mean(corners[:, 0])
        center_y = np.mean(corners[:, 1])
        # 计算每个角点与质心的角度
        angles = np.arctan2(corners[:, 1] - center_y, corners[:, 0] - center_x)
        # 按角度排序
        sorted_indices = np.argsort(angles)
        sorted_corners = corners[sorted_indices]
        return sorted_corners
    def Find_black_rect_contour(self,processed_roi_img,roi,detected_corners):
        roi_gray=cv2.cvtColor(roi, cv2.COLOR_BGR2GRAY)
        kernel_size = (5, 5)
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, kernel_size)
        # 执行开操作（先腐蚀后膨胀）
        procesed_opened_img = cv2.morphologyEx(processed_roi_img, cv2.MORPH_OPEN, kernel)
        contours,_=cv2.findContours(procesed_opened_img,cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        for cnt in contours:
            epsilon = 0.02 * cv2.arcLength(cnt, True)
            approx = cv2.approxPolyDP(cnt, epsilon, True)
            # 计算轮廓面积
            area = cv2.contourArea(cnt)
            if 1000<area<2000 and len(approx)==4:
                cv2.drawContours(roi, [approx], -1, (255, 0, 0), 1)
                # 提取外框的角点reshape操作可以减少一层维度
                corners = approx.reshape(-1, 2)
                detected_corners.append(corners)
                mask = np.zeros_like(roi_gray)
                cv2.drawContours(mask, [cnt], -1, 255, thickness=cv2.FILLED)
                # 使用掩膜提取外框区域
                inner_region = cv2.bitwise_and(roi_gray, roi_gray, mask=mask)
                # 再次使用Canny边缘检测和findContours查找内框
                inner_canny = cv2.Canny(inner_region, 20, 80)
                inner_contours, _ = cv2.findContours(inner_canny, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
                for inner_contour in inner_contours:
                    inner_epsilon = 0.02 * cv2.arcLength(inner_contour, True)
                    inner_approx = cv2.approxPolyDP(inner_contour, inner_epsilon, True)
                    inner_area = cv2.contourArea(inner_contour)
                    # 确保是四边形且面积小于外框
                    if inner_area < area and len(inner_approx) == 4:
                        # 提取内框的角点
                        inner_corners = inner_approx.reshape(-1, 2)
                        detected_corners.append(inner_corners)
                        cv2.drawContours(roi, [inner_contour], -1, (0, 255, 0), 1)
                        # 计算中点框
                        mid_corners = []
                        for i in range(4):
                            mid_x = (corners[i][0] + inner_corners[i][0]) // 2
                            mid_y = (corners[i][1] + inner_corners[i][1]) // 2
                            mid_corners.append([mid_x, mid_y])
                        # 转换为NumPy数组
                        mid_corners = np.array(mid_corners)
                        mid_corners = self.sort_corners_clockwise(mid_corners)
                        print(mid_corners)
                        # 确保中点框的面积在合理范围内
                        mid_area = cv2.contourArea(mid_corners)
                        if 1000 < mid_area:
                            # 绘制中点框
                            cv2.drawContours(roi, [mid_corners], -1, (255, 255, 0), 1)
                            # 在中点框的每条边等分出6个点
                            for i in range(4):
                                start = mid_corners[i]
                                end = mid_corners[(i + 1) % 4]
                                for j in range(1, 20):
                                    x = int(start[0] + (end[0] - start[0]) * j / 20)
                                    y = int(start[1] + (end[1] - start[1]) * j / 20)
                                    cv2.circle(roi, (x, y), 2, (255, 0, 255), -1)

                            # 绘制中点框的角点
                            for mid_corner in mid_corners:
                                x, y = mid_corner
                                cv2.circle(roi, (x, y), 2, (255, 0, 255), -1)
        return roi,mid_corners,mask, inner_region