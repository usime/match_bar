import cv2
from maix import image, display, app, time, camera

disp = display.Display()
cam = camera.Camera(320, 240, image.Format.FMT_BGR888)

while not app.need_exit():
    img=cam.read()
    img_cv=image.image2cv(img, ensure_bgr=False, copy=False)
    img_cv_processd=Process_img(frame=img_cv,canny_low=50,canny_high=180)
   
    img_show = image.cv2image(img_cv_processd, bgr=True, copy=False)
    disp.show(img_show)

