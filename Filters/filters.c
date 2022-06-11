/********************************************************
 * FILTERS: Implementing filters in discrete time.      *
 ********************************************************/

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

/*MAIN*/
int main(void)
{
    init();

    int task_report; // 0: Ok | 1: Task Issues

    /*printf("Ciao!\n");
    task_report = task_create(helloWorldTask, IDX, PER, PER, PRIO);
    printf("Hello World Task! Report: %d \n", task_report);
    wait_for_task(IDX);*/

    task_report = task_create(waveTask, WAVE_IDX, WAVE_PER, WAVE_PER, WAVE_PRIO);
    printf("Wave World Task! Report: %d\n", task_report);
    wait_for_task(WAVE_IDX);

    allegro_exit();
    return 0;
}