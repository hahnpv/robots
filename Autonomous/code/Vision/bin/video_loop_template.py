#!/usr/bin/env python
import cv2

# video template
cap = cv2.VideoCapture('C:/Users/phahn/Desktop/bodacious.mp4')
while (cap.isOpened()):
    rval, frame = cap.read()

    cv2.imshow("output", frame)

    key = cv2.waitKey(1)

    if key == ord('p'):         # if 'p' is pressed, pause until keypress
        key = cv2.waitKey(0)

    if key == 27:  # exit on ESC
        break
cv2.destroyWindow("preview")
