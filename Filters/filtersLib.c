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

/*SEMAPHORES: Mutex*/
pthread_mutex_t mux_signal = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t mux_filter = PTHREAD_MUTEX_INITIALIZER;

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

double highPassFilter()
{
    /************************************************
     *  HIGH PASS FILTER:                           *
     * This function compute the discrete time form *
     * of transfer function G(s):                   *
     *              s                               *
     * G(s) =  -----------                          *
     *          s   +   a                           *      
     *                                              *    
     * a = omega_cut = 2*PI*f_cut                   *
     ************************************************/

    return 0.0;
}

void plotPoint(double time, double y, int color)
{
    //scaled_time = (WIDTH/XLIM)*time;
    //scaled_y = (HEIGHT/2) + (HEIGHT/2 - 1)*y;

    //Rescale & Plot
    putpixel(screen, (WIDTH/XLIM)*time, (HEIGHT/2) + (HEIGHT/2 - 1)*y, color);
}

void signalRealization(int idx)
{
    signals[idx].t = signals[idx].k*signals[idx].Ts;
    // Update signal output
    switch(signals[idx].signal_type)
    {
        case sinusoid:
        // y(k) = A sin(wt + phi)
        signals[idx].y = signals[idx].amplitude*sin(2*PI*signals[idx].frequency*signals[idx].t + signals[idx].phase);
        break;

        case square:
        // y(k) = A sgn(sin(wt + phi))
        signals[idx].y = signals[idx].amplitude*sign(sin(2*PI*signals[idx].frequency*signals[idx].t + signals[idx].phase));
        break;

        case sawtooth:
        //y = - (2*signal.amplitude/PI) * atan(1/(tan(PI*signal.frequency*signal.t + signal.phase)));
        signals[idx].y = (2*signals[idx].amplitude/PI) * atan(1/(tan(PI*signals[idx].frequency*signals[idx].t + signals[idx].phase)));
        break;

        case triang:
        signals[idx].y = (2*signals[idx].amplitude/PI) * asin(sin(2*PI*signals[idx].frequency*signals[idx].t + signals[idx].phase));
        break;

        default:
        signals[idx].y = 0.0;
        printf("Singal Type not valid. Output setted to zero!\n");
        break;
    }

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
    printf("* Ts: \t \t %f [s]\n", signal.Ts);
    printf("***************************************\n");
}

void init()
{
    allegro_init();
    install_keyboard();

    set_color_depth(8); //256 VGA

    srand(time(0)); //Init random numbers

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
        signals[idx].Ts = 0.01;
}

void init_signal(int idx)
{
    /****************************************************
     * INIT SIGNAL: Define initial parameter of signal. *
     ****************************************************/
    //Init general attributes
    signals[idx].amplitude =    frand(0.1, 1.0);
    signals[idx].frequency =    frand(FREQ_MIN, FREQ_MAX);  //[Hz]
    signals[idx].phase =        frand(0, 2*PI);             //[rad]
    signals[idx].signal_type =  floor(frand(sinusoid, triang));

    //Init discrete time parameters
    signals[idx].k = 0;
    set_Ts(idx);

    //Graphic Parameters
    signals[idx].t = 0.0;
    signals[idx].y = 0.0;
    signals[idx].color = floor(frand(WHITE, BLACK));
}

void init_filter(int idx)
{
    /****************************************************
     * INIT FILTER: Define initial parameter of filter. *
     ****************************************************/
    //Init general attributes
    filters[idx].gain = 1.0;
    filters[idx].f_cut = (1.0/3.0)*signals[idx].frequency;  //[Hz]
    filters[idx].filter_type = LOW_PASS;

    //Graphic Parameters
    filters[idx].y_filterd = 0.0;
    filters[idx].color = floor(frand(WHITE, BLACK));
}

void clear_reset(int idx)
{
    pthread_mutex_lock(&mux_signal);
    //Clear Screen
    clear_to_color(screen, BLACK); //Black Background
    //Reset Plot
    signals[idx].k = 0;
    pthread_mutex_unlock(&mux_signal);
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

        /*INCREASE/DECREASE FREQUENCY*/
        case KEY_PLUS_PAD:
        if(signals[n_active_signals-1].frequency + 5 <= FREQ_MAX)
        {
            signals[n_active_signals-1].frequency += 5;
            //Re-compute Sampling Period
            set_Ts(n_active_signals-1);
            printSignal(signals[n_active_signals-1]);

            //Clear and reprint
            clear_reset(n_active_signals-1);
            printf("[PLUS] Increment of 5 Hz last signal's frequency.\n");
        }
        
        else
        {
            if(signals[n_active_signals-1].frequency == FREQ_MAX)
                printf("[PLUS] Increment of signal's frequency not permitted: Maximum reached.\n");

            else
            {
                signals[n_active_signals-1].frequency = FREQ_MAX;
                //Re-compute Sampling Period
                set_Ts(n_active_signals-1);
                printSignal(signals[n_active_signals-1]);

                //Clear and reprint
                clear_reset(n_active_signals-1);
            }
                
        }
        break;

        case KEY_MINUS_PAD:
        if(signals[n_active_signals-1].frequency - 5 >= FREQ_MIN)
        {
            signals[n_active_signals-1].frequency -= 5;
            //Re-compute Sampling Period
            set_Ts(n_active_signals-1);
            printSignal(signals[n_active_signals-1]);

            //Clear and reprint
            clear_reset(n_active_signals-1);
            printf("[MINUS] Decrement of 5 Hz last signal's frequency.\n");
        }
        
        else
        {
            if(signals[n_active_signals-1].frequency == FREQ_MIN)
                printf("[MINUS] Decrement of signal's frequency not permitted: Minimum reached.\n");

            else
            {
                signals[n_active_signals-1].frequency = FREQ_MIN;
                //Re-compute Sampling Period
                set_Ts(n_active_signals-1);
                printSignal(signals[n_active_signals-1]);

                //Clear and reprint
                clear_reset(n_active_signals-1);
            }
                
        }
        break;

        /*EXIT CONDITION*/
        case KEY_ESC:
        end_flag = 1;
        printf("[ESC] Exit from application...\n");
        break;

        case KEY_SPACE:
        end_flag = 1;
        printf("[SPACE] Exit from application...\n");
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
    init_filter(idx);
    printSignal(signals[idx]);

    while(!end_flag)
    {
        pthread_mutex_lock(&mux_signal);

        /***********BODY OF TASK**********/
        //Filters algorithm needs y(k-1) and x(k-1)
        filters[idx].y_filterd = lowPassFilter(filters[idx].y_filterd, signals[idx].y, 2.0*PI*(filters[idx].f_cut), signals[idx].Ts);

        /*COMPUTE SIGNAL*/
        signalRealization(idx);

        //Successive sample
        signals[idx].k++;
        
        pthread_mutex_unlock(&mux_signal);

        //Replot Signal when it reaches XLIM
        if((WIDTH/XLIM)*signals[idx].t > WIDTH)
            clear_reset(idx);
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
        pthread_mutex_lock(&mux_signal);
        /***********BODY OF TASK**********/
        plotPoint(signals[n_active_signals-1].t, signals[n_active_signals-1].y, signals[n_active_signals-1].color);
        plotPoint(signals[n_active_signals-1].t, filters[n_active_signals-1].y_filterd, filters[n_active_signals-1].color);
        /*********************************/
        pthread_mutex_unlock(&mux_signal);

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

