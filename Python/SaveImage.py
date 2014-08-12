import cv2
import numpy as np
import Image
from datetime import datetime
import os

def saveImage(count,img,pathName):
	image = np.asarray(img[:,:])
	name =pathName+"/video_"+str(count)	
	cv2.imwrite(name+".jpg",image)
	
