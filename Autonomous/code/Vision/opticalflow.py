import cv2
import numpy as np

from autonomy.util import io

# http://pastebin.com/X1dpwT9q
# Improved version of the example code which comes with OpenCV. This version will look for new features if none exist
# this code will crash if it loses features and can find no new features.
# presumably feature_params could be tuned to be robot-finding


# useful commments
'''
http://stackoverflow.com/questions/10159236/feature-tracking-using-optical-flow/10172247#10172247
There is another good way to add new features to the existing ones. You can pass a mask into cv::goodFeaturesToTrack().
So you would create a new Mat (same size as original image, type: CV_8UC1), set all pixels to 255 and draw each feature
point as a black circle into this Mat. When you pass this mask into goodFeaturesToTrack() those black circles will be
skipped by the function.

I would also recommend limiting the amount of features. Let's say you limit it to MAX_FEATURES = 300. You then check
every cycle whether you have less tracks than MAX_FEATURES - z (e.g. z = 30). In case you do, search for up to z new
features as stated above and add them to your feature-container.

Also note that you have to actively delete Features when tracking failed. You will therefore have to look at the status
output of calcOpticalFlowPyrLK
'''

# TODO: need provisions to add points as points fall off
# TODO: need provisions to drop points that aren't following bot (ie: we lost them / tagged something wrong)
# TODO: robot rotations seem to lose points as features appear/disappear. Likely need to actively curate points each frame

cap = cv2.VideoCapture('C:/Users/phahn/Desktop/bodacious.mp4')

cv2.namedWindow("optical flow")
cv2.setMouseCallback("optical flow", io.rect_grab)

if not cap.isOpened():
    CV_ASSERT("Cam open failed")
# params for ShiTomasi corner detection
feature_params = dict(maxCorners=10,
                      qualityLevel=0.1,
                      minDistance=7,
                      blockSize=7)
# Parameters for lucas kanade optical flow
lk_params = dict(winSize=(15, 15),
                 maxLevel=2,
                 criteria=(cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 0.03))

# Create some random colors
color = np.random.randint(0, 255, (100, 3))

refresh = True
frame = None
img = None
haveSelection = False
haveFlow = False
refRect = (0,0,0,0)         # purely to detect box selection TODO make a different indicator
mask = None
while (1):
    if refresh:
        ret, frame = cap.read()
        frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    if refRect != io.refRect and not io.mousing:       # new rectangle, new histogram, if we are done mousing
        refRect = io.refRect                           # don't need copy, need to discern if rectangle has changed
        # this is grabcut
        mask = np.zeros(frame.shape[:2], np.uint8)
        bgdModel = np.zeros((1, 65), np.float64)
        fgdModel = np.zeros((1, 65), np.float64)
        cv2.grabCut(frame, mask, refRect, bgdModel, fgdModel, 5, cv2.GC_INIT_WITH_RECT)
        mask2 = np.where((mask == 2) | (mask == 0), 0, 1).astype('uint8')
        # TODO spinner blur grabs more background than I'd like - in that case you need to erode before histogram
        # hue+sat better than just hue. hue+sat+val occluding everything. not sure if bug
        gcimg = frame * mask2[:, :, np.newaxis]
        cv2.imshow('grabcut', gcimg)
        mask3 = np.where((mask2==1), 255,0).astype('uint8')
        cv2.dilate(mask3,np.ones((2,2)))
        cv2.imshow('mask', mask3)
        mask = mask3
        haveSelection = True

    if haveSelection and not haveFlow:
        # Take first frame and find corners in it
        old_frame = frame.copy()
        old_gray = cv2.cvtColor(old_frame, cv2.COLOR_BGR2GRAY)
        p0 = cv2.goodFeaturesToTrack(old_gray, mask=mask, **feature_params)
        totalFeatures = len(p0)
        # Create a mask image for drawing purposes
        mask = np.zeros_like(old_frame)
        haveFlow = True

    if haveFlow:
        # calculate optical flow
        p1, st, err = cv2.calcOpticalFlowPyrLK(old_gray, frame_gray, p0, None, **lk_params)

        # Select good points
        good_new = p1[st == 1]
        good_old = p0[st == 1]

        # draw the tracks
        for i, (new, old) in enumerate(zip(good_new, good_old)):
            a, b = new.ravel()
            c, d = old.ravel()
            mask = cv2.line(mask, (a, b), (c, d), color[i].tolist(), 2)
            frame = cv2.circle(frame, (a, b), 5, color[i].tolist(), -1)
        img = cv2.add(frame, mask)
    else:
        img = frame.copy()

    if io.mousing:
        cv2.rectangle(img, io.refPt, io.refEnd, (0, 255, 0), 2)

    cv2.imshow("optical flow", img)

    # Esc key to stop, otherwise repeat after 20 milliseconds
    key_pressed = cv2.waitKey(20)

    if key_pressed == ord('p'):         # if 'p' is pressed, pause until keypress
        refresh = not refresh

    if key_pressed == 27 or key_pressed == ord('q'):
        break

    # TODO need to remask if you need features. Could take a automated cut around existing points?
    if haveFlow:
        # update features
        if len(p1) <= totalFeatures / 2:
            pass
            '''
            old_gray = frame_gray.copy()
            p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **feature_params)
            totalFeatures = len(p0)
            mask = np.zeros_like(old_frame)  # keep this line if you would like to remove all previously drawn flows
            '''
        else:
            old_gray = frame_gray.copy()
            p0 = good_new.reshape(-1, 1, 2)

cv2.destroyAllWindows()
cap.release()
