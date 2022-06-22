/*INCLUDE*/
//Standard Libraries and Pthred libraries
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
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

//Clear and Reset request
int clear_request = 0;  // If 1, graphic task clears the screen
int clear_fft = 0;      // If 1, graphic task clears fft screen

//FFT enable and request
int fft_enable = 0; //If 0, no FFT
int fft_request = 0; //If 1, plot FFT

//plot preferences
enum Plot_Style plot_style = POINT; //Default: POINT

/*SEMAPHORES: Mutex*/
pthread_mutex_t mux_signal = PTHREAD_MUTEX_INITIALIZER;

/*FUNCTIONS*/
double sign(double x)
{
     /*SIGN FUNCTION:*/

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

double highPassFilter(double x_k, double x_k_1, double y_k_1, double a, double Ts)
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

    double p = exp(-a*Ts);
    double y_k = (x_k - x_k_1) + p*y_k_1;
    return y_k;
}

double bandPassFilter(double x_k_1, double x_k_2, double y_k_1, double y_k_2, double a1, double a2, double Ts)
{
    /************************************************
     *  BAND PASS FILTER:                           *
     * This function compute the discrete time form *
     * of transfer function G(s):                   *
     *              a1              s               *
     * G(s) =  -----------  x  ------------         *
     *          s   +   a1      s   +   a2          *      
     *                                              *    
     * a_i = omega_cut = 2*PI*f_cut_i               *
     ************************************************/

    //Note: a1 > a2
    //To do: recompute algorithm

    double A = a1/(a2-a1);
    double p1 = exp(-a1*Ts);
    double p2 = exp(-a2*Ts);

    double y_k = (p1 + p2)*y_k_1 - (p1*p2)*y_k_2 + A*(p2-p1)*(x_k_2 - x_k_1);
    return y_k; 
}

void plotPoint(BITMAP* window, double time, double y, int color)
{
    //scaled_time = (WIDTH/XLIM)*time;
    //scaled_y = (HEIGHT/2) + (HEIGHT/2 - 1)*y;

    int width = window->w;
    int height = window->h;

    //Rescale & Plot
    putpixel(window, (width/XLIM)*time, (height/2) - (height/2 - 1)*y, color);
}

void plotLin(BITMAP* window, double time_k, double time_k_1, double y_k, double y_k_1, int color)
{
    //scaled_time = (WIDTH/XLIM)*time;
    //scaled_y = (HEIGHT/2) + (HEIGHT/2 - 1)*y;

    int width = window->w;
    int height = window->h;

    //Rescale & Plot
    line(window, (width/XLIM)*time_k_1, (height/2) - ((height/2 - 1)/YLIM)*y_k_1, (width/XLIM)*time_k, (height/2) - ((height/2 - 1)/YLIM)*y_k, color);
}

void signalRealization()
{
    //Compute time
    input_signal.t = input_signal.k*input_signal.Ts;

    //Update signal's output vector
    int i;
    for(i = MAX_ORDER; i > 0; i--)
    {
        //y(k-MAX_ORDER) = y(k-MAX_ORDER+1)
        //...
        //y(k-2) = y(k-1)
        //y(k-1) = y(k)

        input_signal.y[i] = input_signal.y[i-1];
    }

    // Update signal output
    switch(input_signal.signal_type)
    {
        case sinusoid:
        // y(k) = A sin(wt + phi)
        input_signal.y[0] = input_signal.amplitude*sin(2.0*PI*input_signal.frequency*input_signal.t + input_signal.phase);
        break;

        case square:
        // y(k) = A sgn(sin(wt + phi))
        input_signal.y[0] = input_signal.amplitude*sign(sin(2.0*PI*input_signal.frequency*input_signal.t + input_signal.phase));
        break;

        case sawtooth:
        input_signal.y[0] = (2.0*input_signal.amplitude/PI) * atan(1.0/(tan(PI*input_signal.frequency*input_signal.t + input_signal.phase)));
        break;

        case triang:
        input_signal.y[0] = (2.0*input_signal.amplitude/PI) * asin(sin(2.0*PI*input_signal.frequency*input_signal.t + input_signal.phase));
        break;

        default:
        input_signal.y[0] = 0.0;
        printf("Singal Type not valid. Output setted to zero!\n");
        break;
    }

}

void filterRealization(struct Signal signal, int idx)
{
    //Update filter's output vector
    int i;
    for(i = MAX_ORDER; i > 0; i--)
    {
        //y(k-MAX_ORDER) = y(k-MAX_ORDER+1)
        //...
        //y(k-2) = y(k-1)
        //y(k-1) = y(k)

        filters[idx].y_filtered[i] = filters[idx].y_filtered[i-1];
    }

    switch(filters[idx].filter_type)
    {
        case LOW_PASS:
        //Filters algorithm needs y(k-1) and x(k-1)
        filters[idx].y_filtered[0] = lowPassFilter(filters[idx].y_filtered[0], signal.y[1], 2.0*PI*(filters[idx].f_cut), signal.Ts);
        break;

        case HIGH_PASS:
        //Filters algorithm needs y(k-1), x(k) and x(k-1)
        filters[idx].y_filtered[0] = highPassFilter(signal.y[0], signal.y[1], filters[idx].y_filtered[0], 2.0*PI*(filters[idx].f_cut), signal.Ts);
        break;

        case BAND_PASS:
        //Filters algorithm needs y(k-1), y(k-2), x(k-1) and x(k-2)
        // a1 : f_low | a2 : f_high
        filters[idx].y_filtered[0] = bandPassFilter(signal.y[1], signal.y[2], filters[idx].y_filtered[1], filters[idx].y_filtered[2], 2.0*PI*(3.0*signal.frequency), 2.0*PI*(signal.frequency/3.0), signal.Ts);
        break;

        default:
        filters[idx].y_filtered[0] = 0.0;
        printf("Filter Type not valid. Output setted to zero!\n");
        break;
    }
    
}

void fftRealization()
{
    int i;

    if(input_signal.k < N_SAMPLE_PERIOD)
    {
        //Store Data for FFT 
        input_signal.fftData[input_signal.k] = input_signal.y[0];

        for(i = 0; i < n_active_filters; i++)
        {
            filters[i].fftData[input_signal.k] = filters[i].y_filtered[0];
        }
    }

    if(input_signal.k == N_SAMPLE_PERIOD)
    {
        //Maybe this can be removed
        //fill data vector with (2^N - N_SAMPLE_PERIOD) idx with zeros
        /*for(i = N_SAMPLE_PERIOD; i < FFT_DATA; i++)
        {
            input_signal.fftData[i] = 0.0;
        }*/

        fft(input_signal.fftData, FFT_DATA);

        for(i = 0; i < n_active_filters; i++)
        {
            fft(filters[i].fftData, FFT_DATA);
        }

        //Compute Magnitude of i-th Complex Number
        int j;
        for(i = 0; i < FFT_DATA; i++)
        {
            input_signal.fftData[i] = (1.0/N_SAMPLE_PERIOD)*sqrt(pow(creal(input_signal.fftData[i]), 2.0) +  pow(cimag(input_signal.fftData[i]), 2.0));

            for(j = 0; j < n_active_filters; j++)
            {
                filters[j].fftData[i] = (1.0/N_SAMPLE_PERIOD)*sqrt(pow(creal(filters[j].fftData[i]), 2.0) +  pow(cimag(filters[j].fftData[i]), 2.0));
            }
        }

        //P1 = signal_fftData(0:N_SAMPLE_PERIOD/2) -> dim (N_SAMPLE_PERIOD/2) + 1
        //P1[1 : N_SAMPLE_PERIOD/2 - 1] = 2*P1[1 : N_SAMPLE_PERIOD/2 -1]
        for(i = 1; i < N_SAMPLE_PERIOD/2; i++)
        {
            input_signal.fftData[i] *= 2.0;

            for(j = 0; j < n_active_filters; j++)
            {
                filters[j].fftData[i] *= 2.0;
            }
        }

        //fft_request: Enable plotting fft.
        fft_request = 1;
    }
}

void printSignal(struct Signal signal)
{
    printf("********PRINT SIGNAL INFORMATION********\n");

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

void printFilter(struct Filter filter)
{
    printf("********PRINT FILTER INFORMATION********\n");

    switch(filter.filter_type)
    {
        case LOW_PASS:
        printf("* Filter Type: \t Low Pass\n");
        break;

        case HIGH_PASS:
        printf("* Filter Type: \t High Pass\n");
        break;

        case BAND_PASS:
        printf("* Filter Type: \t Band Pass\n");
        break;

        default:
        printf("* Filter Type not valid.\n");
        break;
    }

    printf("* Cut Freq:  \t %f [Hz]\n", filter.f_cut);
    printf("* Gain: \t %f\n", filter.gain);
    printf("***************************************\n");
}

void _fft(cplx buf[], cplx out[], int n, int step)
{
	if (step < n) {
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);
 
		for (int i = 0; i < n; i += 2 * step) {
			cplx t = cexp(-I * PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}
 
void fft(cplx buf[], int n)
{
	cplx out[n];
	for (int i = 0; i < n; i++) out[i] = buf[i];
 
	_fft(buf, out, n, 1);
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

void set_Ts()
{
    if(input_signal.frequency != 0)
    {
        //fs = 100*f_signal | 100 samples for period
        input_signal.Ts = 1/(N_SAMPLE_PERIOD*input_signal.frequency); 
    }

    else
        input_signal.Ts = 0.01;
}

void init_signal()
{
    /****************************************************
     * INIT SIGNAL: Define initial parameter of signal. *
     ****************************************************/
    //Init general attributes
    input_signal.amplitude =    frand(0.5, 0.8);
    input_signal.frequency =    frand(FREQ_MIN, FREQ_MAX);  //[Hz]
    input_signal.phase =        frand(0, 2*PI);             //[rad]
    input_signal.signal_type =  floor(frand(sinusoid, triang));

    //Init discrete time parameters
    input_signal.k = 0;
    set_Ts();

    //Graphic Parameters
    input_signal.t = 0.0;

    //Init signal realization
    int i;
    for(i = 0; i <= MAX_ORDER; i++)
    {
        input_signal.y[i] = 0.0;
    }

    for(i = 0; i < FFT_DATA; i++)
    {
        input_signal.fftData[i] = 0.0;
    }
    
    input_signal.color = floor(frand(BLACK, WHITE));
}

void init_filter(int idx)
{
    /****************************************************
     * INIT FILTER: Define initial parameter of filter. *
     ****************************************************/
    //Init general attributes
    filters[idx].gain = 1.0;
    filters[idx].f_cut = (1.0/3.0)*input_signal.frequency;  //[Hz]
    filters[idx].filter_type = floor(frand(LOW_PASS, BAND_PASS));

    //Graphic Parameters
    filters[idx].color = floor(frand(BLACK, WHITE));

    //Init signal realization
    int i;
    for(i = 0; i <= MAX_ORDER; i++)
    {
        filters[idx].y_filtered[i] = 0.0;
    }

    for(i = 0; i < FFT_DATA; i++)
    {
        filters[idx].fftData[i] = 0.0;
    }
}

void clear_reset(BITMAP* window)
{
    //Clear Screen
    clear_to_color(window, BLACK); //Black Background
    //Reset Plot
    input_signal.k = 0.0;
    input_signal.t = 0.0;

    //Init signal and filters realization
    int i, j;
    for(i = 0; i < n_active_filters; i++)
    {
        for(j = 0; j <= MAX_ORDER; j++)
        {
            input_signal.y[j] = 0.0;
            filters[i].y_filtered[j] = 0.0;
        }
    }
}

void clear_resetIdx(BITMAP* window, int idx)
{
    //Clear Screen
    clear_to_color(window, BLACK); //Black Background
    //Reset Plot
    input_signal.k = 0.0;
    input_signal.t = 0.0;

    //Init signal realization
    int i;
    for(i = 0; i <= MAX_ORDER; i++)
    {
        input_signal.y[i] = 0.0;
        filters[idx].y_filtered[i] = 0.0;
    }
}

void draw_oscilloscope(BITMAP* osc, BITMAP* window)
{
    //Extract dimension of plot
    int osc_width = osc->w;
    int osc_height = osc->h;

    /*DRAW PLOT GRAPHICS*/
    rect(osc, 1, osc_height-1, osc_width-1, 0, WHITE);
    line(osc, 0, osc_height/2, osc_width, osc_height/2, LIGHT_GRAY);                                //time axis
    //textout_ex(osc, font, "0", 5, osc_height/2 -10, LIGHT_GRAY, -1);                                //origin of plot
    textout_ex(osc, font, "Amp", 5, 5, LIGHT_GRAY, -1);                                             //amplitude label  
    textout_centre_ex(osc, font, "time [s]", osc_width - 40, osc_height/2 - 10, LIGHT_GRAY, -1);    //time label

    //Grid
    int i;
    char s_grid[MAX_CHAR];
    for(i = 0; i <= 10; i ++)
    {
        if(i == 0)
            sprintf(s_grid, "  %d", i);
        else
            sprintf(s_grid, "%5.2f", ((float) i)*XLIM/10.0);

        if(i == 10)
            textout_centre_ex(osc, font, s_grid, osc_width*((float) i)/10.0 - 20, osc_height/2 + 10, LIGHT_GRAY, -1);
        else
            textout_centre_ex(osc, font, s_grid, osc_width*((float) i)/10.0, osc_height/2 + 10, LIGHT_GRAY, -1);
    }

    for(i = 1; i < 10; i++)
    {
        sprintf(s_grid, "%5.1f", 1.0 - ((float) i)/10.0);
        textout_centre_ex(osc, font, s_grid, 7, (osc_height/2 - 1)*((float) i)/10.0, LIGHT_GRAY, -1);
    }

    /*PLOT SIGNAL AND FILTERS*/
    if(n_active_filters > 0)
    {
        //int i;

        //Plot signal and filter points
        switch (plot_style)
        {
            case POINT:
            plotPoint(osc, input_signal.t, input_signal.y[0], input_signal.color);

            //Plot every filters
            for(i = 0; i < n_active_filters; i++)
            {
                plotPoint(osc, input_signal.t, filters[i].y_filtered[0], filters[i].color);
            }
            break;

            case INTERP_LIN:
            plotLin(osc, input_signal.t, input_signal.t - input_signal.Ts, input_signal.y[0], input_signal.y[1], input_signal.color);

            //Plot every filters
            for(i = 0; i < n_active_filters; i++)
            {
                plotLin(osc, input_signal.t, input_signal.t - input_signal.Ts, filters[i].y_filtered[0], filters[i].y_filtered[1], filters[i].color);
            }
            break;
            
            default:
            break;
        }

        //Replot Signal when it reaches XLIM OR if there is a clear request
        if(((osc_width/XLIM)*input_signal.t > osc_width) || clear_request)
        {
            //Clear every filter
            clear_reset(osc);
            
            //Reset clear_request to zero
            if(clear_request)
                clear_request = 0;
                
            clear_fft = 1; //not optimized
        }
    }

    blit(osc, window, 0, 0, 0, INFO_HEIGHT, osc_width, osc_height); //finally, copies in original screen
}

void draw_information(BITMAP* info, BITMAP* window)
{
    //String to print
    char s_freq[MAX_CHAR];
    char s_phase[MAX_CHAR];
    char s_amp[MAX_CHAR];

    char s_fcut[MAX_CHAR];
    char s_gain[MAX_CHAR];
    char s_Ts[MAX_CHAR];

    sprintf(s_freq, "Frequency: %5.2f [Hz]", input_signal.frequency);
    sprintf(s_phase, "Phase: %5.2f [rad]", input_signal.phase);
    sprintf(s_amp, "Amplitude: %5.2f", input_signal.amplitude);

    sprintf(s_fcut, "Cut Frequency: %5.2f [Hz]", filters[n_active_filters-1].f_cut);
    sprintf(s_gain, "Gain: %5.2f", filters[n_active_filters-1].gain);
    sprintf(s_Ts, "Sampling Period: %5.2f [ms]", input_signal.Ts*1000.0); //Print sampling period [ms]

    //A bit not-optimized, sorry future Daniele
    clear_to_color(info, BLACK);

    //Init msg
    if(n_active_filters == 0)
    {
        //Title
        textout_ex(info, font, "FILTERS APPLICATION!", TITLE_WIDTH, TITLE_HEIGHT, RED, -1);
        textout_ex(info, font, "Press [ENTER] to init.", TITLE_WIDTH + 200, TITLE_HEIGHT, WHITE, -1);
    }
        
    else
    {
        //First Box
        rect(info, COMMAND_WIDTH -2, 10-2, COMMAND_WIDTH + 200, 60, LIGHT_GRAY);
        textout_ex(info, font, "[ENTER]: Add a filter", COMMAND_WIDTH, 10, WHITE, -1);
        textout_ex(info, font, "[SPACE]/[ESC]: close", COMMAND_WIDTH, 20, WHITE, -1);
        textout_ex(info, font, "[C]: Clean plot", COMMAND_WIDTH, 30, WHITE, -1);
        textout_ex(info, font, "[+]/[-]: +/- 5 Hz", COMMAND_WIDTH, 40, WHITE, -1);
        textout_ex(info, font, "[S]: Change plot style", COMMAND_WIDTH, 50, WHITE, -1);

        //Second Box
        rect(info, BOX_2 -2, 10-2, BOX_2 + 200, 60, LIGHT_GRAY);
        textout_ex(info, font, "[1]: Sinusoidal", BOX_2, 10, WHITE, -1);
        textout_ex(info, font, "[2]: Square", BOX_2, 20, WHITE, -1);
        textout_ex(info, font, "[3]: Sawtooth", BOX_2, 30, WHITE, -1);
        textout_ex(info, font, "[4]: Triangular", BOX_2, 40, WHITE, -1);
        textout_ex(info, font, "[Y]/[W]: Change color", BOX_2, 50, WHITE, -1);

        //Second Box
        rect(info, BOX_3 -2, 10-2, BOX_3 + 170, 60, LIGHT_GRAY);
        textout_ex(info, font, "[5]: Low Pass", BOX_3, 10, WHITE, -1);
        textout_ex(info, font, "[6]: High Pass", BOX_3, 20, WHITE, -1);
        textout_ex(info, font, "[7]: Band Pass", BOX_3, 30, WHITE, -1);
        textout_ex(info, font, "[F]: FFT", BOX_3, 40, WHITE, -1);
    }

    //Signal Information
    textout_ex(info, font, "Signal Information:", INFO_SIGNAL_WIDTH, 10, YELLOW, -1);
    textout_ex(info, font, s_freq, INFO_SIGNAL_WIDTH, 20, WHITE, -1);
    textout_ex(info, font, s_phase, INFO_SIGNAL_WIDTH, 30, WHITE, -1);
    textout_ex(info, font, s_amp, INFO_SIGNAL_WIDTH, 40, WHITE, -1);

    if(n_active_filters == 0)
        textout_ex(info, font, "Signal Type: Unkown", INFO_SIGNAL_WIDTH, 50, WHITE, -1);
    else
    {
        switch(input_signal.signal_type)
        {
            case sinusoid:
            textout_ex(info, font, "Signal Type: Sinusoid", INFO_SIGNAL_WIDTH, 50, WHITE, -1);
            break;

            case square:
            textout_ex(info, font, "Signal Type: Square", INFO_SIGNAL_WIDTH, 50, WHITE, -1);
            break;

            case sawtooth:
            textout_ex(info, font, "Signal Type: Sawtooth", INFO_SIGNAL_WIDTH, 50, WHITE, -1);
            break;

            case triang:
            textout_ex(info, font, "Signal Type: Triangular", INFO_SIGNAL_WIDTH, 50, WHITE, -1);
            break;

            default:
            textout_ex(info, font, "Signal Type: Unkown", INFO_SIGNAL_WIDTH, 50, WHITE, -1);
            break;
        }
    }
    
    //Filter Information
    textout_ex(info, font, "Filter Information: (Last)", INFO_FILTER_WIDTH, 10, CYAN, -1);
    textout_ex(info, font, s_fcut, INFO_FILTER_WIDTH, 20, WHITE, -1);
    textout_ex(info, font, s_gain, INFO_FILTER_WIDTH, 30, WHITE, -1);
    textout_ex(info, font, s_Ts, INFO_FILTER_WIDTH, 40, WHITE, -1);

    if(n_active_filters == 0)
        textout_ex(info, font, "Filter Type: Unkown", INFO_FILTER_WIDTH, 50, WHITE, -1);

    else
    {
        switch(filters[n_active_filters-1].filter_type)
        {
            case LOW_PASS:
            textout_ex(info, font, "Filter Type: Low Pass", INFO_FILTER_WIDTH, 50, WHITE, -1);
            break;

            case HIGH_PASS:
            textout_ex(info, font, "Filter Type: High Pass", INFO_FILTER_WIDTH, 50, WHITE, -1);
            break;

            case BAND_PASS:
            textout_ex(info, font, "Filter Type: Band Pass", INFO_FILTER_WIDTH, 50, WHITE, -1);
            break;

            default:
            textout_ex(info, font, "Filter Type: Unkown", INFO_FILTER_WIDTH, 50, WHITE, -1);
            break;
        }
    }

    blit(info, window, 0, 0, 0, 0, info->w, info->h); //finally, copies in original screen
}

void draw_fft(BITMAP* fft_bitmap, BITMAP* window)
{
    //Extract dimension of plot
    int fft_width = fft_bitmap->w;
    int fft_height = fft_bitmap->h;

    /*DRAW PLOT GRAPHICS*/
    rect(fft_bitmap, 1, fft_height-1, fft_width-1, 0, WHITE);               //box
    line(fft_bitmap, 0, fft_height/2, fft_width, fft_height/2, LIGHT_GRAY); //frequency axis
    textout_ex(fft_bitmap, font, "Mag", 5, 5, LIGHT_GRAY, -1);                                             //amplitude label  
    textout_centre_ex(fft_bitmap, font, "Freq [Hz]", fft_width - 40, fft_height/2 - 10, LIGHT_GRAY, -1);    //frequency label

    /*GRID*/
    int i, j;
    if(n_active_filters > 0)
    {
        char s_grid[MAX_CHAR];
        for(i = 0; i <= 10; i ++)
        {
            if(i == 0)
                sprintf(s_grid, "  %d", i);
            else
                sprintf(s_grid, "%5.1f", ((float) i)*(1.0/(2.0*input_signal.Ts))/10.0);

            if(i == 10)
                textout_centre_ex(fft_bitmap, font, s_grid, fft_width*((float) i)/10.0 - 20, fft_height/2 + 10, LIGHT_GRAY, -1);
            else
                textout_centre_ex(fft_bitmap, font, s_grid, fft_width*((float) i)/10.0, fft_height/2 + 10, LIGHT_GRAY, -1);
        }

        for(i = 1; i < 10; i++)
        {
            sprintf(s_grid, "%5.1f", 1.0 - ((float) i)/10.0);
            textout_centre_ex(fft_bitmap, font, s_grid, 7, (fft_height/2 - 1)*((float) i)/10.0, LIGHT_GRAY, -1);
        }
    }

    //Plot fft
    if(fft_request)
    {
        switch(plot_style)
        {
            case POINT:
            for(i = 0; i <= N_SAMPLE_PERIOD/2; i++)
            {
                putpixel(fft_bitmap, (fft_width*(2.0*input_signal.Ts))*(i/(input_signal.Ts*N_SAMPLE_PERIOD)), (fft_height/2) - (fft_height/2 - 1)*(creal(input_signal.fftData[i])), input_signal.color);

                for(j = 0; j < n_active_filters; j++)
                {
                    putpixel(fft_bitmap, (fft_width*(2.0*input_signal.Ts))*(i/(input_signal.Ts*N_SAMPLE_PERIOD)), (fft_height/2) - (fft_height/2 - 1)*(creal(filters[j].fftData[i])), filters[j].color);
                }
            }
            break;

            case INTERP_LIN:
            for(i = 0; i < N_SAMPLE_PERIOD/2; i++)
            {
                line(fft_bitmap, (fft_width*(2.0*input_signal.Ts))*((i-1)/(input_signal.Ts*N_SAMPLE_PERIOD)), (fft_height/2) - (fft_height/2 - 1)*(creal(input_signal.fftData[i-1])), (fft_width*(2.0*input_signal.Ts))*(i/(input_signal.Ts*N_SAMPLE_PERIOD)), (fft_height/2) - (fft_height/2 - 1)*(creal(input_signal.fftData[i])), input_signal.color);

                for(j = 0; j < n_active_filters; j++)
                {
                    line(fft_bitmap, (fft_width*(2.0*input_signal.Ts))*((i-1)/(input_signal.Ts*N_SAMPLE_PERIOD)), (fft_height/2) - (fft_height/2 - 1)*(creal(filters[j].fftData[i-1])), (fft_width*(2.0*input_signal.Ts))*(i/(input_signal.Ts*N_SAMPLE_PERIOD)), (fft_height/2) - (fft_height/2 - 1)*(creal(filters[j].fftData[i])), filters[j].color);
                }
            }
            break;

            default:
            break;
        }

        //Line o highlight main frequency
        line(fft_bitmap, (fft_width*(2.0*input_signal.Ts))*input_signal.frequency, (fft_height/2), (fft_width*(2.0*input_signal.Ts))*input_signal.frequency, (fft_height/2) - (fft_height/2 - 1)*(0.05), LIGHT_GRAY);
        
        //print main frequency
        char s_main_freq[6];
        sprintf(s_main_freq, "%5.1f", input_signal.frequency);
        textout_centre_ex(fft_bitmap, font, s_main_freq, (fft_width*(2.0*input_signal.Ts))*input_signal.frequency, (fft_height/2) - (fft_height/2 - 1)*(-0.05), YELLOW, -1);

        fft_request = 0;
    }

    //Clear fft bitmap
    if(clear_fft)
    {
        clear_to_color(fft_bitmap, BLACK);

        //Re-init fft data array
        for(i = 0; i < FFT_DATA; i++)
        {
            input_signal.fftData[i] = 0.0;

            for(j = 0; j < n_active_filters; j++)
            {
                filters[j].fftData[i] = 0.0;
            }
        }

        clear_fft = 0;
    }

    blit(fft_bitmap, window, 0, 0, 0, INFO_HEIGHT + (HEIGHT-INFO_HEIGHT)/2, fft_width, fft_height); //finally, copies in original screen
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
        //Clear plot and plot new filter
        if(n_active_filters < MAX_FILTERS)
        {
            if(n_active_filters == 0)
            {
                //Init here and not in signalTask
                init_signal();  
                task_create(signalTask, SIGNAL_IDX, SIGNAL_PERIOD, SIGNAL_PERIOD, SIGNAL_PRIO);
            }

            else
            {
                clear_request = 1;
            }

            n_active_filters++;
            task_create(filterTask, n_active_filters-1, FILTER_PERIOD, FILTER_PERIOD, FILTER_PRIO);
        }
        else
            printf("Maximum number of filters reached.\n");

        printf("[ENTER] Create a new filter.\n");
        break;

        /*INCREASE/DECREASE FREQUENCY*/
        case KEY_PLUS_PAD:
        pthread_mutex_lock(&mux_signal);
        if(input_signal.frequency + 5 <= FREQ_MAX)
        {
            input_signal.frequency += 5;
            //Re-compute Sampling Period
            set_Ts();
            printSignal(input_signal);
            //Re-compute Cut Frequency
            filters[n_active_filters-1].f_cut = (1.0/3.0)*input_signal.frequency;
            printFilter(filters[n_active_filters-1]);

            //Clear and reprint
            clear_request = 1;
            printf("[PLUS] Increment of 5 Hz input signal's frequency.\n");
        }
        
        else
        {
            if(input_signal.frequency == FREQ_MAX)
                printf("[PLUS] Increment of signal's frequency not permitted: Maximum reached.\n");

            else
            {
                input_signal.frequency = FREQ_MAX;
                //Re-compute Sampling Period
                set_Ts();
                printSignal(input_signal);
                //Re-compute Cut Frequency
                filters[n_active_filters-1].f_cut = (1.0/3.0)*input_signal.frequency;
                printFilter(filters[n_active_filters-1]);

                //Clear and reprint
                clear_request = 1;
                printf("[PLUS] Setting signal's frequency to maximum frequency.\n");
            }
                
        }
        pthread_mutex_unlock(&mux_signal);
        break;

        case KEY_MINUS_PAD:
        pthread_mutex_lock(&mux_signal);
        if(input_signal.frequency - 5 >= FREQ_MIN)
        {
            input_signal.frequency -= 5;
            //Re-compute Sampling Period
            set_Ts();
            printSignal(input_signal);
            //Re-compute Cut Frequency
            filters[n_active_filters-1].f_cut = (1.0/3.0)*input_signal.frequency;
            printFilter(filters[n_active_filters-1]);

            //Clear and reprint
            clear_request = 1;
            printf("[MINUS] Decrement of 5 Hz last signal's frequency.\n");
        }
        
        else
        {
            if(input_signal.frequency == FREQ_MIN)
                printf("[MINUS] Decrement of signal's frequency not permitted: Minimum reached.\n");

            else
            {
                input_signal.frequency = FREQ_MIN;
                //Re-compute Sampling Period
                set_Ts();
                printSignal(input_signal);
                //Re-compute Cut Frequency
                filters[n_active_filters-1].f_cut = (1.0/3.0)*input_signal.frequency;
                printFilter(filters[n_active_filters-1]);

                //Clear and reprint
                clear_request = 1;
            }
                
        }
        pthread_mutex_unlock(&mux_signal);
        break;

        /*CHANGE PLOT STYLE*/
        case KEY_S:
        if(plot_style == INTERP_LIN)
            plot_style = POINT;
        else
            plot_style = INTERP_LIN;

        clear_request = 1;
        printf("[S] Change plot style.\n");
        break;

        /*CHANGE SIGNAL TYPE*/
        //Sinusoidal
        case KEY_1:
        printf("[1] Change signal type in Sinusoidal.\n");
        pthread_mutex_lock(&mux_signal);
        if(input_signal.signal_type != sinusoid)
        {
            input_signal.signal_type = sinusoid;
            clear_request = 1; //re-plot signal
        }
        pthread_mutex_unlock(&mux_signal);
        break;

        //Square
        case KEY_2:
        printf("[2] Change signal type in Square.\n");
        pthread_mutex_lock(&mux_signal);
        if(input_signal.signal_type != square)
        {
            input_signal.signal_type = square;
            clear_request = 1; //re-plot signal
        }
        pthread_mutex_unlock(&mux_signal);
        break;

        //Sawtooth
        case KEY_3:
        printf("[3] Change signal type in Sawtooth.\n");
        pthread_mutex_lock(&mux_signal);
        if(input_signal.signal_type != sawtooth)
        {
            input_signal.signal_type = sawtooth;
            clear_request = 1; //re-plot signal
        }
        pthread_mutex_unlock(&mux_signal);
        break;

        //Triangular
        case KEY_4:
        printf("[4] Change signal type in Triangular.\n");
        pthread_mutex_lock(&mux_signal);
        if(input_signal.signal_type != triang)
        {
            input_signal.signal_type = triang;
            clear_request = 1; //re-plot signal
        }
        pthread_mutex_unlock(&mux_signal);
        break;

        /*CHANGE FILTER TYPE*/
        //Low Pass
        case KEY_5:
        printf("[5] Change filter type in Low Pass.\n");
        pthread_mutex_lock(&mux_signal);
        if(filters[n_active_filters-1].filter_type != LOW_PASS)
        {
            filters[n_active_filters-1].filter_type = LOW_PASS;
            clear_request = 1; //re-plot signal
        }
        pthread_mutex_unlock(&mux_signal);
        break;

        case KEY_6:
        //High Pass
        printf("[6] Change filter type in High Pass.\n");
        pthread_mutex_lock(&mux_signal);
        if(filters[n_active_filters-1].filter_type != HIGH_PASS)
        {
            filters[n_active_filters-1].filter_type = HIGH_PASS;
            clear_request = 1; //re-plot signal
        }
        pthread_mutex_unlock(&mux_signal);
        break;

        case KEY_7:
        //Band Pass
        printf("[7] Change filter type in Band Pass.\n");
        pthread_mutex_lock(&mux_signal);
        if(filters[n_active_filters-1].filter_type != BAND_PASS)
        {
            filters[n_active_filters-1].filter_type = BAND_PASS;
            clear_request = 1; //re-plot signal
        }
        pthread_mutex_unlock(&mux_signal);
        break;

        /*CHANGE COLOR OF SIGNAL*/
        //Yellow
        case KEY_Y:
        printf("[Y] Change signal's color in Yellow.\n");
        pthread_mutex_lock(&mux_signal);
        if(input_signal.color != YELLOW)
        {
            input_signal.color = YELLOW;
            clear_request = 1; //re-plot signal
        }
        pthread_mutex_unlock(&mux_signal);
        break;

        //White
        case KEY_W:
        printf("[W] Change signal's color in White.\n");
        pthread_mutex_lock(&mux_signal);
        if(input_signal.color != WHITE)
        {
            input_signal.color = WHITE;
            clear_request = 1; //re-plot signal
        }
        pthread_mutex_unlock(&mux_signal);
        break;
        
        /*FFT*/
        case KEY_F:
        printf("[F] Enable FFT of input signal.\n");
        if(!fft_enable)
        {
            fft_enable = 1;
            clear_request = 1;
        }
        break;

        /*CLEAR REQUEST*/
        case KEY_C:
        clear_request = 1;
        printf("[C] Clear plot.\n");
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

void *signalTask(void *arg)
{
    int idx;
    idx = get_task_index(arg);
    set_activation(idx);

    //init_signal();    //init in keyboard_interp()
    printSignal(input_signal);

    while(!end_flag)
    {
        pthread_mutex_lock(&mux_signal);
        /***********BODY OF TASK**********/
        /*COMPUTE SIGNAL*/
        signalRealization();

        //FFT
        if(fft_enable)
            fftRealization();

        //Successive Sample
        input_signal.k++;
        /*********************************/
        pthread_mutex_unlock(&mux_signal);

        if(deadline_miss(idx))
            printf("******Deadline Miss of Signal Task!******** \n");

        wait_for_activation(idx);
    }

    return NULL;
}

void *filterTask(void* arg)
{
    int idx;
    idx = get_task_index(arg);
    set_activation(idx);

    init_filter(idx);
    printFilter(filters[idx]);

    while(!end_flag)
    {
        pthread_mutex_lock(&mux_signal);
        /***********BODY OF TASK**********/
        /*COMPUTE FILTER*/
        filterRealization(input_signal, idx);

        ////FFT (To do)
        //if(fft_enable)
        //    fftRealization();           
        /*********************************/
        pthread_mutex_unlock(&mux_signal);

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

    BITMAP *screen_copy;    //This copy is necessary to avoid blinking
    BITMAP *osc;            //Plot
    BITMAP *information;    //Information
    BITMAP *fft_screen;     //FFT display
    screen_copy = create_bitmap(WIDTH, HEIGHT);
    osc = create_bitmap(WIDTH, (HEIGHT-INFO_HEIGHT)/2);
    information = create_bitmap(WIDTH, INFO_HEIGHT);
    fft_screen = create_bitmap(WIDTH, (HEIGHT-INFO_HEIGHT)/2);
    clear_to_color(screen_copy, BLACK);

    while(!end_flag)
    {
        pthread_mutex_lock(&mux_signal);
        /***********BODY OF TASK**********/
        draw_information(information, screen_copy);
        draw_oscilloscope(osc, screen_copy);
        draw_fft(fft_screen, screen_copy);

        blit(screen_copy, screen, 0, 0, 0, 0, WIDTH, HEIGHT); //finally, copies in original screen
        /*********************************/
        pthread_mutex_unlock(&mux_signal);

        if(deadline_miss(idx))
            printf("******Deadline Miss of Graphic Task!********\n");

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

