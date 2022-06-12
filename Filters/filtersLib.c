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
double signalRealization(struct Signal signal, double time)
{
    // y(k) = Asin(wt + phi)
    double y = signal.amplitude*sin(2*PI*signal.frequency*time + signal.phase);
    return y;
}

void printSignal(struct Signal signal)
{
    printf("********PLOT SIGNAL INFORMATION********\n");
    printf("* Frequency:  \t %f [Hz]\n", signal.frequency);
    printf("* Amplitude: \t %f\n", signal.amplitude);
    printf("* Phase: \t %f [rad]\n", signal.phase);
    printf("***************************************\n");
}

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
    /***********************************TASK DESCRIPTION*************************
     * Plot sinusoidal signal.                                                  *
     * Time axis is rescaled in a range of [0, 1s] (instead [0, WIDTH])         *
     * Amplitude axis is rescaled in a range of [-1, 1] (instead [0, HEIGHT])    *    
     ***************************************************************************/

    int idx = get_task_index(arg);
    set_activation(idx);

    //Init
    int k = 0;
    double time = 0.0;
    double y = 0.0;

    struct Signal signal = {1.0, 100.0, 0.0};
    double Ts = 1/(10*signal.frequency); //fs = 10*f_signal
    printSignal(signal);

    while((!end_flag))
    {
        /*COMPUTE SIGNAL*/
        //t = k*Ts
        time = k*Ts;
        //y(k) = Asin(wkTs + phi)
        y = signalRealization(signal, time);

        /*DRAW POINTS*/
        //Rescale
        putpixel(screen, WIDTH*time, (HEIGHT/2)*(y + 1), YELLOW);
        //Debug information
        printf("******** Iteration %d **********\n", k);
        printf("t = \t %f [s]\n", time);
        printf("y = \t %f\n", y);
        /*printf("Rescale time = \t %f\n", WIDTH*time);
        printf("Rescale y = \t %f\n", (HEIGHT/2)*(y + 1));*/

        //Successive sample
        k++;

        if(deadline_miss(idx))
            printf("******Deadline Miss of Wave Task!******** \n");
        
        wait_for_activation(idx);
    }

    return NULL;
}

