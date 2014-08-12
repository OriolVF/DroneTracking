/*
autopylot_commands.c - command functions for AR.Drone Autopylot.

    Copyright (C) 2013 Simon D. Levy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as 
    published by the Free Software Foundation, either version 3 of the 
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License 
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 You should also have received a copy of the Parrot Parrot AR.Drone 
 Development License and Parrot AR.Drone copyright notice and disclaimer 
 and If not, see 
   <https://projects.ardrone.org/attachments/277/ParrotLicense.txt> 
 and
   <https://projects.ardrone.org/attachments/278/ParrotCopyrightAndDisclaimer.txt>.

*/
 

// For talking to SDK
#include <ardrone_tool/ardrone_tool_configuration.h>
#include <ardrone_tool/ardrone_version.h>
#include <ardrone_tool/UI/ardrone_input.h>


// Globals
#include "ardrone_autopylot.h"

void zap() 
{

    g_is_bellycam = !g_is_bellycam;
	
    int32_t channel = g_is_bellycam ? ZAP_CHANNEL_VERT : ZAP_CHANNEL_HORI;
	
    ARDRONE_TOOL_CONFIGURATION_ADDEVENT(video_channel, &channel, NULL);
}

void set(float phi, float theta, float gaz, float yaw)
{
    //int hover = 1;
    // psi= 0, psi_accuracy = 0
    int hover = (phi == 0 && theta == 0 && yaw == 0) ?  0 : 1 ;
	ardrone_tool_set_progressive_cmd(hover, phi, theta, gaz, yaw, 0, 0);
}

void takeOff_Land(int command)
{
	int32_t start;
	if (command == 1) 
	{	
		start = 1;
	}
	else 
	{
		start = 0;
	}
	printf("Start: %d \n",start);
	ardrone_tool_set_ui_pad_start(start);
}

void recover()
{
	//isEmergency=1;
	ardrone_tool_set_ui_pad_select(1);
}

