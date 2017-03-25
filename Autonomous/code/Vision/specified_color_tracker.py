from collections import deque
import numpy as np
import argparse
import imutils
import cv2

# Tracks a specified color (hue) within a specified sat/val range
# idea is that a acqusition code could determine hue, then track that hue
# TODO make color input flexible/exposed
# TODO implement multiple hues for two dominant color bots
# TODO if thing moves offscreen indicate which direction
# TODO merge blobs (ie: doominator you get left or right side, merge 'em all)
# base code from http://www.pyimagesearch.com/2015/09/14/ball-tracking-with-opencv/

# define the lower and upper boundaries of the "green"
# ball in the HSV color space, then initialize the
# list of tracked points

# BROHEIM GREEN
greenLower = (50, 60, 60)
greenUpper = (70, 140, 140)

# LICH KING black
#greenLower = (0, 40, 0)
#greenUpper = (25, 160, 40)
# LICH KING orange doesnt work because cabinets
#greenLower = (0, 115, 115)
#greenUpper = (10, 150, 150)



# DOOMINATOR RED, robot roundabout
#greenLower = (170, 60, 250)
#greenUpper = (180, 180, 256)
# kitchen
#greenLower = (170, 64, 64)
#greenUpper = (180, 255, 255)

# ECLIPSE is challenging
# JACKKNIFE
#greenLower = (40, 50, 125)
#greenUpper = (65, 120, 256)

# BODACIOUS orange
#greenLower = (0, 64, 254)
#greenUpper = (10, 256, 256)

#greenLower = (0, 0, 254)
#greenUpper = (1, 1, 256)


buffer = 64             # contrail buffer
pts = deque(maxlen=buffer)
#cap = cv2.VideoCapture('C:/Users/phahn/Desktop/ball-tracking/ball_tracking_example.mp4')
cap = cv2.VideoCapture('C:/Users/phahn/Desktop/broheim.mp4')    # WORKS!
w = int(cap.get(3)) * 2
h = int(cap.get(4))
write = cv2.VideoWriter('fpv.avi', cv2.VideoWriter_fourcc(*'MJPG'), 30, (w, h))

# keep looping
while cap.isOpened():
    # grab the current frame
    rval, frame = cap.read()

    # resize the frame, blur it, and convert it to the HSV
    # color space
    # frame = imutils.resize(frame, width=600)
    # blurred = cv2.GaussianBlur(frame, (11, 11), 0)
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # construct a mask for the color then perform a series of dilations and erosions to remove any small blobs left in the mask
    mask = cv2.inRange(hsv, greenLower, greenUpper)
    mask = cv2.erode(mask,  np.ones((2, 2)))
    mask = cv2.dilate(mask,  np.ones((5, 5)))

    #
    masked_frame = cv2.bitwise_and(frame, frame, mask=mask)

    # find contours in the mask and initialize the current (x, y) center of the ball
    cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)[-2]
    center = None

    # only proceed if at least one contour was found
    # TODO maybe if they are close combine them
    if len(cnts) > 0:
        # find the largest contour in the mask, then use
        # it to compute the minimum enclosing circle and
        # centroid
        c = max(cnts, key=cv2.contourArea)
        (x, y), radius = cv2.minEnclosingCircle(c)                      # circle is plotted at centre
        M = cv2.moments(c)
        center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))   # movement trail plotted at centroid

        # only proceed if the radius meets a minimum size
        # TODO hull instead of circle
        if radius > 10:
            # draw the circle and centroid on the frame,
            # then update the list of tracked points
            cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 255), 2)
            cv2.circle(frame, center, 5, (0, 0, 255), -1)
        else:
            print "radius too small"
        for cnt in cnts:
            (x, y), radius = cv2.minEnclosingCircle(cnt)                      # circle is plotted at centre
            cv2.circle(masked_frame, (int(x), int(y)), int(radius), (0, 255, 255), 2)
    # update the points queue
    pts.appendleft(center)

    # loop over the set of tracked points
    for i in xrange(1, len(pts)):
        # if either of the tracked points are None, ignore
        # them
        if pts[i - 1] is None or pts[i] is None:
            continue

        # otherwise, compute the thickness of the line and
        # draw the connecting lines
        thickness = int(np.sqrt(buffer / float(i + 1)) * 2.5)
        cv2.line(frame, pts[i - 1], pts[i], (0, 0, 255), thickness)

#    cv2.imshow("Frame", frame)
#    cv2.imshow("Mask", masked_frame)
    display = np.concatenate((frame, masked_frame),axis=1)
    cv2.imshow("test", np.concatenate((frame, masked_frame),axis=1))
    key = cv2.waitKey(1) & 0xFF
    write.write(display)

    if key == ord('p'):         # if 'p' is pressed, pause until keypress
        key = cv2.waitKey(0)

    if key == ord("q"):         # if the 'q' key is pressed, stop the loop
        break

# cleanup the camera and close any open windows
cap.release()
cv2.destroyAllWindows()
write.release()