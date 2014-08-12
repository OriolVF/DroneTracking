
Drone person and object tracking implementation, based on TLD and DPM algorithms. This is a Python and C++ project for the Parrot ARDrone.	

			INSTALLING PREREQUISITES

-Ubuntu 12.4
-Opencv-2.4.6
-Python 2.7


The Deformable part based model and the TLD implementations are open source codes and are under a GNU General Public License. The original code can be found here:

DPM: https://www.idiap.ch/~cdubout/code/ffld.tar.gz
TLD: https://github.com/gnebehay/OpenTLD


Our implementation is based on that code but includes modifications in order to create the Python interfaces and to adapt it to our application requirements.

-Install DPM detector and create the needed wrappers

	1- cd ardrone_autopylot/PedestrianDetector
	2- Read the install.txt and readme.txt files to install all the libraries and dependencies for the DPM code
	2- mkdir build
	3- cd build
	4- cmake ..
	5- make
	
	If everything is fine we have compiled and built the DPM detector, and a Python wrapper class has been created. We can import this class then into any Python code and use the detector.

-Install the TLD tracker and create the needed wrappers

	1- cd ardrone_autopylot/TLD/openTLD
	2- Read the Readme.md file to install the OpenTLD libraries and dependencies
	3- mkdir build
	4- cd build
	5- cmake ..
	6- make

	If everything is fine we have compiled and built the TLD Tracker and a Python wrapper class has been created. We can import this class then into any Python code and use the tracker.


			RUNNING THE APPLICATION

-Installing the AR.Drone control system and the application.

The AR.Drone control system is based on the AR.Drone SDK tool and the implementation from: http://home.wlu.edu/~levys/software/ardrone_autopylot/ 


	1- cd ardrone_autopylot/AR.Drone
	2- Open the Makefile and change the SDK_PATH variable to the path where the AR.Drone SDK is located
	3- make
	4- An executable file with the name ardrone_autopylot will be created in ardrone_autopylot/Python.
	5- cd ardrone_autopylot/Python 
	6- ./ardrone_autopylot

The execution will start.

To change between the DPM and the TLD code change the autopylot_agent file for the autoplyot_agentDPM

				USAGE

If the algorithm is the TLD a window will appear asking the user to draw a bounding box around the desired target. When done press enter and the application will start tracking.

The controls to manage the drone are the following:


	- Space: Take off / land
	- p : Enable/disable tracking mode
	- r : recover from emergency state
	- n : Select new target to track (only for TLD)
	- m : Enable/disable learning (Only for TLD)

	- w : go up
	- s : go down
	- a : rotate left
	- d : rotate right
	- i : go forward
	- k : go backward
	- l : go right
	- j : go left
