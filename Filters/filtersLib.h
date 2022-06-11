#ifndef FILTERSLIB_H_
#define FILTERSLIB_H_

/*DEFINE*/
//task parameters
#define PER 40
#define PRIO 1
#define IDX 1

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