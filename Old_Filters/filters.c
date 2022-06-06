/**********RTS PROJECT: FILTERS******
 * Simulate Filters and Signals		*
 ************************************/

/*INCLUDE*/
//std libraries
#include <stdio.h>
#include <stdlib.h>

//math operations
#include <math.h>

//Pthread
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

//Graphics
#include <allegro.h>

//Custom Libraries
#include "plib.h"
#include "tlib.h"

/*DEFINE*/
//Math
#define PI 3.14159

//Graphics
#define WIDTH 800
#define HEIGHT 700

//Colours
#define BLACK 0
#define YELLOW 14

//TASK
//Index
#define FILTERS_TASK_INDEX 0
#define GRAPHIC_TASK_INDEX 1

//Period
#define FILTERS_PERIOD 10 //100 Hz
#define GRAPHIC_PERIOD 10

//Deadline
#define FILTER_DREL 10
#define GRAPHIC_DREL 10

//Priority
#define FILTER_PRIO 20
#define GRAPHIC_PRIO 23

//Flag
int end = 0;
int command = 0;

/*struct Point2D
{
	double x;
	double y;
};*/

struct signals
{
	double amplitude;
	double frequency;

	double time;
	double value;
};

/*GLOBAL VARIABLE*/
struct signals signal;

//INITIALIZATION
void init()
{
	allegro_init();
	install_keyboard();	//Keyboard

	set_color_depth(8);	//256 VGA

	//Enter in Graphics mode with full screen(width, height)
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, WIDTH, HEIGHT, 0, 0);

	//Clear Screen: ("global BITMAP called screen")
	clear_to_color(screen, BLACK); //Black Background

}

//Low Pass Filter
double lowPassFilter(double y_k_1, double x_k_1, double a, double Ts)
{
	double p = exp(-a*Ts);
	double y_k = p*y_k_1 + (1-p)*x_k_1;

	return y_k;
}

/*void plotSignals(double frequency, int n_period, int color)
{
	const int scale_time = 20000;
	const int scale_y = 100;
	for(double t = 0; t <n_period/frequency; t+=1/(100*frequency))
	{
		double x2 = scale_time*t;
		double y2 = HEIGHT/2 + scale_y*sin(2*3.14*frequency*t);
		putpixel(screen, x2, y2, color);
	}
	
}*/

void plotSignals(struct signals signal, int color)
{
	const int scale_time = 20000;
	const int scale_y = 100;

	double x2 = scale_time*signal.time;
	double y2 = HEIGHT/2 + scale_y*signal.value;
	putpixel(screen, x2, y2, color);
	//readkey();
}

void *filters_task(void *arg)
{
	//Gestisco il tempo
	set_activation(FILTERS_TASK_INDEX);
	//Test
	while(!end)
	{
		//get time in seconds
		double time = (double) get_task_period(filters_task)/1000; 

		signal.frequency = 1/FILTERS_PERIOD;
		signal.time = time;
		signal.amplitude = 100;
		signal.value = signal.amplitude*sin(2*PI*signal.frequency*signal.time);

		wait_for_activation(FILTERS_TASK_INDEX);
	}

	return NULL;
}

/*void *user_task(void *arg)
{

	return NULL;
}*/

//Graphic Task
void *graphic_task(void *arg)
{
	set_activation(GRAPHIC_TASK_INDEX);

	while(!end)
	{
		plotSignals(signal, YELLOW);

		if (deadline_miss(GRAPHIC_TASK_INDEX)) 
		{			//check deadline miss
			printf("graphic_task deadline miss %d\n", param[GRAPHIC_TASK_INDEX].dmiss);
			//dlm_graphic++;
		}

		wait_for_activation(GRAPHIC_TASK_INDEX);
	}
	return NULL;
}

int main()
{
	/*Init*/
	init();

	task_create(graphic_task, GRAPHIC_TASK_INDEX, GRAPHIC_PERIOD, GRAPHIC_DREL, GRAPHIC_PERIOD);
	task_create(filters_task, FILTERS_TASK_INDEX, FILTERS_PERIOD, FILTER_DREL, FILTER_PRIO);

	allegro_exit();
	return 0;
}