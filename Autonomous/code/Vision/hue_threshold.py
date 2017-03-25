#!/usr/bin/env python
import cv2
import numpy as np
from matplotlib import pyplot as plt


# hue thresholding using histogram. Bin peaks, plot and see if it identifies color
# may also look at saturation thresholding
# value peaks occur at min/max which makes sense lots of black and white in the video

def huemask(min, max, frame):  # H goes from 0 to 180 note
    lower_range = np.array([min, 16, 0])  # ,32,32 great          48/48
    upper_range = np.array([max, 255, 255])  # ,225,225 great        192/192 works on colored bots
    # 32:255 on sat effectively notches out black insert and white insert, but completely loses black and white bots
    # 225:255 on value picks up doominator red/eclipse white very well but not white/black wedge or broheim (might be artifact of white floor in that video vs. black in others)
    # ultimately may need multiple filters, select best one for the match.
    # Threshold the HSV image to get only blue colors
    mask = cv2.inRange(frame, lower_range, upper_range)
    return mask


# color shift watershed working code of your own
vc = cv2.VideoCapture('C:/Users/phahn/Desktop/jackknife.mp4')

plt.axis([0, 255, 0, 10000])
plt.ion()

if vc.isOpened():  # try to get the first frame
    rval, frame = vc.read()
else:
    rval = False

while rval:
    rval, frame = vc.read()
    #        frame = cv2.resize(frame,None,fx=0.1, fy=0.1, interpolation = cv2.INTER_AREA)
    #        shifted = cv2.pyrMeanShiftFiltering(frame, 21, 51)
    #        cv2.imshow("Input", frame)

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    hue, sat, val = cv2.split(hsv)
    cv2.imshow("Hue", hue)
    cv2.imshow("Sat", sat)
    cv2.imshow("Val", val)

    '''
    TODO try adding good features to track
    TODO try identifying the robot blob then track by color
    '''

    mask1 = huemask(0, 90, hsv)
    mask2 = huemask(90, 180, hsv)

    '''
    lower_range = np.array([90,0,0])
    upper_range = np.array([110,255,255])

    # Threshold the HSV image to get only blue colors
    mask = cv2.inRange(hsv, lower_range, upper_range)
    '''

    # Bitwise-AND mask and original image
    masked_frame = cv2.bitwise_and(frame, frame, mask=(mask1 + mask2))
    cv2.imshow("Input", masked_frame)  # masked_frame)

    '''
        # define range of blue color in HSV
        lower_blue = np.array([110,50,50])
        upper_blue = np.array([130,255,255])

        # Threshold the HSV image to get only blue colors
        mask = cv2.inRange(hsv, lower_blue, upper_blue)

        # Bitwise-AND mask and original image
        res = cv2.bitwise_and(frame,frame, mask= mask)
    '''

    '''
    plt.cla()
    color = ('b','g','r')
    for i,col in enumerate(color):
        histr = cv2.calcHist([frame],[i],None,[256],[0,256])
        plt.plot(histr,color = col)
        plt.xlim([0,256])
    plt.show()
#        plt.pause(0.05)
    '''
    key = cv2.waitKey(1)
    if key == 27:  # exit on ESC
        break
cv2.destroyWindow("preview")
