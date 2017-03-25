import cv2
import numpy as np
import copy

vc = cv2.VideoCapture('C:/Users/phahn/Desktop/bodacious.mp4')

while vc.isOpened():
    rval, frame = vc.read()
    img = cv2.medianBlur(frame, 5)
    imgg = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
    hull_img = cv2.cvtColor(imgg, cv2.COLOR_GRAY2BGR)
    contour_img = copy.deepcopy(img)  # copy for contours
    detected_img = np.zeros(frame.shape, np.uint8)  # copy.deepcopy(img) # copy for lines

    # canny contours
    edges = cv2.Canny(imgg, 100, 200, apertureSize=3)  # higher numbers seem to recover more complex curves and less lines (300,500) was (100,200)

    # dilate to merge features together
#    edges = cv2.dilate(edges, np.ones((2, 2)))

    cv2.imshow("first_canny", edges)  # more selective than threshold, useful
    '''
    # tried using corner detector but it detected a lot of useful features
    # detect and remove lines
    # this kind of works, at least it chops up long persistent lines but why doesnt it remove them entirely?
    minLineLength = 50
    maxLineGap = 25
    lines = cv2.HoughLinesP(edges, 1, np.pi / 180, 10, minLineLength, maxLineGap)
    for x in range(0, len(lines)):
        for x1, y1, x2, y2 in lines[x]:
            cv2.line(edges, (x1, y1), (x2, y2), (0, 0, 0), 5)

    cv2.imshow("canny_line_removed", edges)  # more selective than threshold, useful
    '''
    # TODO try using aproxPolyDP to break up long lines which intersect and form triangles http://docs.opencv.org/3.2.0/d3/dc0/group__imgproc__shape.html#ga0012a5fdaea70b8a9970165d98722b4c

    # TODO try filtering out closed v. non-closed curves? Might filter out stuff we want but should filter out lines.

    # still line garbage but we can't erode lest we erode our signal as well!

    # dilate to merge remaining features together
    edges = cv2.dilate(edges, np.ones((10, 10)))

    im2, contours, hierarchy = cv2.findContours(edges, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)  # canny
    cv2.drawContours(contour_img, contours, -1, (128, 0, 128), 3)

    # do stuff with regions
    hulls = []

    culled = 0
    compact = 0
    saved = 0
    for cnt, hier in zip(contours, hierarchy[0]):
        if hier[3] == -1:  # we don't care about the children
            hull = cv2.convexHull(cnt)

            if cv2.arcLength(hull, True) < 0.00001:         # infintessimally small hulls
                culled += 1
                continue

            if cv2.contourArea(hull) > 0.05*(img.shape[0]*img.shape[1]):     # cull large areas (for now)
                culled += 1
                continue

            compactness = cv2.contourArea(hull) / (cv2.arcLength(hull, True) * cv2.arcLength(hull, True))
            (x, y), (w, h), ang = cv2.minAreaRect(cnt)
            if h > 0.001:
                aspectRatio = float(w)/h
                if aspectRatio > 5 or aspectRatio < 0.2:                       # thin lines
                    print "culled high AR"
                    culled += 1
                    continue


            cv2.drawContours(hull_img, [hull], -1, (128, 0, 128), 3)  # hulls over areas
            hulls.append(hull)
            mask = np.zeros(imgg.shape, np.uint8)  # make a mask
            cv2.drawContours(mask, [hull], 0, 255, -1)
            mean_val = cv2.mean(frame, mask=mask)  # get average color within mask
            # min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(imgray,mask = mask) # get min and max for bounding
            # TODO could do k-val or hist on HSV inside the hull, detect primary 1-2 colors then work off color detection
            if compactness < 0.02:  # this doesn't do anything with the dilation
                compact += 1
                mean_val = (255, 0, 0)  # low compactness = lines = arena not bot (generally)
            else:
                saved += 1
            cv2.drawContours(detected_img, [hull], 0, mean_val, -1)

    print str(culled) + "\t" + str(compact) + "\t" + str(saved)

    cv2.imshow("canny", edges)  # more selective than threshold, useful
    cv2.imshow("find contours", contour_img)  # more selective than threshold, useful
    cv2.imshow("hull contours", hull_img)  # yummy
    cv2.imshow("avg colors over hull", detected_img)  # not bad!

    key = cv2.waitKey(1)
    if key == 27:  # exit on ESC
        break
cv2.destroyWindow("preview")
