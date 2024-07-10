import pyb,time
from pyb import UART,LED,Pin
import sensor
import time
import math
import image
import display

LEDB = LED(3)                  # 蓝色LED

uart = UART(3, 115200)         # 初始化串口3，波特率115200 --》母板上 UART接口

ch = 0
thresholds = [
    (0, 100, -128, 14, -128, 127),
    (21, 79, -56, -14, 33, 77),
]

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking
lcd = display.SPIDisplay()
clock = time.clock()

threshold_list_ball = []
threshold_list_ball.append(thresholds[0])
threshold_list_door = []
threshold_list_door.append(thresholds[1])

ball_output = 0
gate_output = 0

def adjust_brightness(img, factor):
    """
    调整图像亮度
    :param img: 输入图像
    :param factor: 亮度调整因子，>1 增加亮度，<1 降低亮度
    :return: 亮度调整后的图像
    """
    img = img.to_rgb565()  # 确保图像格式为 RGB565
    for x in range(img.width()):
        for y in range(img.height()):
            r, g, b = img.get_pixel(x, y)
            r = min(int(r * factor), 255)
            g = min(int(g * factor), 255)
            b = min(int(b * factor), 255)
            img.set_pixel(x, y, (r, g, b))
    return img

brightness_factor = 0.2  # 亮度调整因子，可以根据需要调整

while True:
    clock.tick()
    img = sensor.snapshot()

    blobs = img.find_blobs(
        threshold_list_door,invert=False ,pixels_threshold=100, area_threshold=100, merge=True)
    if blobs:
        max_blob = blobs[0]
        for blob in blobs:
            if blob.area()>max_blob.area():
                max_blob = blob
            rect_tuple = max_blob.rect()
#            img.draw_rectangle(rect_tuple)
            x = max_blob.cx()
            x = int(x/(400/100))
            print("rect_x:", x)
            uart.writechar(x)
    else:
        print(100)
        uart.writechar(100)

    blobs = img.find_blobs(
    threshold_list_ball,invert=True ,pixels_threshold=100, area_threshold=100, merge=True)
    if blobs:
        max_blob = blobs[0]
        for blob in blobs:
            print("blob.roundness:",blob.roundness())
            print("blob.area():",blob.area())
            if blob.roundness()>max_blob.roundness() and blob.roundness()>0.60:
                max_blob = blob
        if max_blob.roundness()>0.60:
            circle_tuple = max_blob.enclosing_circle()
#            img.draw_circle(circle_tuple)
            x = int(circle_tuple[0]/(400/50) + 100)
#            print("circ_roundness", max_blob.roundness())
#            print("circ_area", max_blob.area())
#            print("circ_x:", x)
            if max_blob.area()>7000:
#                uart.writechar(201)
#                ball_output = 201
                x = x+50
            ball_output = x
            uart.writechar(x)

        elif max_blob.roundness()>0.1 and max_blob.area()>1000:
            circle_tuple = max_blob.enclosing_circle()
#            img.draw_circle(circle_tuple)
            x = int(circle_tuple[0]/(400/100) + 100)
#            print("circ_roundness", max_blob.roundness())
#            print("circ_area", max_blob.area())
#            print("circ_x:", x)
            if max_blob.area()>7000:
#                uart.writechar(201)
#                ball_output = 201
                x = x+50
            uart.writechar(x)

        else:
            print(200)
            uart.writechar(200)
    else:
        print(200)
        uart.writechar(200)

