// MAKE PINK BACKGROUND OF A BITMAP OF BG GREEN

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

#define N 			14

int main(){

	allegro_init();
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);		// enters the graphic mode, for now virtual screen is not defined
	clear_to_color(screen, 0);

	BITMAP *shotgun_img;
	BITMAP *shotgunP;
	PALETTE 	pal;
	int 			x, y, c;
	unsigned int 	PINK, GREEN;
	float 			hue, sat, val;
	GREEN = makecol(0, 255, 0);
	PINK = makecol(255, 0, 255);

	shotgun_img = load_bitmap("Shot_sequence_bmp/13.bmp", NULL);
	if (!shotgun_img)
	{
		printf("Error loading shotgun\n");
		exit(1);
	}

	shotgunP = create_bitmap(shotgun_img->w, shotgun_img->h);
	if (!shotgunP)
	{
		printf("Error creating BITMAP\n");
		exit(1);
	}

	for(x = 0; x<shotgun_img->w; x++){
		for(y = 0; y<shotgun_img->h; y++){
			c = getpixel(shotgun_img, x, y);
			rgb_to_hsv(getr(c), getg(c), getb(c),
					   &hue, 	&sat, 	 &val);
			if (((hue >= 80) && (hue <= 195)) && (getg(c) > 70))
				c = PINK;
			// if( ((hue>=50/240*360)&&(hue<=90/240*360)) && (sat == 240/240) && (val==107/240))
				// c = PINK;
			putpixel(shotgunP, x, y, c);
		}
	}
	get_palette(pal);
	save_bitmap("Shot_sequence_bmp/13p.bmp", shotgunP, pal);

	allegro_exit();
	return 0;
}