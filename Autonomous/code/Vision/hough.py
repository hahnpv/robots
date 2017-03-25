import cv2
import numpy as np
import copy

# bitbanged the following two together
# http://stackoverflow.com/questions/22814609/cv2-hough-circles-error-on-video
# http://stackoverflow.com/questions/33541551/hough-lines-in-opencv-python
# and added getContours from the tutorial. 3 separate methods in this script

vc = cv2.VideoCapture('C:/Users/phahn/Desktop/broheim.mp4')

if vc.isOpened():  # try to get the first frame
    rval, frame = vc.read()
else:
    rval = False

while rval:
    rval, frame = vc.read()
    img = cv2.medianBlur(frame, 5)
    imgg = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
    cimg = cv2.cvtColor(imgg, cv2.COLOR_GRAY2BGR)
    rimg = copy.deepcopy(cimg)  # copy for contours
    limg = copy.deepcopy(cimg)  # copy for lines
    timg = copy.deepcopy(cimg)  # copy for lines

    # Hough Circles - mostly useless escept side view of wheels
    circles = cv2.HoughCircles(imgg, cv2.HOUGH_GRADIENT, 1, 20, param1=100, param2=30, minRadius=0, maxRadius=120)
    if circles is not None:
        for i in circles[0, :]:
            cv2.circle(cimg, (i[0], i[1]), i[2], (0, 255, 0), 1)  # draw the outer circle
            cv2.circle(cimg, (i[0], i[1]), 2, (0, 0, 255), 3)  # draw the center of the circle

    # Hough Lines from Canny detector
    edges = cv2.Canny(imgg, 100, 200, apertureSize=3)
    minLineLength = 30
    maxLineGap = 10
    lines = cv2.HoughLinesP(edges, 1, np.pi / 180, 15, minLineLength, maxLineGap)
    for x in range(0, len(lines)):
        for x1, y1, x2, y2 in lines[x]:
            cv2.line(limg, (x1, y1), (x2, y2), (0, 255, 0), 2)

    # threshold contours
    ret, thresh = cv2.threshold(imgg, 127, 255, 0)  # TODO use hue/intensity/etc
    im2, contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)  # threshold
    cv2.drawContours(timg, contours, -1, (128, 0, 128), 3)

    # canny contours
    im2, contours, hierarchy = cv2.findContours(edges, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)  # canny
    cv2.drawContours(rimg, contours, -1, (128, 0, 128), 3)

    cv2.imshow("circles", cimg)  # mostly useless for bot ID except wedge wheels from side
    cv2.imshow("threshold contours", timg)  # noisy
    cv2.imshow("canny contours", rimg)  # more selective than threshold, useful
    cv2.imshow("lines", limg)  # only good to detect walls, random bot hits/misses

    key = cv2.waitKey(20)
    if key == 27:  # exit on ESC
        break
cv2.destroyWindow("preview")
