// Test to calculate Worst Case Observed Time of single task
// Task runs 1'000'000 times and worst case is stored in the output final variable

#include <stdio.h>
#include <time.h>

#define milione 1000000

int main() {

	struct 		timespec t1;
	struct 		timespec t2;
	time_t 		sec_elapsed;
	long 		nanosec_elapsed;
	long int 	count = 0;

	while(count == milione)
	{
		clock_gettime(CLOCK_MONOTONIC, &t1);

		/* task code */

		clock_gettime(CLOCK_MONOTONIC, &t2);

		sec_elapsed = t2.tv_sec - t1.tv_sec;
		nanosec_elapsed = t2.tv_nsec - t1.tv_nsec;
	}

	return 0;
}