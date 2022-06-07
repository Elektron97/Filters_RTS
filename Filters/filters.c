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

//-----------------------------------------------------
// THE PROGRAM NEEDS TO BE LAUNCHED AS SUPER USER (sudo) 
// BECAUSE OF THE EXPLICIT NON-DEFAULT SCHEDULER
//-----------------------------------------------------

int main(void)
{
    printf("Ciao!");
    return 0;
}