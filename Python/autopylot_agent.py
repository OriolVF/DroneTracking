'''
Python TLD interface object and person Tracker Implementation
'''

import cv
import os
from datetime import datetime
import imp
import numpy as np
import math

#DroneLibraries
from DistanceError import *
from DroneCommands import *
from Mod import *
import SaveImage



##################PARAMETERS#######################################
WINDOW_NAME = 'Drone CAM' 
ImagePath = "Video/"
minAltitude = 1000
maxAltitude = 1200

bbXrefHeight = 80

#Reference values to interpolate and get the reference bounding box upper body / medium box
'''bbMinDetection = 30
bbMaxDetection = 150
bbMinRefWidth = 30
bbMaxRefWidth = 150'''

# Reference values to interpolate face
bbMinDetection = 10
bbMaxDetection = 25
bbMinRefWidth = 10
bbMaxRefWidth = 25
#######################################################################


# Routine called by C program.
def action(img_bytes, img_width, img_height, is_belly, ctrl_state, vbat_flying_percentage, theta, phi, psi, altitude, vx, vy, vz):

	
	# Set up command defaults
	zap = 0
	phi = 0     
	theta = 0 
	gaz = 0
	yaw = 0
	imagectr = (img_width/2, img_height/2)
	   
	# Create full-color image from bytes
	image = cv.CreateImageHeader((img_width,img_height), cv.IPL_DEPTH_8U, 3)      
	cv.SetData(image, img_bytes, img_width*3)

    # Init, only called the first iteration
	if not hasattr(action, 'count'):
		action.count = 0
		action.phi_1 = 0
		action.theta_1 = 0
		action.gaz_1 = 0
		action.Tracker = _InitTracker(np.array(image[:,:]))
		action.flying = False
		action.previousFrame = img_bytes
		action.boundingboxes = list()
		action.currentbb = None
		action.reached = False
		action.bbRefWidth = 0
		action.track = 5
		dt = datetime.now()
		#action.log = open("Log_"+str(dt.day)+str(dt.hour)+str(dt.minute)+str(dt.second)+".txt","w")		
		action.MoveVertical = False
		action.countFrames = False
		cv.DestroyAllWindows()

		#Create folder to record the current sequence
		#action.pathName = "Sequence_"+str(dt.day)+str(dt.hour)+str(dt.minute)+str(dt.second)
		#if (not os.path.exists(action.pathName)):
		#	os.makedirs(action.pathName)


	#Don't send any commands while taking off and landing
	if (action.track == 1 or action.track == 2):
		action.track = 5

    # calculate tracking 
	result = action.Tracker.Run(img_width,img_height,str(img_bytes),img_width,img_height,str(action.previousFrame))
	action.previousFrame = img_bytes

	if (action.count == 0):

        #Calculate center of image and reference Window according to the first bounding box drawing
		coords = result.split(";")
		action.bbRefWidth = int(coords[2])
		action.bbRefHeight = int(coords[3]) 
		action.bbRefTopLeft = (imagectr[0]-action.bbRefWidth/2,imagectr[1]-action.bbRefHeight/2)
		action.bbRefBottomRight = (imagectr[0] + action.bbRefWidth/2,imagectr[1] + action.bbRefHeight/2)  
        
	if (result != "-1"):
        
        # Append the new bounding box to the previous detected bounding boxes
		action.boundingboxes.append(result)
		pxsum = 0
		pysum = 0
		widthsum = 0
		heightsum = 0

        # calculate the average of the previous 5 bounding boxes
		ponderateValues = [0.4,0.25,0.2,0.1,0.05]
		i=0;
		for bb in action.boundingboxes:
			coords = bb.split(";")
			pxsum = pxsum + int (int(coords[0]) * ponderateValues[i])
			pysum = pysum + int (int(coords[1]) * ponderateValues[i])
			widthsum =  widthsum + int( int(coords[2]) * ponderateValues[i])
			heightsum = heightsum + int(int(coords[3]) * ponderateValues[i])
			i+=1

        # Calculate detection bounding box, coordinates center and update current bounding box
		px = pxsum 
		py = pysum 
		width = widthsum 
		height = heightsum 
		ctr = (px + width/2 , py + height/2)
		action.currentbb = (px,py,width,height)
 		
		#if the lentgh is 5 the first element is removed so in the next iteration we will have the last 5 elements                   
		if (len(action.boundingboxes) == 5):
			action.boundingboxes.pop(0)
               			

		#Get referencebb

		referencebb = getBBreference(action.currentbb,imagectr)

		#To avoid accidents check bounding box width and if bounding box is touching any border for more than 5 frames                   
		if (width > 10 ):
 
            #Before doing any tracking the drone has to reach a minimum altitude	   
			if (altitude < minAltitude):
				gaz = 1.0
				action.gaz_1 = gaz
				action.MoveVertical = True
				action.countFrames = 0
			elif (altitude > maxAltitude):
				gaz = -1.0	 
				action.gaz_1 = gaz
				action.MoveVertical = True
				action.countFrames = 0	
			
			#Checks if the objective is within the altitude references, if comented the drone will only fly between the maximum and minum altitude
			#elif (not isInsideHeight (py, py+height,img_height)):
			#	erry = 0.5-fyError(py,action.bbRefHeight,img_height)
			#	if (erry > 0):
			#		gaz = 0.5
			#	else:
			#		gaz = -0.5					
			#	action.gaz_1 = gaz
			#	action.MoveVertical = True
			#	action.countFrames = 0

			else:

				if (action.countFrames < 2 and action.MoveVertical):
					gaz = action.gaz_1
					action.countFrames+=1					
				else:
					action.countFrames = 0
					action.MoveVertical = False
					action.reached = True		
					#Get commands speed						
					(phi,action.phi_1) = GetPhiValue(ctr,imagectr,px,width,img_width,action.phi_1)
					(theta,action.theta_1) = GetThetaValue(action.bbRefWidth,action.bbRefHeight,width,height,action.theta_1)
					

		action.count +=1	

		#Draw Reference window, detection and center of detection
		cv.Rectangle(image,referencebb[0],referencebb[1],(255,0,0),3)      
		cv.Rectangle(image,(action.currentbb[0],action.currentbb[1]),(action.currentbb[0]+action.currentbb[2],action.currentbb[1]+action.currentbb[3]),(0,0,255),3)			   
		cv.Circle(image, ctr, 10,(0,0,0),-1)
	

	
    #Read keyboard inputs to take off, land or toogle options 
	OnKeyPressedAction(img_width,img_height,img_bytes)


	if (action.track==0):
		_writeparametersOnScreen(image,zap,phi,theta,gaz,yaw,vbat_flying_percentage,altitude)
	else:
		_writeparametersOnScreen(image,zap,0,0,0,0,vbat_flying_percentage,altitude)
	
    # ShowImage   
	drawline(image,img_width,img_height)
	cv.NamedWindow(WINDOW_NAME)
	cv.ShowImage(WINDOW_NAME, image)
	
	
    #SaveImage
	#SaveImage.saveImage(action.count,image,action.pathName)
	
    # Log commands and send control parameters back to drone
	#log(action.log,action.track, phi, theta, gaz, yaw, altitude)

	return (action.track, phi, theta, gaz, yaw)




#Initializes TLD tracker
def _InitTracker(img):

	MainOTLD = imp.load_source('MainOTLD','../TLD/opentld/build/MainOTLD.py')
	Tracker = MainOTLD.MainOTLD();
	Tracker.Init(img.shape[1],img.shape[0],img.tostring())
	
	return Tracker



def OnKeyPressedAction(img_width,img_height,img_bytes):

	k = cv.WaitKey(5)
	key = chr(k & 255)
	phi = 0
	theta = 0
	gaz = 0
	yaw = 0
	speed = 0.1
	#Keyboard actions
	if (k == 32 or k == 1048608): #space
		if (not action.flying):
			zap = 1 #TakeOff
			action.track = zap
			action.flying = not action.flying
			print "Taking off"
		else:
			zap = 2 #Land
			action.track = zap
			action.flying = not action.flying
			action.reached = False
			print "Landing" +  str(action.flying)

	elif (key=="r"): 
		zap = 3
		action.track = zap
		print "recover"

	elif (key=="m"):
		action.Tracker.ToogleOptions(0,img_width,img_height,str(img_bytes))
		print ("python: learning")

	elif (key=="p"): 	
		if (action.track == 0):
			action.track = 5
			print "Stop tracking"
		else:
			action.track = 0
			print "Start tracking"

	elif (key=="n"): 
		cv.DestroyAllWindows();
		print "Select new target"
		action.Tracker.ToogleOptions(2,img_width,img_height,str(img_bytes))
	elif (key == "a"):
		yaw = speed
		print "rotating right"
	elif (key == "d"):
		yaw = -speed
		print "rotating left"
	elif (key == "w"):
		gaz = 1.0
		print "going Up"
	elif (key =="s"):
		gaz = -1.0
		print "going down"
	elif (key == "i"):
		theta = -speed
		print "going foward"
	elif (key =="k"):
		theta = speed
		print "going backward"
	elif (key == "j"):
		print "going left"
		phi = -speed
	elif (key == "l"):
		phi = speed
		print "going right"
		


#Reference lines
def drawline(img,img_width,img_height):
	
	startLine1 = (0,40)
	endLine1 = (img_width,40)
	
	startLine2 = (0,img_height-40)
	endLine2 = (img_width,img_height-40)
	cv.Line(img,startLine1,endLine1,(255,0,0),2,8)
	cv.Line(img,startLine2,endLine2,(255,0,0),2,8)
	

#Calculate reference bounding box size pixel/distance scalation by interpolation
def getBBreference(detectedbb,imagectr):
	
	width = detectedbb[2]
	refbbWidth =  bbMaxRefWidth + ( (bbMinRefWidth - bbMaxRefWidth) / (bbMaxDetection - bbMinDetection) ) * (width - bbMinDetection) 

	if ( refbbWidth < 50 ):
		refbbWidth = 50
	elif (refbbWidth > 130):
		refbbWidth = 130
	refbb = ((imagectr[0]-refbbWidth/2,imagectr[1]-bbXrefHeight/2),(imagectr[0]+refbbWidth/2,imagectr[1]+bbXrefHeight/2))	

	return refbb
	
	
#Display control and navdata parameters
def _writeparametersOnScreen(img,zap,phi,theta,gaz,yaw,battery,altitude):
	
	Point1 = (5,10)
	Point2 = (5,25)
	Point3 = (5,40)
	Point4 = (5,55)
	Point5 = (img.width-90,10)
	Point6 = (img.width-90, 25)
	Point7 = (5,75)
	Point8 = (5,90)
	Point9 = (5,105)
	Point10 = (5,120)

	fontTypeBig = cv.InitFont(cv.CV_FONT_HERSHEY_SIMPLEX,1, 1, 1, 3, 2)
	fontType = cv.InitFont(cv.CV_FONT_HERSHEY_SIMPLEX,0.4, 0.4, 0, 1, 2)
	color = cv.RGB(255, 0, 0)

	zapLine = "Zap: " + str(zap)
	phiLine = "Phi: " + str(phi)
	thetaLine = "Theta: " + str(theta)
	gazLine = "Gaz: " + str(gaz)
	yawLine = "Yaw: " + str(yaw)
	batteryLine = "Battery: " + str(battery) + "%"
	altitudeLine = "Altitude: " + str(altitude/float(1000)) + "m"
	direction1 = ""
	direction2 = ""
	if phi > 0:
		direction1 = "--->"
	elif phi < 0:
		direction1 = "<---"
	if theta > 0:
		direction2 = "Backward"
	elif theta < 0:
		direction2 = "Forward"

	cv.PutText(img, direction1, Point2, fontTypeBig, color)
	cv.PutText(img, direction2, Point4, fontTypeBig, color)
	cv.PutText(img, phiLine, Point7, fontType, color)
	cv.PutText(img, thetaLine, Point8, fontType, color)
	cv.PutText(img, gazLine, Point9, fontType, color)
	cv.PutText(img, yawLine, Point10, fontType, color)
	cv.PutText(img, batteryLine, Point5, fontType, color)
	cv.PutText(img, altitudeLine, Point6, fontType, color)


def log(logfile,action, phi, theta, gaz, yaw, altitude):

	line = str(phi)+";"+str(theta)+";"+str(gaz)+";"+str(yaw)+";"+str(altitude)+ "\n"
	logfile.write(line) # Write a string to a file

