

#include <stdlib.h>
#include <sys/select.h>

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ardrone_tool/UI/ardrone_input.h>
#include "ardrone_autopylot.h"
#include <ardrone_tool/ardrone_tool.h>

#define KEY_UP 65 
#define KEY_DOWN 66 
#define KEY_LEFT 68  
#define KEY_RIGHT 67
#define SPACE 32 
#define KEY_ESC 27 

float speed = 0.05;
int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}
	 
	return 0;
}

int running = 1;
void init_keyboard(void)
{
	running =1;
	
}

void update_keyboard(void)
{
	int32_t start = 0;
	int isEmergency = 0;
	while(running)
	{	  		
		//printf("hello \n");
		if (kbhit())
		{
			
			int key= getchar();
			switch (key)
			{
				case SPACE : 
					g_autopilot = FALSE;
					if (start ==0)
					{
						start = 1;
					     	printf("Taking off\n");
					     	ardrone_tool_set_ui_pad_start( start );
					}
					else
					{
						start = 0;
						printf("Landing\n");
						ardrone_tool_set_ui_pad_start( start );
					}
					     break;
				case 'i'://KEY_UP: 
					g_autopilot = FALSE;
            				printf("Flying forward\n");
					set(0,-speed,0,0);
					fflush(stdout);
					break;
				case 'k'://KEY_DOWN: 
					g_autopilot = FALSE;
					printf("Flying backward\n");
					set(0,speed,0,0);
					break;
				case 'j'://KEY_LEFT : 
					g_autopilot = FALSE;
					printf("Flying to left\n");
		                        set(-speed,0,0,0);
					break;
				case 'l' : //KEY_RIGHT :
					g_autopilot = FALSE;
					printf("Flying to right\n");
					set(speed,0,0,0);
					break;
				case 'a':
					g_autopilot = FALSE;
					printf("Turning left\n");
					set(0,0,0,-0.3);
					break;
				case 'd':
					g_autopilot = FALSE;
					printf("Turning right\n");
					set(0,0,0,+0.3);
					break;
				case 'w':
					g_autopilot = FALSE;
					printf("Getting up\n");
					set(0,0,0.5,0);
					break;
				case 's':
					g_autopilot = FALSE;
					printf("Getting down\n");
					set(0,0,-0.5,0);
					break;
				case 'z':
					g_autopilot = FALSE;
					printf("Zapping camera\n");
					zap();
					break;
				case KEY_ESC: 
					g_autopilot = FALSE;
					isEmergency=1;
				  	ardrone_tool_set_ui_pad_select(isEmergency);
					printf("Stopping, sending emergency signal\n");
					running = 0;
					break;
				case 'g':
					if (g_autopilot)
					{
						printf("Autopilot face tracking is off: \n");
						g_autopilot = FALSE;
					}
					else
					{
						printf("Autopilot face tracking is on: \n");
						g_autopilot = TRUE;
					}
					break;
				case 'r':
					
					if (!isEmergency)
					{
						printf("Recovering signal\n");
						isEmergency=1;
						ardrone_tool_set_ui_pad_select(isEmergency);
					}
					else
					{
						printf("Recovering signal\n");
						isEmergency=0;
						ardrone_tool_set_ui_pad_select(isEmergency);
					}
					break;
				case 'h': 
					printf("hovering\n");
					ardrone_tool_set_progressive_cmd(0,0, 0, 0, 0, 0, 0);
					break;
				case 'm':
					printf ("Altitude: %d", g_navdata.altitude);
					printf ("Battery: %d", g_navdata.vbat_flying_percentage);
					break;
					
		
			}
	  		
		}
	}
	printf("Resuming thread \n");
}

void close_keyboard(void)
{
	running = 0;
	
}



