#ifndef FILTERSLIB_H_
#define FILTERSLIB_H_

/*DEFINE*/
//general task parameters
#define PER 40 // [ms]
#define PRIO 1
#define IDX 2

//wave task parameters
#define WAVE_PER 100 // [ms] -> 10 Hz
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

//colours
#define BLACK 0
#define YELLOW 14

/*ENUMS*/
enum Signal_Type 
{
    sinusoid,
    square,
    saw,
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
double signalRealization(struct Signal signal, double time);
void printSignal(struct Signal signal);

//Init
void init();

//Tasks
void *helloWorldTask(void* arg);
void *waveTask(void* arg);

#endif /* FILTERSLIB_H_ */