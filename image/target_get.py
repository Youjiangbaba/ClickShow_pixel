#!/usr/bin/env python
# -*- coding: utf-8 -*-
import cv2
import numpy as np

def get_red(img):
    redImg = img[:,:,2]
    return redImg

def get_green(img):
    greenImg = img[:,:,1]
    return greenImg

def get_blue(img):
    greenImg = img[:,:,0]
    return greenImg

def get_hue(hsv):
    hueImg = hsv[:,:,0]
    return hueImg

def get_sat(hsv):
    satImg = hsv[:,:,1]
    return satImg

def get_value(hsv):                                     #一般用亮度值
    valueImg = hsv[:,:,2]
    return valueImg

def GetU8C1_target_location(img,lower,higher):          #入口参数为单通道图像,提高帧率
    #cv2.imshow("source",img)
    mask = cv2.inRange(img,lowerb = lower,upperb = higher)
    #cv.imshow("result",mask)
    return mask

def separate_color(frame):
    cv2.imshow("原图", frame)
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)                            #色彩空间转换为hsv，便于分离
    lower_hsv = np.array([37, 43, 46])                                      #提取颜色的低值         范围由C++程序找好
    high_hsv = np.array([77, 255, 255])                                     #提取颜色的高值
    mask = cv2.inRange(hsv, lowerb = lower_hsv, upperb = high_hsv)
    cv2.imshow("inRange", mask)

def image_roi(frame,x0,y0,w,h):                                               #(x0,y0)左上角坐标，截取的宽高wh  注意 image[y,x]
    roi = frame[y0:(y0+h),x0:(x0+w)]
    return roi

def print_img_data(frame):
    size = frame.shape                                                        #尺寸
    print size

def print_location(event,x,y,flags,param):                                     #鼠标操作回调函数
    global xi,yi
    if event==cv2.EVENT_LBUTTONDOWN:
        ix, iy = y, x
        img = image                                     #需要操作的图像
        #cv2.circle(img,(x,y),100,(255,255,0),3)
        px=img[ix,iy]
        bx=img[ix,iy,0]
        gx=img[ix,iy,1]
        rx=img[ix,iy,2]
        print(x, y,px)



if __name__ == '__main__':
    kernel_2 = np.ones((2,2),np.uint8)#2x2的卷积核
    kernel_3 = np.ones((3,3),np.uint8)#3x3的卷积核
    kernel_4 = np.ones((4,4),np.uint8)#4x4的卷积核
    cap = cv2.VideoCapture(1)
    ret,image = cap.read()
    print_img_data(image)
    cv2.namedWindow('image')
    cv2.setMouseCallback('image', print_location)
    while(True):

        ret,image = cap.read()

        #cv2.imshow("image", image)
        roi = image_roi(image,480,100,100,100)                          #裁剪，增加帧率
        # 用高斯滤波进行模糊处理，进行处理的原因：每个输入的视频都会因自然震动、光照变化或者摄像头本身等原因而产生噪声。对噪声进行平滑是为了避免在运动和跟踪时将其检测出来。
        #gray_roi = cv2.cvtColor(roi, cv2.COLOR_BGR2GRAY)
        #gray_roi = cv2.GaussianBlur(roi, (21, 21), 0)                   #（5,5）代表高斯核


        cv2.imshow("roi", roi)
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)                    #转为HSV色彩模式
        r = get_red(image)
        maskr = GetU8C1_target_location(r,160,255)
        v = get_value(hsv)
        maskv = GetU8C1_target_location(v,180,255)
        mask_jiao = cv2.bitwise_and(maskr, maskv, dst=None, mask=None)       #取交集

        #下面四行是用卷积进行滤波
        erosion = cv2.erode(mask_jiao,kernel_4,iterations = 1)
        erosion = cv2.erode(erosion,kernel_4,iterations = 1)
        dilation = cv2.dilate(erosion,kernel_4,iterations = 1)
        dilation = cv2.dilate(dilation,kernel_4,iterations = 1)

        contours, hierarchy = cv2.findContours(dilation,cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)   #在dilation中发现轮廓，轮廓按照面积从小到大排列

        for i in contours:                                   #遍历所有的轮廓
            x,y,w,h = cv2.boundingRect(i)                    #将轮廓分解为识别对象的左上角坐标和宽、高 #在图像上画上矩形（图片、左上角坐标、右下角坐标、颜色、线条宽度）
            cv2.rectangle(image,(x,y),(x+w,y+h),(100,200,100),1) #画在原图上

        cv2.imshow("r", maskr)
        cv2.imshow("v", maskv)
        cv2.imshow("image", image)
        cv2.imshow("mask", mask_jiao)
        if cv2.waitKey(1) == 27:                                          #esc quit
            break
# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()
