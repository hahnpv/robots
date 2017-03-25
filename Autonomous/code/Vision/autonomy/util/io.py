import cv2

# it sucks to have all of these members because they are repetitive but different operations require them
# instead of doing these calculations in situ it is more responsible to do it in one place correctly
refPt  = (0, 0)      # starting point       (opencv rectangle drawing needs start and end corner)
refEnd = (0, 0)      # moving/end point     (opencv rectangle drawing needs start and end corner)
refRect = (0,0,0,0)  # x  y  w  h           (opencv Rect)
refSlice = [0]       # y0 y1 x0 x1          (python slice)
mousing = False      # True if we are in the act of dragging the mouse

# Rectangle grabbing callback
def rect_grab(event, x, y, flags, param):
    global refPt, refEnd, mousing, refRect, refSlice
    if event == cv2.EVENT_LBUTTONDOWN:
        refPt = (x, y)
        refEnd = (x, y)
        mousing = True
    elif event == cv2.EVENT_MOUSEMOVE:
        if mousing:
            refEnd = (x, y)
    elif event == cv2.EVENT_LBUTTONUP:
        refEnd = (x, y)
        mousing = False
        # verify order is increasing
        refPt = list(refPt)
        refEnd = list(refEnd)
        for i in (0,1):
            if refEnd[i] < refPt[i]:
                tmp = refEnd[i];
                refEnd[i] = refPt[i];
                refPt[i] = tmp;
        refPt = tuple(refPt)
        refEnd = tuple(refEnd)
    refSlice = [slice(refPt[1], refEnd[1]), slice(refPt[0], refEnd[0])]
    refRect = (refPt[0], refPt[1], refEnd[0]-refPt[0], refEnd[1]-refPt[1])
