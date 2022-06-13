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
int n_active_filters = 0;
int n_active_signals = 0;

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
    printf("* Ts: \t %f [s]\n", signal.Ts);
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

void set_Ts(int idx)
{
    if(signals[idx].frequency != 0)
    {
        //fs = 100*f_signal | 100 samples for period
        signals[idx].Ts = 1/(N_SAMPLE_PERIOD*signals[idx].frequency); 
    }

    else
        signals[idx].Ts = 0;
}

void init_signal(int idx)
{
    /****************************************************
     * INIT SIGNAL: Define initial parameter of signal. *
     * Default signal: Sinusoidal Wave 1 Hz.            *
     ****************************************************/
    signals[idx].amplitude =    1.0;
    signals[idx].frequency =    1.0;        //[Hz]
    signals[idx].phase =        0.0;        //[rad]
    signals[idx].signal_type =  sinusoid;

    set_Ts(idx);
}

void keyboard_interp()
{
    /********************************    
     * KEYBOARD INTERPRETER:        *
     * Scan e interpret keyboard.   *
     ********************************/

    char scan = get_scancode(); //read from keyboard

    //Interpreting keyboard
    switch(scan)
    {
        /*CREATE A NEW SIGNAL*/
        case KEY_ENTER:
        if(n_active_signals < MAX_SIGNALS)
        {
            n_active_signals++;
            task_create(filterTask, n_active_signals-1, FILTER_PERIOD, FILTER_PERIOD, FILTER_PRIO);
        }
        else
            printf("Numero massimo di segnali raggiunto.\n");

        printf("[ENTER] Create a new signal.\n");
        break;

        case KEY_PLUS_PAD:
        signals[n_active_signals-1].frequency += 5;
        //Re-compute Sampling Period
        set_Ts(n_active_signals-1);
        //printSignal(signals[n_active_signals-1]);

        //Clear and reprint

        printf("[PLUS] Increment of 5 Hz last signal's frequency.\n");
        break;

        /*case KEY_MINUS_PAD:
        break;*/

        /*EXIT CONDITION*/
        case KEY_ESC:
        end_flag = 1;
        printf("[ESC] Exit from application...\n");
        break;

        /*DEFAULT CONDITION*/
        default:
        break;        
    }
}

/*void *helloWorldTask(void* arg)
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
}*/

/*void *waveTask(void* arg)
{
    int idx = get_task_index(arg);
    set_activation(idx);

    //Init
    int k = 0;
    double time = 0.0;
    double y = 0.0;

    double y_filtered = 0.0;

    struct Signal signal = {1.0, 30.0, 0.0, ,sinusoid};

    const double Ts = 1/(N_SAMPLE_PERIOD*signal.frequency); //fs = 100*f_signal | 100 samples for period

    //Print Information
    printf("Wave Task Period: %d\n", WAVE_PER);
    printSignal(signal);

    while((!end_flag) && ((WIDTH/XLIM)*time <= WIDTH))
    {
        keyboard_interp(); //Keyboard test


        //Test Filters
        //Filters algorithm needs y(k-1) and x(k-1)
        y_filtered = lowPassFilter(y_filtered, y, 2*PI*10, Ts);

        //COMPUTE SIGNAL
        time = k*Ts;
        y = signalRealization(signal, time);

        //DRAW POINTS
        plotPoint(time, y, YELLOW);
        plotPoint(time, y_filtered, WHITE);

        //Debug information
        //printf("******** Iteration %d **********\n", k);
        //printf("t = \t %f [s]\n", time);
        //printf("y = \t %f\n", y);

        //Successive sample
        k++;

        if(deadline_miss(idx))
            printf("*****Deadline Miss of Wave Task!*******\n");
        
        wait_for_activation(idx);
    }

    return NULL;
}*/

void *filterTask(void* arg)
{
    int idx;
    idx = get_task_index(arg);
    set_activation(idx);

    init_signal(idx);

    //Local Variables
    int k = 0;
    double time = 0.0;
    double y = 0.0;
    //double y_filtered = 0.0;

    while(!end_flag)
    {
        /***********BODY OF TASK**********/
        /*COMPUTE SIGNAL*/
        time = k*signals[idx].Ts;
        y = signalRealization(signals[idx], time);

        /*DRAW POINTS*/
        plotPoint(time, y, YELLOW);

        //Successive sample
        k++;

        //Replot Signal
        if((WIDTH/XLIM)*time > WIDTH)
        {
            //Clear Screen
	        clear_to_color(screen, BLACK); //Black Background
            //Reset Local Variables
            k = 0;
        }
        /*********************************/

        if(deadline_miss(idx))
            printf("******Deadline Miss of Filter Task!******** \n");

        wait_for_activation(idx);        
    }

    return NULL;
}

void *graphicTask(void* arg)
{
    int idx;
    idx = get_task_index(arg);
    set_activation(idx);

    while(!end_flag)
    {
        /***********BODY OF TASK**********/
        printf("Graphic Task!\n");


        /*********************************/
        if(deadline_miss(idx))
            printf("******Deadline Miss of Graphic Task!******** \n");

        wait_for_activation(idx);        
    }

    return NULL;
}

void *userTask(void* arg)
{
    int idx;
    idx = get_task_index(arg);
    set_activation(idx);

    while(!end_flag)
    {
        /***********BODY OF TASK**********/
        keyboard_interp();
        /*********************************/

        if(deadline_miss(idx))
            printf("******Deadline Miss of User Task!******** \n");

        wait_for_activation(idx);        
    }

    return NULL;
}

