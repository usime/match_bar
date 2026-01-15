import cv2
from maix import image, display, app, time, camera

def Process_img(frame, canny_low, canny_high):
    if frame is None:
        return None
    gray_img = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # 高斯滤波去噪
    gray_blur_img = cv2.GaussianBlur(gray_img, (5, 5), 0)
    # Canny边缘检测
    img_canny = cv2.Canny(gray_blur_img, canny_low, canny_high)
    ##形态学闭运算，先膨胀后腐蚀 清空内部噪点和外部粘连
    img_closed = cv2.morphologyEx(img_canny, cv2.MORPH_CLOSE,
                                  kernel=cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3)), iterations=1)

    return img_closed