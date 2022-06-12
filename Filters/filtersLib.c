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
    double y;
    int n_period = n_period = (time + signal.phase)*signal.frequency;
    printf("Number of Periods: %d", n_period);

    switch(signal.signal_type)
    {
        case sinusoid:
        // y(k) = Asin(wt + phi)
        y = signal.amplitude*sin(2*PI*signal.frequency*time + signal.phase);
        break;

        case square:
        //Periodicity
        if(n_period >= 1)
            time -= n_period/signal.frequency + signal.phase;

        //Rect
        if((time + signal.phase) <= 1/(2*signal.frequency))
            y = signal.amplitude;

        else
            y = -signal.amplitude;

        break;

        default:
        y = 0.0;
        printf("Singal Type not valid. Output setted to zero\n");
        break;


    }

    return y;
}

void printSignal(struct Signal signal)
{
    printf("********PLOT SIGNAL INFORMATION********\n");

    switch(signal.signal_type)
    {
        case sinusoid:
        printf("* Signal Type: \t Sinusoidal\n");
        break;

        case square:
        printf("* Signal Type: \t Square\n");
        break;

        default:
        printf("* Singal Type not valid.\n");
        break;
    }


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
     * Time axis is rescaled in a range of [0, XLIM] (instead [0, WIDTH])         *
     * Amplitude axis is rescaled in a range of [-1, 1] (instead [0, HEIGHT])    *    
     ***************************************************************************/

    int idx = get_task_index(arg);
    set_activation(idx);

    //Init
    int k = 0;
    double time = 0.0;
    double y = 0.0;
    double scaled_time = 0.0;
    double scaled_y = 0.0;

    struct Signal signal = {1.0, 10.0, 0.0, sinusoid};
    const double Ts = 1/(N_SAMPLE_PERIOD*signal.frequency); //fs = 100*f_signal | 100 samples for period

    //Print Information
    printf("Wave Task Period: %d\n", WAVE_PER);
    printSignal(signal);

    while((!end_flag) && (scaled_time < WIDTH))
    {
        /*COMPUTE SIGNAL*/
        //y(k) = Asin(w*kTs + phi)
        time = k*Ts;
        y = signalRealization(signal, time);

        /*DRAW POINTS*/
        scaled_time = (WIDTH/XLIM)*time;
        scaled_y = (HEIGHT/2) + (HEIGHT/2 - 1)*y;
        //Rescale & Plot
        putpixel(screen, scaled_time, scaled_y, YELLOW);

        //Debug information
        printf("******** Iteration %d **********\n", k);
        printf("t = \t %f [s]\n", time);
        printf("y = \t %f\n", y);
        /*printf("Rescale time = \t %f\n", scaled_time);
        printf("Rescale y = \t %f\n", scaled_y);*/

        //Successive sample
        k++;

        if(deadline_miss(idx))
            printf("*****Deadline Miss of Wave Task!*******\n");
        
        wait_for_activation(idx);
    }

    return NULL;
}

