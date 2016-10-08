import cv2
from PIL import Image
import numpy as np
from pylab import *
%matplotlib inline

im=Image.open("a.png").convert('L')
im=cv2.resize(np.array(im), (32, 32), interpolation=cv2.INTER_CUBIC)
_, im = cv2.threshold(im, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
for row in im:
    output='"'
    for column in row:
        if column >240:
            output=output+' '
        elif column<100:
            output=output+'X'
        else:
            output=output+'.'
    output=output+'",'
    print output
