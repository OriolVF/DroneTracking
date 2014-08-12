#Calculates the errors between the current detection and the optimal positions

from Mod import *
import math


############################Errors###############################################
#Parameters
widthRange = 0.8
heightRange = 40

#Calculates x error
def fxError (xbb, wbb,wim):
	return (xbb+(wbb/2)) / float(wim)


#Calculates z error, bounding box sizes
def fzError (wim,him,wbb,hbb):
	return math.sqrt( (wim*him) / float(wbb*hbb) )


#Calculates Y error
def fyError (ybb,hbb,him):
	return (ybb+(hbb/2)) / float (him)	

	
#Returns if the corners of the bounding box are within the limits
def isInsideHeight(tl,bl,img_height):
	return (tl - heightRange > 0) and (bl - (img_height-heightRange) < 0)
	
	
#Returns if the point is inside the width of the defined window
def isInside(point,windowRefTL,windowRefWidth):			
	return (point[0]-windowRefTL[0] > 0) and ((windowRefTL[0]+windowRefWidth)-point[0] > 0)
	
	
#Returns if the bounding box width is between the optimal width
def isInsideWidth (optimalWidth,windowWidth):
	if (windowWidth > optimalWidth):
		return optimalWidth >= (windowWidth * widthRange)
	elif (windowWidth < optimalWidth):
		return not (windowWidth <= (optimalWidth * widthRange))
	else:
		return True

