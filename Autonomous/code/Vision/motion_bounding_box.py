import argparse
import time
import cv2
import numpy as np
# http://stackoverflow.com/questions/42876998/bounding-box-around-moving-objects

camera = cv2.VideoCapture('C:/Users/phahn/Desktop/broheim.mp4')
time.sleep(0.25)
# firstFrame = None
lastFrame = None
gray = None
while True:
    (grabbed, frame) = camera.read()

    if not grabbed:
        break
    lastFrame = gray
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (21, 21), 0)

    if lastFrame is None:
        lastFrame = gray

    frameDelta = cv2.absdiff(lastFrame, gray)
    # cv2.imshow('Delta', frameDelta)

    thresh = cv2.threshold(frameDelta, 50, 255, cv2.THRESH_BINARY)[1]

    # dilate the thresholded image to fill in holes, then find contours
    # on thresholded image
    thresh = cv2.dilate(thresh, np.ones((4,4)))
    cnts = cv2.findContours(thresh, cv2.RETR_EXTERNAL,
                            cv2.CHAIN_APPROX_SIMPLE)[1]  # using.copy() is unnecessary

    for c in cnts:
        # compute the bounding box for the contour, draw it on the frame
        (x, y, w, h) = cv2.boundingRect(c)
        # print(w,h)
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)

    cv2.namedWindow("Original", cv2.WINDOW_NORMAL)
    cv2.resizeWindow("Original", 747, 420)
    cv2.moveWindow("Original", 100, 100)

    cv2.namedWindow("Thresholded", cv2.WINDOW_NORMAL)
    cv2.resizeWindow("Thresholded", 747, 420)
    cv2.moveWindow("Thresholded", 950, 100)

    cv2.imshow("Original", frame)
    cv2.imshow("Thresholded", frameDelta)
    key = cv2.waitKey(1) & 0xff
    if key == 27:
        break

camera.release()
cv2.destroyAllWindows()
