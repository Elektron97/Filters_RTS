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

/*DEFINE*/
#define PER 40
#define PRIO 1
#define IDX 1

/*GLOBAL VARIABLES*/
int end_flag = 0;

/*FUNCTION SIGNATURES*/
void *helloWorldTask(void* arg);

int main(void)
{
    int task_report;

    printf("Ciao!\n");

    task_report = task_create(helloWorldTask, IDX, PER, PER, PRIO);



    return 0;
}

void *helloWorldTask(void* arg)
{
    int idx;

    idx = get_task_index(arg);
    set_activation(idx);

    while(!end)
    {
        printf("Hello World! \n");
        deadline_miss(idx);
        wait_for_activation(idx);
    }
    
}