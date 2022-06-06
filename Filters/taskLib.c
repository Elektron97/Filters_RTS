/****************************************
 *TASK LIB: Library for task management *
 ****************************************/

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

/*This function initializes a task*/
int task_create(void* (*task)(void *), int idx, int period, int drel, int prio)
{
    pthread_attr_t attr;
    struct sched_param prio_task;
    int tret;

    //Update parameters of idx-th task
    param[idx].arg = idx;
    param[idx].period = period;
    param[idx].relativeDL = drel;
    param[idx].priority = prio;
    param[idx].dMiss = 0;

    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    prio_task.sched_priority = param[idx].priority;
    pthread_attr_setschedparam(&attr, &prio_task);
    tret = pthread_create(&tid[idx], &attr, task, (void*)(&param[idx]));

    return tret;
}

/*This function sets absolute activation time and deadline*/
void set_activation(int idx)
{
    struct timespec t;

    //Read current time
    clock_gettime(CLOCK_MONOTONIC, &t);
    time_copy(&(param[idx].actTime), t);
    time_copy(&(param[idx].deadline), t);

    //Add period to compute next activation time and deadline
    time_add_ms(&(param[idx].actTime), param[idx].period);
    time_add_ms(&(param[idx].deadline), param[idx].period);
}