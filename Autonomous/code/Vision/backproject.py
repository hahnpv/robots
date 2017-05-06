import time

import cv2
import numpy as np

from autonomy.util import io
from autonomy.vision import histogram as hist

# CONFIGURATION
useCamshift = True
hsvComponents = [0]         # [0], [0, 1]
hsvMaxes      = [180]       # [180], [180, 255] TODO check this param and see what its for seems redundant
hsvRanges     = [0, 180]    # [0, 180], [0, 180, 0, 255]
# Camshift termination criteria, either 10 iteration or move by atleast 1 pt
term_crit = (cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 1)

# crib off of hist_finder for program structure
# http://opencv-python-tutroals.readthedocs.io/en/latest/py_tutorials/py_imgproc/py_histograms/py_histogram_backprojection/py_histogram_backprojection.html#histogram-backprojection
# TODO try doing kmeans? small area might compute quick enough for pre-fight check? otherwise still useful offline

cv2.namedWindow("image")
cv2.setMouseCallback("image", io.rect_grab)

my_camera = cv2.VideoCapture('C:/Users/phahn/Desktop/rumble.mp4')
time.sleep(2)
success, image = my_camera.read()
refresh = True
haveHistogram = False
roihist = None
refRect = io.refRect

while (True):
    if refresh:
        success, image = my_camera.read()

    img = image.copy()

    if refRect != io.refRect and not io.mousing:       # new rectangle, new histogram, if we are done mousing
        refRect = io.refRect                           # don't need copy, need to discern if rectangle has changed
        print "calculating histogram"
        # TODO my_grabcut_hist_wrapper()
        # this is grabcut
        mask = np.zeros(img.shape[:2], np.uint8)
        bgdModel = np.zeros((1, 65), np.float64)
        fgdModel = np.zeros((1, 65), np.float64)
        cv2.grabCut(img, mask, refRect, bgdModel, fgdModel, 5, cv2.GC_INIT_WITH_RECT)
        mask2 = np.where((mask == 2) | (mask == 0), 0, 1).astype('uint8')
        # TODO spinner blur grabs more background than I'd like - in that case you need to erode before histogram
        # TODO add option to take entire rect for histogram (key toggle grabcut off/on)
        # hue+sat better than just hue. hue+sat+val occluding everything. not sure if bug
        gcimg = img * mask2[:, :, np.newaxis]
        print img.shape[:2]
        cv2.imshow('grabcut', gcimg)
        mask3 = np.where((mask2==1), 255,0).astype('uint8')

        cv2.imshow('mask', mask3)
        roihist = hist.histogram(img,mask3,hsvComponents,hsvRanges,hsvMaxes)

        # TODO your roihist is being treated like a binary mask below shouldnt it be a gradient?
        # todo try plotting in numpy to visualize better as a colormap

        # Eroding and dilating the histogram gets rid of a lot of the 'noise'
        # TODO maybe apply a smoothing kernel then threshold instead? spread signal around then remove weak signal
        # TODO this makes everything no worse or better
        roihist = cv2.erode(roihist, np.ones((2,2)))
        roihist = cv2.dilate(roihist, np.ones((8,8)))

        cv2.imshow("Histogram",roihist)      # dilate to make it a bit more readable
        print np.max(roihist)
        haveHistogram = True
        track_window = refRect

    if haveHistogram:
        thresh = hist.backproject(cv2.cvtColor(img,cv2.COLOR_BGR2HSV),roihist,hsvComponents,hsvRanges)

        '''
        TODO
        When tracking, calculate a back projection of a hue plane of each input video frame using that pre-computed
        histogram. Threshold the back projection to suppress weak colors. It may also make sense to suppress pixels
        with non-sufficient color saturation and too dark or too bright pixels.
        '''

        if useCamshift:
            '''
            Need to fix hist.backproject (returning 3 components always -> may be fucking up the else as well
            Works with bright features but can get 'lost'. Can assess 'success' to verify.
            '''
            thresh = cv2.calcBackProject([cv2.cvtColor(img,cv2.COLOR_BGR2HSV)], hsvComponents, roihist, hsvRanges, 1)
            success, track_window = cv2.CamShift(thresh, track_window, term_crit)
            # Draw it on image
            pts = cv2.boxPoints(success)
            print success
            pts = np.int0(pts)
            img2 = cv2.polylines(img, [pts], True, 255, 2)
        else:
            ret,thresh = cv2.threshold(thresh,50,255,0)
            thresh = cv2.merge((thresh,thresh,thresh))

                # H S V
            res = cv2.bitwise_and(img, thresh)
            lower_range = np.array([0, 128,0])           # get rid of shiny things
            upper_range = np.array([180, 256, 256])    # TODO make this a parameter and show red square on histogram to warn
            hsv_res = cv2.cvtColor(res, cv2.COLOR_BGR2HSV)
            mask = cv2.inRange(hsv_res, lower_range, upper_range)
    #        mask = cv2.dilate(mask, np.ones((20,20)))
            res = cv2.bitwise_and(res, res, mask=mask)
    #        res = cv2.morphologyEx(res, cv2.MORPH_CLOSE, np.ones((5,5)))
            cv2.imshow('backproject_val_threshold', res)

    if io.mousing:
        cv2.rectangle(img, io.refPt, io.refEnd, (0, 255, 0), 2)

    cv2.imshow('image', img)

    # Esc key to stop, otherwise repeat after 20 milliseconds
    key_pressed = cv2.waitKey(20)

    if key_pressed == ord('p'):         # if 'p' is pressed, pause until keypress
        refresh = not refresh

    if key_pressed == 27 or key_pressed == ord('q'):
        break

cv2.destroyAllWindows()
my_camera.release()