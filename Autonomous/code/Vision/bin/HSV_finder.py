import numpy as np
import cv2
import time
import copy

refPt = (0, 0)

# callback
def onclick(event, x, y, flags, param):
    global refPt
    if event == cv2.EVENT_LBUTTONDOWN:
        refPt = (y, x)


cv2.namedWindow("image")
cv2.setMouseCallback("image", onclick)

my_camera = cv2.VideoCapture('C:/Users/phahn/Desktop/fpv.mp4')
w = int(my_camera.get(3))
h = int(my_camera.get(4))
time.sleep(2)
refresh = True
image = None
while (True):
    if refresh:
        success, image = my_camera.read()
        #    image = cv2.GaussianBlur(image,(5,5),0)

    image2 = copy.deepcopy(image)
    image_HSV = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    colour = str(image_HSV[refPt[0]][refPt[1]])
    cv2.line(image2, (0, refPt[0]), (w - 1, refPt[0]), [255, 255, 255])
    cv2.line(image2, (refPt[1], 0), (refPt[1], h - 1), [255, 255, 255])
    cv2.putText(image2, colour, (10, 30), cv2.FONT_HERSHEY_PLAIN, 1, [0, 0, 0])
    cv2.imshow('image', image2)

    # Esc key to stop, otherwise repeat after 20 milliseconds
    key_pressed = cv2.waitKey(20)

    if key_pressed == ord('p'):         # if 'p' is pressed, pause until keypress
        refresh = not refresh

    if key_pressed == 27:
        break

cv2.destroyAllWindows()
my_camera.release()
