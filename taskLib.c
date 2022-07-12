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

/*This function makes a task idx sleep and resets it for next period*/
void wait_for_activation(int idx)
{
    //sleeps until activation time
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &(param[idx].actTime), NULL);

    //resets next activation time and deadline
    time_add_ms(&(param[idx].actTime), param[idx].period);
    time_add_ms(&(param[idx].deadline), param[idx].period);
}

/*wait for task: implementing pthread_join in idx system*/
void wait_for_task(int idx)
{
    pthread_join(tid[idx], NULL);
    return;
}

/*Deadline Miss*/
int deadline_miss(int idx)
{
    struct timespec t;

    //read current time
    clock_gettime(CLOCK_MONOTONIC, &t);

    //check
    if(time_cmp(t, param[idx].deadline) > 0)
    {
        param[idx].dMiss++; //increases deadline miss counter
        return 1;
    }

    return 0;
}

/*Get Task Index*/
int get_task_index(void* arg)
{
    struct task_par *tpar;

    tpar = (struct task_par *)arg;
    return tpar->arg; //returns task index (arg)
}

/*frand generates a random number in a range*/
float frand(float xmi, float xma)
{
    float r;

    r = rand()/(float)RAND_MAX;
    return xmi + (xma - xmi)*r;
}

/*Get scan code: fetches a pressed key as char*/
char get_scancode()
{
    if(keypressed())
        return readkey() >> 8;
    else
        return 0;
}