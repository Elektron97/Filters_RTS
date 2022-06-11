/*INCLUDE*/
//Standard Libraries and Pthred libraries
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <allegro.h>
#include <time.h>

//Custom Libraries
#include "timeLib.h"
#include "taskLib.h"
#include "filtersLib.h"

/*GLOBAL VARIABLES*/
int end_flag = 0;

/*FUNCTIONS*/
void init()
{
    allegro_init();
    install_keyboard();

    set_color_depth(8); //256 VGA

    //Enter in Graphics mode with full screen(width, height)
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, WIDTH, HEIGHT, 0, 0);

	//Clear Screen: ("global BITMAP called screen")
	clear_to_color(screen, BLACK); //Black Background
}

void *helloWorldTask(void* arg)
{
    int idx;

    idx = get_task_index(arg);
    set_activation(idx);

    while(!end_flag)
    {
        printf("Hello World! \n");
        
        if(deadline_miss(idx))
            printf("******Deadline Miss!******** \n");

        wait_for_activation(idx);
    }

    return NULL;
}

void *waveTask(void* arg)
{
    int idx = get_task_index(arg);
    set_activation(idx);

    int k = 0;
    double time = 0;
    double amplitude = 0;

    while(!end_flag)
    {
        time = k*time_step;
        amplitude = HEIGHT/2 + 100*sin(2*PI*1000*time);

        //Draw points
        putpixel(screen, time, amplitude, YELLOW);
        printf("k: %d\n", k);
        printf("current_time = %f\n", time);
        printf("current amplitude = %f\n", amplitude);

        //Successive sample
        k++;


        if(deadline_miss(idx))
            printf("******Deadline Miss of Wave Task!******** \n");
        
        wait_for_activation(idx);
    }

    return NULL;
}