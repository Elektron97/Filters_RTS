#ifndef FILTERSLIB_H_
#define FILTERSLIB_H_

/*DEFINE*/
/*Filter and Signal parameters*/
//maximum number
#define MAX_SIGNALS 10
#define MAX_FILTERS 10

//min and maximum frequency
#define FREQ_MIN 0          // Constant Signal
#define FREQ_MAX 100        // 100 Hz

/*Task Parameters*/
//Index
#define GIDX (MAX_SIGNALS + 1) //Graphic Task
#define UIDX (MAX_SIGNALS + 2) //User Task

//Period
#define FILTER_PERIOD   10     //[ms]
#define GRAPHIC_PERIOD  20     //[ms]
#define USER_PERIOD     30     //[ms]

//Priority  | 1 (low) - 99 (high)
#define FILTER_PRIO     3
#define GRAPHIC_PRIO    2
#define USER_PRIO       1

//Math constants
#define PI 3.14159

//Discrete Time Constants
#define N_SAMPLE_PERIOD 100 //n samples in a period

/*Graphics Parameters*/
#define WIDTH 800
#define HEIGHT 600

//axis limit
#define XLIM 1.0

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
    BAND_PASS,
    BAND_STOP
};

//To do
/*enum Discretization_Tech
{
    FORWARD_EULER,
    BACKWARD_EULER,
    ZETA
};*/

/*STRUCTURES*/
struct Signal
{
    double amplitude;
    double frequency;   //[Hz]
    double phase;       //[rad]
    double Ts;          //[s]
    int    k;           //k-th sample
    int    color;
    enum Signal_Type signal_type;
};

struct Filter
{
    double freq_low;    //[Hz]
    double freq_high;   //[Hz]
    double gain;
    enum Filter_Type filter_type;
};

/*Global Resources*/
struct Signal signals[MAX_SIGNALS];
struct Filter filters[MAX_FILTERS];


/*FUNCTION SIGNATURES*/
//Functions
double sign(double x);
double lowPassFilter(double y_k_1, double x_k_1, double a, double Ts);
void plotPoint(double time, double y, int color);
double signalRealization(struct Signal signal, double time);
void printSignal(struct Signal signal);

//Init
void init();
void set_Ts(int idx);
void init_signal(int idx);
void clear_reset(int idx);

//Keyboard Interpreter
void keyboard_interp();

/*TASKS*/
void *filterTask(void* arg);
void *graphicTask(void* arg);
void *userTask(void* arg);

/*TEST TASKS*/
//void *helloWorldTask(void* arg);
//void *waveTask(void* arg);

#endif /* FILTERSLIB_H_ */