#ifndef TASKLIB_H_
#define TASKLIB_H_

#define TMAX 40 //maximum number of tasks

struct task_par
{ 		
	int	arg;					// task argument		
	int	period; 				// [ms]
	int	relativeDL;	    		// [ms]
	int	priority;				// priority: [0 - 99]
	int	dMiss;					// how many times
	struct timespec actTime;	// next activation time	
	struct timespec deadline; 	// absolute deadline
};

struct task_par param[TMAX]; 	 
pthread_t tid[TMAX] ;

int	task_create(void*	(*task)(void	*), int	i, int	period, int	drel, int	prio);
void set_activation(int	i);
void wait_for_activation(int i);
void wait_for_task(int i);
int	deadline_miss(int i);
int	get_task_index(void* arg);
float frand(float xmi, float xma);
char get_scancode();

#endif /* TASKLIB_H_ */