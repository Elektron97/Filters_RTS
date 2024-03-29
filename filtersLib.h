#ifndef FILTERSLIB_H_
#define FILTERSLIB_H_

/*DEFINE*/
/*Filter and Signal parameters*/
//maximum number
#define MAX_FILTERS 10

//min and maximum frequency
#define FREQ_MIN 0          // Constant Signal
#define FREQ_MAX 100        // 100 Hz

//n-order filter
//#define MAX_ORDER 1         //First Order 
#define MAX_ORDER 2         //Second Order (actually)

/*Task Parameters*/
//Index
#define SIGNAL_IDX  MAX_FILTERS //Signal Task
#define GIDX (MAX_FILTERS + 1)  //Graphic Task
#define UIDX (MAX_FILTERS + 2)  //User Task

//Period
#define SIGNAL_PERIOD   10     //[ms]
#define FILTER_PERIOD   10     //[ms]
#define GRAPHIC_PERIOD  20     //[ms]
#define USER_PERIOD     30     //[ms]

//Priority  | 1 (low) - 99 (high)
#define SIGNAL_PRIO     24
#define FILTER_PRIO     23
#define GRAPHIC_PRIO    22
#define USER_PRIO       21

//Math constants
#define PI 3.14159

//Discrete Time Constants
#define N_SAMPLE_PERIOD 100 //n samples in a period

/*Graphics Parameters*/
#define WIDTH 1024
#define HEIGHT 768

//Title
#define TITLE_WIDTH 50
#define TITLE_HEIGHT 30

//Command information 
#define COMMAND_WIDTH 10
#define BOX_2 (COMMAND_WIDTH + 205)
#define BOX_3 (BOX_2 + 205)

//Information Tab height
#define INFO_HEIGHT 68
#define INFO_SIGNAL_WIDTH 600
#define INFO_FILTER_WIDTH (INFO_SIGNAL_WIDTH + 200)

//axis limit
#define XLIM 1.0 //Default 1.0
#define YLIM 1.0 //Fixed

//String maximum length
#define MAX_CHAR 50

//Colors
#define BLACK           0
#define BLUE            1
#define GREEN           2
#define CYAN            3
#define RED             4
#define MAGENTA         5
#define BROWN           6
#define LIGHT_GRAY      7
#define DARK_GRAY       8
#define LIGHT_BLUE      9
#define LIGHT_GREEN     10
#define LIGHT_CYAN      11
#define LIGHT_RED       12
#define LIGHT_MAGENTA   13
#define YELLOW          14
#define WHITE           15

//FFT constant
#define FFT_DATA 128 //Must be 2^N

/*ENUMS*/
enum Signal_Type
{
    sinusoid,
    square,
    sawtooth,
    triang
};

enum Filter_Type
{
    LOW_PASS,
    HIGH_PASS,
    BAND_PASS
};

enum Signal_Operation
{
    NO_OPERATION,   //no operations
    SUM,            //sum of signals
    PROD            //product of signals
};

enum Plot_Style
{
    POINT,
    INTERP_LIN
};

//Complex typedef for FFT
typedef double complex cplx;

/*STRUCTURES*/
struct Signal
{
    //General Attributes of Signal
    double amplitude;
    double frequency;   //[Hz]
    double phase;       //[rad]
    enum Signal_Type signal_type;

    //Discrete Time Parameter
    double Ts;          //[s]
    int    k;           //k-th sample

    //Graphic Attribute
    double t;           //[s]
    double y[MAX_ORDER + 1]; 
    int    color;

    //fft
    cplx fftSamples[FFT_DATA];
    cplx fftData[FFT_DATA];
};

struct Filter
{
    //General Attributes of Filter
    double f_cut;   //[Hz]
    double gain;
    enum Filter_Type filter_type;

    //Graphic Attribute
    double y_filtered[MAX_ORDER + 1]; //{y(k), y(k-1), ..., y(k-MAX_ORDER)}
    int color;

    //fft
    cplx fftSamples[FFT_DATA];
    cplx fftData[FFT_DATA];
};

/****************************************************
 * Note on struct Signal & Filter:                  *
 * signal.y and filter.y_filtered are array         *
 * because a n-order filter needs n past samples    *
 * to compute filtered signal.                      *
 * An example, for an first order high pass filter, *
 * algorithm needs x(k), x(k-1) and y(k-1).         *
 ****************************************************/

/*Global Resources*/
struct Signal input_signal;
struct Filter filters[MAX_FILTERS];

/*FUNCTION SIGNATURES*/
//Maths
double sign(double x);

//Signal
double sinWave(double amp, double frequency, double phase, double t);
double squareWave(double amp, double frequency, double phase, double t);
double sawtoothWave(double amp, double frequency, double phase, double t);
double triangWave(double amp, double frequency, double phase, double t);
double waveGenerator(double amp, double frequency, double phase, double t, int signal_type);

//Filter
double lowPassFilter(double y_k_1, double x_k_1, double a, double Ts);
double highPassFilter(double x_k, double x_k_1, double y_k_1, double a, double Ts);
double bandPassFilter(double x_k_1, double x_k_2, double y_k_1, double y_k_2, double a1, double a2, double Ts);

//Plot style
void plotPoint(BITMAP* window, double time, double y, int color);
void plotLin(BITMAP* window, double time_k, double time_k_1, double y_k, double y_k_1, int color);

//Realization
void signalRealization();
void filterRealization(struct Signal signal, int idx);
void fftRealization();

//Debug
void printSignal(struct Signal signal);
void printFilter(struct Filter filter);

//FFT
void _fft(cplx buf[], cplx out[], int n, int step);
void fft(cplx buf[], int n);

//Init
void init();
void set_Ts();
void init_signal();
void init_filter(int idx);

//Graphics
void clear_reset(BITMAP* window);
void clear_resetIdx(BITMAP* window, int idx);
void draw_oscilloscope(BITMAP* osc, BITMAP* window);
void draw_information(BITMAP* info, BITMAP* window);
void draw_fft(BITMAP* fft_bitmap, BITMAP* window);

//Keyboard Interpreter
void keyboard_interp();

/*TASKS*/
void *signalTask(void *arg);
void *filterTask(void* arg);
void *graphicTask(void* arg);
void *userTask(void* arg);

#endif /* FILTERSLIB_H_ */