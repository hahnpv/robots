#!/usr/bin/env python
import cv2
import numpy as np

# color shift watershed working code of your own
cap = cv2.VideoCapture('C:/Users/phahn/Desktop/bodacious.mp4')
while (cap.isOpened()):
    rval, frame = cap.read()

    # try to watershed - erode to just reveal a bot.
    # track on that if fast/robust otherwise use as filter to get bot color histogram and track color


    # gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    hue, sat, val = cv2.split(hsv)

    # val and gray work about evenly well out of the box
    ret, thresh = cv2.threshold(val, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)

    thresh = cv2.bitwise_not(thresh)

    cv2.imshow("frame", frame)
    cv2.imshow("threshold", thresh)

    # noise removal
    kernel = np.ones((3, 3), np.uint8)
    opening = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel, iterations=2)

    cv2.imshow("opening", opening)

    # sure background area
    sure_bg = cv2.dilate(opening, kernel, iterations=3)

    # Finding sure foreground area
    dist_transform = cv2.distanceTransform(opening, cv2.DIST_L2, 5)
    ret, sure_fg = cv2.threshold(dist_transform, 0.25 * dist_transform.max(), 255, 0)

    # Finding unknown region
    sure_fg = np.uint8(sure_fg)
    unknown = cv2.subtract(sure_bg, sure_fg)

    cv2.imshow("dist xform", dist_transform)
    cv2.imshow("sure background", sure_bg)
    cv2.imshow("sure foreground", sure_fg)
    cv2.imshow("unknown", unknown)

    # Marker labelling
    ret, markers = cv2.connectedComponents(opening)  # was sure_fg but opening looks better

    # Add one to all labels so that sure background is not 0, but 1
    markers = markers + 1

    # Now, mark the region of unknown with zero
    markers[unknown == 255] = 0  # apparently this is key

    markers = cv2.watershed(frame, markers)
    frame[markers == -1] = [255, 0, 0]

    cv2.imshow("watershed", frame)

    key = cv2.waitKey(1)
    if key == 27:  # exit on ESC
        break
cv2.destroyWindow("preview")
