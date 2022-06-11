#ifndef FILTERSLIB_H_
#define FILTERSLIB_H_

/*DEFINE*/
//general task parameters
#define PER 40 // [ms]
#define PRIO 1
#define IDX 2

//wave task parameters
#define WAVE_PER 10 // [ms] -> 100 Hz
#define WAVE_PRIO 1
#define WAVE_IDX 1

//Discrete Integration
#define TIME_SCALE 0.01 
#define time_step TIME_SCALE*WAVE_PER // TIME_SCALE*WAVE_PER -> 10 KHz

//graphics parameters
#define WIDTH 800
#define HEIGHT 600

//colours
#define BLACK 0
#define YELLOW 14

/*FUNCTION SIGNATURES*/
void init();
void *helloWorldTask(void* arg);
void *waveTask(void* arg);

#endif /* FILTERSLIB_H_ */