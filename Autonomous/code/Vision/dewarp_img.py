import cv2
import numpy as np
import matplotlib.pyplot as plt
import time

# build the mapping
def buildMap(Ws,Hs,Wd,Hd,R1,R2,Cx,Cy):
    mapx = np.zeros((Hd,Wd),np.float32)
    mapy = np.zeros((Hd,Wd),np.float32)
    wd = 1. / float(Wd)
    hd = 1. / float(Hd)
    for y in range(0,int(Hd-1)):
        for x in range(0,int(Wd-1)):
            r = (float(y) * hd)*(R2-R1)+R1
            theta = (float(x) * wd)*2.0*np.pi
            xS = Cx+r*np.sin(theta)
            yS = Cy+r*np.cos(theta)
            mapx[y,x] = int(xS)
            mapy[y,x] = int(yS)
    np.savez('cammap', mapx=mapx, mapy=mapy)
    return mapx, mapy

img = cv2.imread('360.jpg')
makeMap = False
if makeMap:
    # center of the "donut"
    Cx = 600
    Cy = 390
    # Inner donut radius
    R1 = 120
    # outer donut radius
    R2 = 220
    # our input and output image siZes
    Wd = np.int32(2.0*((R2+R1)/2)*np.pi)
    Hd = (R2-R1)
    Ws = 1280
    Hs = 720
    # build the pixel map, this could be sped up
    print "BUILDING MAP!"
    tic = time.time()
    xmap,ymap = buildMap(Ws,Hs,Wd,Hd,R1,R2,Cx,Cy)
    print time.time() - tic
    print "MAP DONE!"
else:
    maps = np.load('cammap.npz')
    print maps.files
    xmap = maps['mapx']
    ymap = maps['mapy']

result = cv2.remap(img,xmap,ymap,cv2.INTER_LINEAR)  # 0.07 sec for 1280x720 and 640x360... hopefully debug overhead

cv2.imshow("unwarped", result)
cv2.imshow("warped", img)

plt.figure(1)
plt.subplot(211)
plt.imshow(xmap)
plt.subplot(212)
plt.imshow(ymap)
plt.show()