// TRANSFORM COLORS FROM RGB TO INTEGER (ACCORDING TO ALLEGRO)

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <math.h>
#include <allegro.h>

// #define NDEBUG
#include <assert.h> 	//assert(h > 3);

// PERSONAL LIBRARIES
#include "plib.h"
#include "tlib.h"

#define XWIN 		1024		// window x resolution
#define YWIN 		768			// window y resolution

int main(){

	allegro_init();
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);		// enters the graphic mode, for now virtual screen is not defined
	clear_to_color(screen, 0);
	unsigned int black = makecol(0, 0, 0);
	printf("black is:%d\n", black);

	unsigned int red = makecol(255, 0, 0);
	printf("red is:%d\n", red);

	unsigned int light_gray = makecol(211, 211, 211);
	printf("light_gray is:%d\n", light_gray);

	unsigned int blue = makecol(0, 0, 255);
	printf("blue is:%d\n", blue);

	unsigned int green = makecol(0, 255, 0);
	printf("green is:%d\n", green);

	unsigned int cyan = makecol(0, 255, 255);
	printf("cyan is:%d\n", cyan);

	unsigned int light_pink = makecol(255, 102, 255);
	printf("light_pink is:%d\n",light_pink);

	unsigned int dark_green = makecol(0, 100, 0);
	printf("dark_green is:%d\n", dark_green);

	unsigned int burnt_orange = makecol(204,85,0);
	printf("burnt_orange is:%d\n", burnt_orange);

	unsigned int magenta = makecol(255, 0, 255);
	printf("magenta is:%d\n", magenta);

	unsigned int brown = makecol(165,42,42);
	printf("brown is:%d\n", brown);

	unsigned int electric_blue = makecol(125, 249, 255);
	printf("electric_blue is:%d\n", electric_blue);

	unsigned int light_green = makecol(150, 249, 123);
	printf("light_green is:%d\n", light_green);

	unsigned int electric_green = makecol(204, 255, 0);
	printf("electric_green is:%d\n", electric_green);

	unsigned int electric_purple = makecol(191, 0, 255);
	printf("electric_purple is:%d\n", electric_purple);

	unsigned int light_cyan = makecol(172, 255, 252);
	printf("light_cyan is:%d\n", light_cyan);

	unsigned int light_red = makecol(255, 71, 76);
	printf("light_red is:%d\n", light_red);

	unsigned int light_magenta = makecol(231, 139, 231);
	printf("light_magenta is:%d\n", light_magenta);

	unsigned int neon_carrot = makecol(255, 153, 51);
	printf("neon_carrot is:%d\n", neon_carrot);

	unsigned int yellow = makecol(255, 255, 0);
	printf("yellow is:%d\n", yellow);

	unsigned int white = makecol(255, 255, 255);
	printf("white is:%d\n", white);

	// int black = makecol(0, 0, 0);
	// printf("black is:%d\n", black);
	// int black = makecol(0, 0, 0);
	// printf("black is:%d\n", black);
	// int black = makecol(0, 0, 0);
	// printf("black is:%d\n", black);
	// int black = makecol(0, 0, 0);
	// printf("black is:%d\n", black);
	// int black = makecol(0, 0, 0);
	// printf("black is:%d\n", black);
	// int black = makecol(0, 0, 0);
	// printf("black is:%d\n", black);
	// int black = makecol(0, 0, 0);
	// printf("black is:%d\n", black);
	// int black = makecol(0, 0, 0);
	// printf("black is:%d\n", black);

	allegro_exit();

	return 0;
}