#!/usr/bin/env python
import numpy as np
import cv2
from autonomy.vision import histogram as hist


'''
enums
FLOODFILL_FIXED_RANGE
If set, the difference between the current pixel and seed pixel is considered. Otherwise, the difference between
neighbor pixels is considered (that is, the range is floating).
FLOODFILL_MASK_ONLY
If set, the function does not change the image ( newVal is ignored), and only fills the mask with the value specified
in bits 8-16 of flags as described above. This option only make sense in function variants that have the mask parameter.

Ideas
- convert to hsv
- only consider hue, hue-sat
- ultimately hue floodfill -> hist   -> backproject OR
                 floodfill -> kmeans -> match
'''

# TODO at this to io
def onmouse(event, x, y, flags, param):
    global seed_pt
    if flags & cv2.EVENT_FLAG_LBUTTON:
        seed_pt = x, y

def nothing(x):
    pass

cv2.namedWindow('floodfill')
cv2.setMouseCallback('floodfill', onmouse)
cv2.createTrackbar('hlo', 'floodfill', 1, 180, nothing)
cv2.createTrackbar('hhi', 'floodfill', 1, 180, nothing)

cv2.createTrackbar('slo', 'floodfill', 1, 255, nothing)
cv2.createTrackbar('shi', 'floodfill', 1, 255, nothing)

cv2.createTrackbar('vlo', 'floodfill', 1, 255, nothing)
cv2.createTrackbar('vhi', 'floodfill', 1, 255, nothing)

fixed_range = True
connectivity = 4
seed_pt = None
cap = cv2.VideoCapture('C:/Users/phahn/Desktop/bodacious.mp4')
pause = False
frame = None
while (cap.isOpened()):
    if not pause:
        rval, frame = cap.read()
    h, w = frame.shape[:2]
    mask = np.zeros((h+2, w+2), np.uint8)

    if seed_pt is None:
        cv2.imshow('floodfill', frame)
    else:
        # TODO only recalc if updated
        flooded = frame.copy()
        mask[:] = 0
        hlo = cv2.getTrackbarPos('hlo', 'floodfill')        # 20/20/40/40 isnt bad for a lot of things.
        hhi = cv2.getTrackbarPos('hhi', 'floodfill')
        slo = cv2.getTrackbarPos('slo', 'floodfill')
        shi = cv2.getTrackbarPos('shi', 'floodfill')
        vlo = cv2.getTrackbarPos('vlo', 'floodfill')        # to disregard a parameter set both lo and hi to 255
        vhi = cv2.getTrackbarPos('vhi', 'floodfill')
        flags = connectivity
        if fixed_range:
            flags |= cv2.FLOODFILL_FIXED_RANGE
        cv2.floodFill(flooded, mask, seed_pt, (255, 255, 255), (hlo,slo,vlo), (hhi,shi,vhi), flags)

        # take histogram of flooded area



        cv2.circle(flooded, seed_pt, 2, (0, 0, 255), -1)
        cv2.imshow('floodfill', flooded)
        cv2.imshow('mask', mask.astype(np.uint8)*255)       # mask retval is bool, apparently...
        # TODO update hist with mouse event

    key = cv2.waitKey(1)

    if key == ord('p'):         # if 'p' is pressed, pause until keypress
        pause = not pause

    if key == 27:  # exit on ESC
        break
cv2.destroyWindow("preview")
