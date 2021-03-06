##########################################
####Converts the error into drone commands

from DistanceError import *
from Mod import *


###Constant parameters
MaxPhiSpeed = 0.08
MaxThetaSpeed = 0.07
xerr = 0.52
bbXrefWidth = 60
bbXrefHeight = 60


#Returns phi angle value according to error measurements 
def GetPhiValue(ctr,imagectr,px,width,img_width,prevPhi):

	phi = 0
	phi_1 = 0
	if (not isInside(ctr,(imagectr[0]-bbXrefWidth/2,imagectr[1]-bbXrefHeight/2),bbXrefWidth)):

		#calculate xError respect the center and calculate phi angle 	    	  			
		errx = fxError(px,width,img_width) - xerr		
		phi = relativeXspeed(errx)

		if (phi > 0):
			phi = math.pow(mod(errx*2),2.5) 
		elif (phi < 0):
			phi = -( math.pow(mod(errx*2),2.5)) 		
		#MaxPhiSpeed
		if (mod(phi) > MaxPhiSpeed):
			if (phi>0):
				phi = MaxPhiSpeed
			else:
				phi = -MaxPhiSpeed

		phi_1 = phi
	else:
		if (prevPhi!= 0):
			phi = -(prevPhi*4)
			phi_1 = 0

	return phi, phi_1
	
#Returns theta angle value according to error measurements
#bbZrefWidth = Reference width taken from the first bounding box input by the user
#bbZrefHeight = Reference height taken from the first bounding box input by the user
#CurrentWidth, CurrentHeight = Width and height of the current detection
def GetThetaValue(bbZrefWidth, bbZrefHeight, CurrentWidth, CurrentHeight,PrevTheta):

	theta = 0
	theta_1 = 0
	if (not isInsideWidth(bbZrefWidth,CurrentWidth)):

		errz = 1-fzError (bbZrefWidth,bbZrefHeight,CurrentWidth,CurrentHeight)
		theta = relativeZspeed(errz)

		if (theta > 0):
			theta = math.pow(mod(errz*2),2.5)
		elif (theta < 0):
			theta = - (math.pow(mod(errz*2),2.5))

		#max speed
		if (mod(theta) > MaxThetaSpeed):
			if (theta>0): 
				theta = MaxThetaSpeed
			else:
				theta = -MaxThetaSpeed

		theta_1=theta
	else:
		if (PrevTheta != 0):
			theta = -(PrevTheta*4)
			theta_1 = 0
	
	return theta,theta_1

#Returns gaz angle value according to error measurements
def GetGazValue(Yerr):
	if (Yerr < 0):
		return -1.0
	elif (Yerr > 0):
		return 1.0
	else:
		return 0
	
		
############################Commands speed###############################################
	
#Calculates X speed according to x error
def relativeXspeed (errx):
	if (mod(errx) > 0.30):
		return errx*0.8
	else:
		return errx*0.3

		
#Calculates Z speed according to Z error
def relativeZspeed(errz):
	if (errz > 0.30):
		return errz *0.8
	else:
		return errz *0.3

		
#Calculate Y speed according to Y error
def relativeYspeed(erry):	
	return erry*0.5
	

