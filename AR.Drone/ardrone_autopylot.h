/*
ardrone_autopylot.h - global variable declarations for AR.Drone Autopylot program

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

#ifndef _MAIN
extern
#endif
bool_t g_autopilot, g_is_bellycam;

#ifndef _MAIN
extern
#endif
navdata_demo_t g_navdata;

// Switches cameras
void zap();

// Sends command to drone
void set(float phi, float theta, float gaz, float yaw);

// Sends takeoff/landing signals
void takeOff_Land (int command);

//Recovers/sets emergency state
void recover();

