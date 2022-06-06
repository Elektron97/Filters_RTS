#ifndef PLIB_H_
#define PLIB_H_

#define NT 10 

struct task_par{ 		
	int	arg;		
	int 	period; 		//periodo (in millisecondi)
	int 	deadline;		//dead line relativa (in millisecondi)
	int	priority;		//da 0 a 99
	int	dmiss;			//numero di deadline misses
	struct timespec activ_time; 	//variabile di tipo struttura timespec che chiamo activ_time (la struttura time spec è della lib time e contiene tv_sec (secondi) e tv_nsec (nanosecondi))
				    	
	struct timespec dead_time; 	
	};
struct task_par param[NT]; 	 
	
pthread_t tid[NT] ; //dichiaro 7 variabili di tipo thread 

//prototipi

int task_create (void* (*task)(void *), int i, int period, int drel, int prio);

void set_activation( int i);

void	wait_for_activation(int i);

void	wait_for_task(int i);

int	deadline_miss(int i);

int	get_task_index(void* arg);

int	get_task_period(void* arg);

#endif /* PLIB_H_ */
