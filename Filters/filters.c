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

int main()
{
	/*Init*/
	allegro_init();
	install_keyboard();	//Keyboard

	set_color_depth(8);	//256 VGA

	//Enter in Graphics mode with full screen(width, height)
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, WIDTH, HEIGHT, 0, 0);

	//Clear Screen: ("global BITMAP called screen")
	clear_to_color(screen, BLACK); //Black Backg(round

	circlefill(screen, 100, 100, 50, 4);

	readkey();
	allegro_exit();
	return 0;
}