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

//graphics parameters
#define WIDTH 800
#define HEIGHT 600

//colours
#define BLACK 0
#define YELLOW 14

/*STRUCTURES*/
struct Signal
{
    double amplitude;
    double frequency;
    double phase;
    //To add here: type of signals
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