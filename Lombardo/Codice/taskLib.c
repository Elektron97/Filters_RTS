#include	<stdlib.h>
#include	<stdio.h>
#include	<math.h>
#include	<pthread.h>
#include	<sched.h>
#include	<allegro.h>
#include	<time.h>
#include	"ballLib.h"
#include	"taskLib.h"
#include	"timeLib.h"

//task_create initializes a task//
int	task_create(void* (*task)(void *), int	idx, int	period, int	drel, int	prio){
	pthread_attr_t attr; 
	struct sched_param prio_task; 
	int tret;
	
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

//set_activation sets absolute activation time and deadline//
void set_activation(int	idx){
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	time_copy(&(param[idx].actTime), t);
	time_copy(&(param[idx].deadline), t);
	time_add_ms(&(param[idx].actTime), param[idx].period);
	time_add_ms(&(param[idx].deadline), param[idx].period);
}

//wait for activation makes a task sleep and resets it for next period//
void	wait_for_activation(int idx){
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &(param[idx].actTime), NULL); //sleeps until activation time
	time_add_ms(&(param[idx].actTime), param[idx].period); //sets next activation time and deadline
	time_add_ms(&(param[idx].deadline), param[idx].period); 	
}

//wait for task//
void	wait_for_task(int idx){
	pthread_join(tid[idx], NULL); //wait for task to end
	return;
}

//deadline_miss checks for dl miss//
int	deadline_miss(int idx){
	struct	timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	if (time_cmp(t, param[idx].deadline) > 0) {
		param[idx].dMiss++; // increases dl miss counter if deadline is exceeded
		drawInfo();	// refresh dlmiss counter on screen
		return 1;
	}
	return 0;
}

//get task index//
int	get_task_index(void* arg){
	struct	task_par *tpar;

	tpar = (struct task_par *)arg;
	return tpar->arg; //returns task index (arg)
}

//frand generates a random number in a range//
float	frand(float	xmi, float	xma){
	float	r;
	
	r = rand()/(float)RAND_MAX;
	return xmi + (xma - xmi)*r;
}

//get_scancode fetches a pressed key as a char//
char	get_scancode(){
	if (keypressed())
		return readkey() >> 8;
	else
		return 0;
}
