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
//task parameters
#define PER 40
#define PRIO 1
#define IDX 1

//graphics parameters
#define WIDTH 800
#define HEIGHT 600

//colours
#define BLACK 0
#define YELLOW 14

/*GLOBAL VARIABLES*/
int end_flag = 0;

/*FUNCTION SIGNATURES*/
void init();
void *helloWorldTask(void* arg);
void *waveTask(void* arg);

/*MAIN*/
int main(void)
{
    init();

    int task_report; // 0: Ok | 1: Task Issues

    printf("Ciao!\n");
    task_report = task_create(helloWorldTask, IDX, PER, PER, PRIO);
    printf("Hello World Task! Report: %d \n", task_report);
    wait_for_task(IDX);

    allegro_exit();
    return 0;
}

void init()
{
    allegro_init();
    install_keyboard();

    set_color_depth(8); //256 VGA

    //Enter in Graphics mode with full screen(width, height)
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, WIDTH, HEIGHT, 0, 0);

	//Clear Screen: ("global BITMAP called screen")
	clear_to_color(screen, BLACK); //Black Background
}

/*FUNCTIONS*/
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

void *waveTask(void* arg)
{
    int idx = get_task_index(arg);
    set_activation(idx);

    while(!end_flag)
    {
        /************************
         * Body of Task here:   *
         * **********************/
        
        if(deadline_miss(idx))
            printf("******Deadline Miss of Wave Task!******** \n");

        wait_for_activation(idx);
    }

    return NULL;
}