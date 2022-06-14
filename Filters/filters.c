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

    int user_report, graphic_report; // 0: Ok | 1: Task Issues

    user_report = task_create(userTask, UIDX, USER_PERIOD, USER_PERIOD, USER_PRIO);
    printf("User Task! Report: %d\n", user_report);
    graphic_report = task_create(graphicTask, GIDX, GRAPHIC_PERIOD, GRAPHIC_PERIOD, GRAPHIC_PRIO);
    printf("Graphic Task! Report: %d\n", graphic_report);
    wait_for_task(UIDX);
    wait_for_task(GIDX);

    allegro_exit();
    return 0;
}