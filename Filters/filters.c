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
//Graphics
#define WIDTH 800
#define HEIGHT 700

//Colours
#define BLACK 0

//Test

//Low Pass Filter
double lowPassFilter(double y_k_1, double x_k_1, double a, double Ts)
{
	double p = exp(-a*Ts);
	double y_k = p*y_k_1 + (1-p)*x_k_1;

	return y_k;
}

void printSignals(double frequency, int n_period)
{
	const int scale_time = 20000;
	const int scale_y = 100;
	for(double t = 0; t <n_period/frequency; t+=1/(100*frequency))
	{
		double x2 = scale_time*t;
		double y2 = HEIGHT/2 + scale_y*sin(2*3.14*frequency*t);
		putpixel(screen, x2, y2, 14);
	}
	
}

int main()
{
	/*Init*/
	allegro_init();
	install_keyboard();	//Keyboard

	set_color_depth(8);	//256 VGA

	//Enter in Graphics mode with full screen(width, height)
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, WIDTH, HEIGHT, 0, 0);

	//Clear Screen: ("global BITMAP called screen")
	clear_to_color(screen, BLACK); //Black Background

	printSignals(100.0, 1);

	readkey();
	allegro_exit();
	return 0;
}