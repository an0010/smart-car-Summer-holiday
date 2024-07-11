# This work is licensed under the MIT license.
# Copyright (c) 2013-2023 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# AprilTags Example
#
# This example shows the power of the OpenMV Cam to detect April Tags
# on the OpenMV Cam M7. The M4 versions cannot detect April Tags.

import sensor
import image
import time
import math
import pyb,time
from pyb import UART,LED,Pin
import sensor
import time
import math
import image
import display

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
lcd = display.SPIDisplay()
clock = time.clock()

# Note! Unlike find_qrcodes the find_apriltags method does not need lens correction on the image to work.

# The apriltag code supports up to 6 tag families which can be processed at the same time.
# Returned tag objects will have their tag family and id within the tag family.

tag_families = 0
tag_families |= image.TAG16H5  # comment out to disable this family
tag_families |= image.TAG25H7  # comment out to disable this family
tag_families |= image.TAG25H9  # comment out to disable this family
tag_families |= image.TAG36H10  # comment out to disable this family
tag_families |= image.TAG36H11  # comment out to disable this family (default family)
tag_families |= image.ARTOOLKIT  # comment out to disable this family

# What's the difference between tag families? Well, for example, the TAG16H5 family is effectively
# a 4x4 square tag. So, this means it can be seen at a longer distance than a TAG36H11 tag which
# is a 6x6 square tag. However, the lower H value (H5 versus H11) means that the false positive
# rate for the 4x4 tag is much, much, much, higher than the 6x6 tag. So, unless you have a
# reason to use the other tags families just use TAG36H11 which is the default family.

LEDB = LED(3)                  # 蓝色LED

uart = UART(3, 115200)         # 初始化串口3，波特率115200 --》母板上 UART接口

ch = 0
thresholds = [
    (0, 100, -128, 42, -128, 127),
    (21, 79, -56, -14, 33, 77),
]

threshold_list_ball = []
threshold_list_ball.append(thresholds[0])
threshold_list_door = []
threshold_list_door.append(thresholds[1])

ball_cx = 0
gate_cx = 0
AprilTag_cx = 0
AprilTag_distance = 0
ball_dis_flag = 0


def family_name(tag):
    if tag.family() == image.TAG16H5:
        return "TAG16H5"
    if tag.family() == image.TAG25H7:
        return "TAG25H7"
    if tag.family() == image.TAG25H9:
        return "TAG25H9"
    if tag.family() == image.TAG36H10:
        return "TAG36H10"
    if tag.family() == image.TAG36H11:
        return "TAG36H11"
    if tag.family() == image.ARTOOLKIT:
        return "ARTOOLKIT"

def search_gate(img):
    blobs = img.find_blobs(
            threshold_list_door,invert=False ,pixels_threshold=100, area_threshold=100, merge=True)
    if blobs:
        max_blob = blobs[0]
        for blob in blobs:
            if blob.area()>max_blob.area():
                max_blob = blob
        rect_tuple = max_blob.rect()
#        img.draw_rectangle(rect_tuple)
        x = max_blob.cx() #0~165
        # print("rect_x:", x)
        return x
    else:
        print(200)
        return 200

def search_ball(img):
    blobs = img.find_blobs(
        threshold_list_ball,invert=True ,pixels_threshold=10, area_threshold=10, merge=True)
    if blobs:
        max_blob = blobs[0]
        for blob in blobs:
            print("blob.roundness:",blob.roundness())
            print("blob.area():",blob.area())
            if (blob.roundness()>max_blob.roundness() and blob.roundness() > 0.60) or (blob.area()>max_blob.area() and blob.area()>15000):
                max_blob = blob

        # case 1: ball is far and round
        if max_blob.roundness()>0.60:
            circle_tuple = max_blob.enclosing_circle()
#            img.draw_circle(circle_tuple)
            x = circle_tuple[0] #0~165
            # print("circ_roundness", max_blob.roundness())
            # print("circ_area", max_blob.area())
#            print("circ_x:", x)

            #whether the ball is too close
            if max_blob.area()>2000:
                ball_dis_flag = 2
            else :
                ball_dis_flag = 1
            return x, ball_dis_flag

        # case 2: ball is near so it is not round
        elif max_blob.area()>4000:
            circle_tuple = max_blob.enclosing_circle()
#            img.draw_circle(circle_tuple)
            x = circle_tuple[0] #0~165
#            print("circ_roundness", max_blob.roundness())
#            print("circ_area", max_blob.area())
#            print("circ_x:", x)

            #whether the ball is too close
            if max_blob.area()>2000:
                ball_dis_flag = 2
            else :
                ball_dis_flag = 1
            return x, ball_dis_flag
        # case 3: there is no ball but there is a blob
        else:
            # print(200)
            return 200, 0

    # case 4: there is no ball and no blob
    else:
        # print(200)
        return 200, 0

def search_AprilTag(img):
    for tag in img.find_apriltags(
        families=tag_families
    ):  # defaults to TAG36H11 without "families".
        img.draw_rectangle(tag.rect(), color=(255, 0, 0))
        img.draw_cross(tag.cx(), tag.cy(), color=(0, 255, 0))
        print("cx:", tag.cx())
        print("z:", tag.z_translation())
        return tag.cx(), tag.z_translation()



if __name__ == "__main__":
    while True:
        clock.tick()
        img = sensor.snapshot()

        Apriltag_output = search_AprilTag(img)
        if Apriltag_output:
            AprilTag_cx = Apriltag_output[0]
            AprilTag_distance = Apriltag_output[1]
        ball_output = search_ball(img)
        if ball_output:
            ball_x = ball_output[0]
            ball_dis_flag = ball_output[1]
        gate_x = search_gate(img)

        uart.writechar(255) # start byte
        uart.writechar(253)
        uart.writechar(ball_x) # 100-200: ball position, 100-150: ball is far, 150-200: ball is near, 200: no ball
        print("ball_cx:", ball_x)
        uart.writechar(gate_x) # 0-100: gate position, 100: no gate
        print("gate_cx:", gate_x)
        uart.writechar(ball_dis_flag) # 0: ball is far, 1: ball is near, 3: no ball
        print("dis_flag:",ball_dis_flag)
        uart.writechar(254)
