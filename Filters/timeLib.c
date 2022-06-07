/********************************
 * TIME LIB: Custom libraries   *
 * to management time           *
 ********************************/

/*INCLUDE*/
//Time libraries
#include <time.h>
#include "timeLib.h"

/*REMARK: timespec*/

/*
struct timespec
{
    time_t tv_sec; //seconds
    long tv_nsec; //nanoseconds
}

//Note that time = tv_sec . tv_nsec
//Example: 10.123456789
// tv_sec = 10
// tv_nsec = 123456789
*/


/* This function copies a source time variable ts
 * in a destination variable pointed by td */
void time_copy(struct timespec *td, struct timespec ts)
{
    td->tv_sec = ts.tv_sec;
    td->tv_nsec = ts.tv_nsec;
};

/* This function add ms expressed in milliseconds
 * to the time variable pointed by t*/

void time_add_ms(struct timespec *t, int ms)
{
    t->tv_sec += ms/1000;
    t->tv_nsec += (ms%1000)* 1000000;

    //If nanoseconds are > 1e9
    if (t->tv_nsec > 1000000000)
    {
		t->tv_nsec -= 1000000000;
		t->tv_sec += 1;
	}
};

/* This function compares two time variables t1 and t2:
 * return 0: t1 == t2
 * return 1: t1 > t2
 * return -1: t1 < t2 */

int time_cmp(struct timespec t1, struct timespec t2)
{
	if (t1.tv_sec > t2.tv_sec) return 1;
	if (t1.tv_sec < t2.tv_sec) return -1;
	if (t1.tv_nsec > t2.tv_nsec) return 1;
	if (t1.tv_nsec < t2.tv_nsec) return -1;
	return 0;
};





