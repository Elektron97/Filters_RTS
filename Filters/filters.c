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
    task_report = task_create(userTask, UIDX, USER_PERIOD, USER_PERIOD, USER_PRIO);
    printf("User Task! Report: %d\n", task_report);
    wait_for_task(UIDX);

    allegro_exit();
    return 0;
}