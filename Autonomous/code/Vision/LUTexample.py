# todo
import cv2
import time
import numpy as np

'''
This is good at capturing colorful bots
TODO:
    1. make mask for Joshua's videos (gopro is fixed not moving)
    2. bring in video and filter through mask
    3. detect blob in masked_frame
    4. draw line from blob to center of picture
'''

bits = 2;
identity = np.arange(256, dtype=np.dtype('uint8'))
identity = np.right_shift(identity, 8 - bits)  # reduce 'bits' lsb
identity = np.left_shift(identity, 8 - bits)  #
print identity

zeros = np.zeros(256, np.dtype('uint8'))
# lut = np.dstack((identity, zeros, zeros))
lut = np.dstack((identity, identity, identity))

cap = cv2.VideoCapture('C:/Users/phahn/Desktop/bodacious.mp4')
w = int(cap.get(3))
h = int(cap.get(4))
write = cv2.VideoWriter('lut.avi', cv2.VideoWriter_fourcc(*'MJPG'), 30, (w, h))

time.sleep(2)

while (cap.isOpened()):
    ret, frame = cap.read()
    frame2 = cv2.LUT(frame, lut)
    cv2.imshow('LUT', frame2)

    # try thresholding out pit / surroundings
    hsv = cv2.cvtColor(frame2, cv2.COLOR_BGR2HSV)
    hue, sat, val = cv2.split(hsv)
    cv2.imshow("Hue", hue)
    cv2.imshow("Sat", sat)
    cv2.imshow("Val", val)

    # H S V
    lower_range = np.array([0, 96,
                            32])  # this is getting good for 2 bit color, colored bots are unmasked. wall artifacts disappear w/bots unfortunately
    upper_range = np.array([180, 255, 196])  # This is ineffective with 1 bit color
    mask = cv2.inRange(hsv, lower_range, upper_range)

    # Bitwise-AND mask and original image
    masked_frame = cv2.bitwise_and(frame2, frame2, mask=mask)
    cv2.imshow("Masked", masked_frame)
    cv2.imshow("Mask", mask)

    write.write(masked_frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
write.release()
