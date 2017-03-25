#!/usr/bin/python

# from https://www.learnopencv.com/blob-detection-using-opencv-python-c/
# website has verbose info on customizing blob detection
# not working on my test videos

# !/usr/bin/env python
import cv2
import numpy as np
from matplotlib import pyplot as plt

# Setup SimpleBlobDetector parameters.
params = cv2.SimpleBlobDetector_Params()

params.filterByColor = True

# Change thresholds
params.minThreshold = 10
params.maxThreshold = 200

# Filter by Area.
params.filterByArea = True
params.minArea = 500

# Filter by Circularity
# params.filterByCircularity = True
# params.minCircularity = 0.1

# Filter by Convexity
# params.filterByConvexity = True
# params.minConvexity = 0.87

# Filter by Inertia
params.filterByInertia = True
params.minInertiaRatio = 0.01

# Create a detector with the parameters
detector = cv2.SimpleBlobDetector_create(params)

# color shift watershed working code of your own
vc = cv2.VideoCapture('C:/Users/phahn/Desktop/bodacious.mp4')  # can find Broheim on white

if vc.isOpened():  # try to get the first frame
    rval, frame = vc.read()
else:
    rval = False

while rval:
    rval, frame = vc.read()
    gray = frame  # cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Detect blobs.
    keypoints = detector.detect(gray)

    print len(keypoints)

    # Draw detected blobs as red circles.
    # cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS ensures
    # the size of the circle corresponds to the size of blob

    im_with_keypoints = cv2.drawKeypoints(gray, keypoints, np.array([]), (0, 0, 255),
                                          cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

    # Show blobs
    cv2.imshow("Keypoints", im_with_keypoints)

    key = cv2.waitKey(1)
    if key == 27:  # exit on ESC
        break
cv2.destroyWindow("preview")
