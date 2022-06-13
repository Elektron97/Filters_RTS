
/*STANDARD LIBRARIES*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <allegro.h>

/*PERSONAL LIBRARIES*/
#include "plib.h"
#include "tlib.h"


#define XWIN 		1024		// window x resolution
#define YWIN 		768			// window y resolution
#define BITS_COL	8			// number of bits used for color
#define BKG 		0			// background color
#define PCOLOR  	14			// prompt color
#define RED 		4


#define IDX_COM_INTERP 4
#define PER 40
#define DREL 40
#define PRIO_INTERP 20

void *control_task(void *arg);

//----------------------------------------------------------------------------
// GET_SCANCODE: returns the scancode of a pressed key
//----------------------------------------------------------------------------
char 	get_scancode()
{
		if (keypressed()){					// returns true if there are characters in keyboard buffer,
											// else false (it does not block the program execution)
				return readkey() >> 8;}		// shift right the high byte
		else	return 0;
}

//----------------------------------------------------------------------------
// COM_INTERP FUNCTION: initialize graphic
//----------------------------------------------------------------------------
void* com_interp(void* arg)
{
		set_activation(IDX_COM_INTERP);
int i;
char scan;

	i = get_task_index(arg);
	printf("%d\n", i);


	do
	{	
		scan = 0;
		scan = get_scancode();						// returns the scancode of a pressed key
		switch (scan)
		{
			case KEY_SPACE:
				printf("ciao");
				break;
			default: break;
		}
		wait_for_activation(IDX_COM_INTERP);

	} while (scan != KEY_ESC);

	return 0;
}

long 	lrand(long min, long max)
{
long result;

		result = rand() % (max + 1 - min) + min;
		return result;
}

int 	irand(int min, int max)
{
int result;

		result = rand() % (max + 1 - min) + min;
		return result;
}


int main() {

srand(time(NULL));
long num;
long min = 100000000;
long max = 900000000;

	num = lrand(min, max);
	printf("%li\n", num);
return 0;
}
