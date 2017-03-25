import numpy as np
import cv2
import time
import copy
from matplotlib import pyplot as plt


refPt  = (0, 0)
refEnd = (0, 0)
mousing = False
calcHist = False
# callback
def onclick(event, x, y, flags, param):
    global refPt, refEnd, calcHist, mousing, image
    if event == cv2.EVENT_LBUTTONDOWN:
        refPt = (x, y)
        mousing = True
    elif event == cv2.EVENT_MOUSEMOVE:
        if mousing:
            refEnd = (x, y)
    elif event == cv2.EVENT_LBUTTONUP:
        refEnd = (x, y)
        # verify order is increasing
        refPt = list(refPt)
        refEnd = list(refEnd)
        for i in (0,1):
            print i
            if refEnd[i] < refPt[i]:
                tmp = refEnd[i];
                refEnd[i] = refPt[i];
                refPt[i] = tmp;
        refPt = tuple(refPt)
        refEnd = tuple(refEnd)
        print refPt
        print refEnd
        if refPt[0] != refEnd[0] and refPt[1] != refEnd[1]:
            calcHist = True
        mousing = False

cv2.namedWindow("image")
cv2.setMouseCallback("image", onclick)

plt.ion()

my_camera = cv2.VideoCapture('C:/Users/phahn/Desktop/broheim.mp4')
wx = int(my_camera.get(3))
hx = int(my_camera.get(4))
time.sleep(2)
success, image = my_camera.read()
h = None
refresh = True
image = None
img = None
while (True):
    if refresh:
        success, image = my_camera.read()

    img = copy.deepcopy(image)

    if calcHist:        # temporary - try grabcut. roll in if it makes sense.
        mask = np.zeros(img.shape[:2],np.uint8)

        bgdModel = np.zeros((1,65),np.float64)
        fgdModel = np.zeros((1,65),np.float64)

        rect = (refPt[0], refPt[1], refEnd[0]-refPt[0], refEnd[1]-refPt[1])
        cv2.grabCut(img,mask,rect,bgdModel,fgdModel,5,cv2.GC_INIT_WITH_RECT)

        mask2 = np.where((mask==2)|(mask==0),0,1).astype('uint8')
        gcimg = img*mask2[:,:,np.newaxis]
        cv2.imshow('grabcut', gcimg)
        # TODO try doing kmeans? small area might compute quick enough for pre-fight check? otherwise still useful offline

        print "calculating histogram"
        img_HSV = cv2.cvtColor(image[refPt[1]:refEnd[1],refPt[0]:refEnd[0]], cv2.COLOR_BGR2HSV)
        h = np.zeros(image.shape)
        bins = np.arange(256).reshape(256,1)
        plt.cla()
        color = ('b','g','r')
        legend = ('Hue','Sat','Val')
        for ch, col in enumerate(color):
            hist_item = cv2.calcHist([img_HSV],[ch],None,[256],[0,256])
            cv2.normalize(hist_item,hist_item,0,255,cv2.NORM_MINMAX)
            hist=np.int32(np.around(hist_item))
            pts = np.column_stack((bins,hist))
#            cv2.polylines(h,[pts],False,col)
            h = plt.plot(hist,color = col, label=legend[ch])
            plt.figure(2)
            plt.xlim([0,256])
        plt.legend( loc='upper right')
        plt.axis([0, 255, 0, 300])
        plt.show()
        h=np.flipud(h)
        calcHist = False

    cv2.rectangle(img, refPt, refEnd, (0, 255, 0), 2)
    cv2.imshow('image', img)

    # Esc key to stop, otherwise repeat after 20 milliseconds
    key_pressed = cv2.waitKey(20)

    if key_pressed == ord('p'):         # if 'p' is pressed, pause until keypress
        refresh = not refresh

    if key_pressed == 27:
        break

cv2.destroyAllWindows()
my_camera.release()
