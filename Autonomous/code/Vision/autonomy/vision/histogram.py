import cv2
import numpy as np

# histogram functionality
# TODO likely want to make this a class where you can define thresholding ops for backproject
# TODO grabcut doesn't belong here
# TODO hist needs to be more generic and support a masked image (for floodfill etc)
# TODO don't need image if you move img_hst to beginning of def
# TODO need to be able to select any/all combo of HSV and threshold values
# todo move slice out
def histogram(img, mask, hsvComponents,hsvRanges,hsvMaxes):
    # calculating object histogram
    img_HSV = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)  # numpy slicing: y0:y1, x0:x1 different than cv
    roihist = cv2.calcHist([img_HSV], hsvComponents, mask, hsvMaxes, hsvRanges)  # only using hue and sat TODO whats the thing between mask and hsvRanges?
    cv2.normalize(roihist, roihist, 0, 255, cv2.NORM_MINMAX)
#    ret, roihist = cv2.threshold(roihist, 64, 256, 0)           # remove weak correlations (128+H, 64+H,S
    return roihist


# histogram backprojection
def backproject(hsvt,roihist, hsvComponents,hsvRanges):
    # backprojection method
    dst = cv2.calcBackProject([hsvt], hsvComponents, roihist, hsvRanges, 1)
    # Now convolute with circular disc
    disc = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))
    cv2.filter2D(dst, -1, disc, dst)
    # threshold and binary AND
#    cv2.imshow("unthresholded backprojection", dst)
#    ret, thresh = cv2.threshold(dst, 128, 256, 0)                 # remove weak correlations
#    cv2.imshow("thresholded backprojection", thresh)
    return cv2.merge((dst, dst, dst))