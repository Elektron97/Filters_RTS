#ifndef PLIB_H_
#define PLIB_H_

#define NT 10 						// max number of tasks

// definisco una struttura chiamata task_par contenente i parametri di un task
struct task_par {
	int		arg;					// argomento del task, ad ogni task associo un indice (numero) in questo modo non uso puntatori ma interi
	long	wcet;					// worst case execution time
	int 	period; 				// in millisecondi
	int 	deadline;				// dead line relativa (in millisecondi)
	int 	priority;				// da 0 a 99 (non compatubile con standard MISRA)
	int		dmiss;					// contatore delle deadline miss
	struct 	timespec activ_time; 	// variabile di tipo struttura timespec (proviene da time.h) che chiamo activ_time
									// fornisce il tempo di attivazione successivo
	struct 	timespec dead_time;		// fornisce la deadline assoluta
};

struct task_par param[NT]; 			// definisco la variabile param di tipo struttura task_par (è un vettore di strutture)
	
pthread_t tid[NT];	 				// identificativi per ogni pthread (task)

// prototipi

int 	task_create (void* (*task)(void *), int i, int period, int drel, int prio);

void 	set_activation(int i);

void	wait_for_activation(int i);

void	wait_for_task(int i);

int		deadline_miss(int i);

int		get_task_index(void* arg);

int		get_task_period(void* arg);

#endif /* PLIB_H_ */
