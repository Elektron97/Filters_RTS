#ifndef FILTERSLIB_H_
#define FILTERSLIB_H_

/*DEFINE*/
//general task parameters
#define PER 40 // [ms]
#define PRIO 1
#define IDX 2

//wave task parameters
#define WAVE_PER 100 // [ms]
#define WAVE_PRIO 1
#define WAVE_IDX 1

//Math constants
#define PI 3.14159

//Discrete Time Constants
#define N_SAMPLE_PERIOD 100 //n samples in a period

//graphics parameters
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

/*STRUCTURES*/
struct Signal
{
    double amplitude;
    double frequency;
    double phase;
    enum Signal_Type signal_type;
};

/*FUNCTION SIGNATURES*/
//Functions
double sign(double x);
double lowPassFilter(double y_k_1, double x_k_1, double a, double Ts);
void plotPoint(double time, double y, int color);
double signalRealization(struct Signal signal, double time);
void printSignal(struct Signal signal);

//Init
void init();

//Tasks
void *helloWorldTask(void* arg);
void *waveTask(void* arg);

#endif /* FILTERSLIB_H_ */