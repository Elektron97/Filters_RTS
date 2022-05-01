#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <allegro.h>
#include "ballLib.h"
#include "taskLib.h"
#include "timeLib.h"

//-----------------------------------------------------
// THE PROGRAM NEEDS TO BE LAUNCHED AS SUPER USER (sudo) 
// BECAUSE OF THE EXPLICIT NON-DEFAULT SCHEDULER
//-----------------------------------------------------

int	main(void){
	int	r;

	init();
	r = task_create(userTask, UIDX, UPER, UPER, UPRIO);	// if 0, ok
	printf("user task: %d \n", r);
	r = task_create(displayTask, DIDX, DPER, DPER, DPRIO);
	printf("display task: %d \n", r);
	wait_for_task(DIDX);
	wait_for_task(UIDX);
}