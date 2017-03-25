# USAGE
# python color_kmeans.py --image images/jp.png --clusters 3

# import the necessary packages
from sklearn.cluster import KMeans
import matplotlib.pyplot as plt
import numpy as np
import argparse
import cv2
import time


def centroid_histogram(labels):
    # grab the number of different clusters and create a histogram
    # based on the number of pixels assigned to each cluster
    numLabels = np.arange(0, len(np.unique(labels)) + 1)
    (hist, _) = np.histogram(labels, bins=numLabels)

    # normalize the histogram, such that it sums to one
    hist = hist.astype("float")
    hist /= hist.sum()

    # return the histogram
    return hist


def plot_colors(hist, centroids):
    # initialize the bar chart representing the relative frequency
    # of each of the colors
    bar = np.zeros((50, 300, 3), dtype="uint8")
    startX = 0

    # loop over the percentage of each cluster and the color of
    # each cluster
    for (percent, color) in zip(hist, centroids):
        # plot the relative percentage of each cluster
        endX = startX + (percent * 300)
        cv2.rectangle(bar, (int(startX), 0), (int(endX), 50),
                      color.astype("uint8").tolist(), -1)
        startX = endX

    # return the bar chart
    return bar


cap = cv2.VideoCapture('C:/Users/phahn/Desktop/doominator.mp4')

time.sleep(2)

# grab first frame for KMeans analsis
ret, image = cap.read()
image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

t = time.time()

# reshape the image to be a list of pixels
image = image.reshape((image.shape[0] * image.shape[1], 3))

clusters = 15

# OPENCV impl
Z = image.reshape((-1, 3))
# convert to np.float32
Z = np.float32(Z)
# define criteria, number of clusters(K) and apply kmeans()
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 10, 1.0)
ret, label, center = cv2.kmeans(Z, clusters, None, criteria, 10, cv2.KMEANS_RANDOM_CENTERS)
hist = centroid_histogram(label)
bar = plot_colors(hist, center)

print time.time() - t

print center

# show our color bar
plt.figure()
plt.axis("off")
plt.ion()
plt.imshow(bar)
plt.show()

while (cap.isOpened()):
    ret, image = cap.read()
    cv2.imshow('frame', image)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
