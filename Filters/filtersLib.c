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
double sign(double x)
{
    if(x > 0)
        return 1.0;
    if(x < 0)
        return -1.0;
    else
        return 0.0;
}

//Low Pass Filter
double lowPassFilter(double y_k_1, double x_k_1, double a, double Ts)
{
    /************************************************
     *  LOW PASS FILTER:                            *
     * This function compute the discrete time form *
     * of transfer function G(s):                   *
     *              a                               *
     * G(s) =  -----------                          *
     *          s   +   a                           *      
     *                                              *    
     * a = omega_cut = 2*PI*f_cut                   *
     ************************************************/

	double p = exp(-a*Ts);
	double y_k = p*y_k_1 + (1-p)*x_k_1;

	return y_k;
}

void plotPoint(double time, double y, int color)
{
    //scaled_time = (WIDTH/XLIM)*time;
    //scaled_y = (HEIGHT/2) + (HEIGHT/2 - 1)*y;

    //Rescale & Plot
    putpixel(screen, (WIDTH/XLIM)*time, (HEIGHT/2) + (HEIGHT/2 - 1)*y, color);
}

double signalRealization(struct Signal signal, double time)
{
    double y;

    switch(signal.signal_type)
    {
        case sinusoid:
        // y(k) = A sin(wt + phi)
        y = signal.amplitude*sin(2*PI*signal.frequency*time + signal.phase);
        break;

        case square:
        // y(k) = A sgn(sin(wt + phi))
        y = signal.amplitude*sign(sin(2*PI*signal.frequency*time + signal.phase));
        break;

        case sawtooth:
        //y = - (2*signal.amplitude/PI) * atan(1/(tan(PI*signal.frequency*time + signal.phase)));
        y = (2*signal.amplitude/PI) * atan(1/(tan(PI*signal.frequency*time + signal.phase)));
        break;

        case triang:
        y = (2*signal.amplitude/PI) * asin(sin(2*PI*signal.frequency*time + signal.phase));
        break;

        default:
        y = 0.0;
        printf("Singal Type not valid. Output setted to zero!\n");
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

        case sawtooth:
        printf("* Signal Type: \t Sawtooth\n");
        break;

        case triang:
        printf("* Signal Type: \t Triangular\n");
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
     * Plot signals of 4 types: Sawtooth, Triangular, Sinusoid, Square.         *
     * Time axis is rescaled in a range of [0, XLIM] (instead [0, WIDTH])       *
     * Amplitude axis is rescaled in a range of [-1, 1] (instead [0, HEIGHT])   *    
     ****************************************************************************/

    int idx = get_task_index(arg);
    set_activation(idx);

    //Init
    int k = 0;
    double time = 0.0;
    double y = 0.0;

    double y_filtered = 0.0;

    struct Signal signal = {1.0, 30.0, 0.0, sinusoid};

    const double Ts = 1/(N_SAMPLE_PERIOD*signal.frequency); //fs = 100*f_signal | 100 samples for period

    //Print Information
    printf("Wave Task Period: %d\n", WAVE_PER);
    printSignal(signal);

    while((!end_flag) && ((WIDTH/XLIM)*time <= WIDTH))
    {
        //Test Filters
        //Filters algorithm needs y(k-1) and x(k-1)
        y_filtered = lowPassFilter(y_filtered, y, 2*PI*10, Ts);

        /*COMPUTE SIGNAL*/
        time = k*Ts;
        y = signalRealization(signal, time);

        /*DRAW POINTS*/
        plotPoint(time, y, YELLOW);
        plotPoint(time, y_filtered, WHITE);

        //Debug information
        /*printf("******** Iteration %d **********\n", k);
        printf("t = \t %f [s]\n", time);
        printf("y = \t %f\n", y);*/

        //Successive sample
        k++;

        if(deadline_miss(idx))
            printf("*****Deadline Miss of Wave Task!*******\n");
        
        wait_for_activation(idx);
    }

    return NULL;
}

