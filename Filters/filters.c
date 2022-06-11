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
    printf("Hello World Task! Report: %d \n", task_report);
    wait_for_task(IDX);

    return 0;
}

void *helloWorldTask(void* arg)
{
    int idx;

    idx = get_task_index(arg);
    set_activation(idx);

    while(!end_flag)
    {
        printf("Hello World! \n");
        
        if(deadline_miss(idx))
            printf("******Deadline Miss!******** \n");

        wait_for_activation(idx);
    }

    return NULL;
}