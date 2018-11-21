#!/usr/bin/env python
# -*- coding: utf-8 -*-
import cv2
import numpy as np









if __name__ == '__main__':

    print "输入图片："
    file = raw_input("raw_input: ")
    image = cv2.imread(file)
    cv2.namedWindow('window')
    cv2.setMouseCallback('window', print_location)
