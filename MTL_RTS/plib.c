// Ptask Library

#include <stdio.h>		// standard input-output header
#include <stdlib.h>   	// standard library
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>		// execution scheduling
#include "plib.h"		// header plib
#include "tlib.h"		// header del tempo

// definisco le funzioni
int 	task_create (void* (*task)(void *), int i, int period, int drel, int prio);
void	set_activation(int i);
void	wait_for_activation(int i);
void	wait_for_task(int i);
int		deadline_miss(int i);
int		get_task_index(void* arg);
int		get_task_period(void* arg);

/*funzione che mi setta i valori della struttura param[i] con quelli che gli passo io, e assegna gli attibuti (scheduler, priorità), e crea il thread  */
int 	task_create (void* (*task)(void *), int i, int period, int drel, int prio)
{
	pthread_attr_t attributi; 	//variabile di tipo pthread_attr_t che si chiama attributi (sono gli attributi di tid, da inizializzare 
								// ogni modifica che si fa); è una struttura camuffata:stack size-state-priority-scheduler slide 4,1)
	
	struct sched_param prio_task;	// **variabile prio_task1 che è una variabile struttura per la priorità del task,
									// è una struttura con solo questo campo (contiene sched_priority) (sarebbe mypar. per priority)
	
	int tret;	//valore di ritorno del thread
	
	if (i > NT){
		printf("Number of task exceeds NT\n");
		return -1;
	}

	param[i].arg = i;
	param[i].period = period;
	param[i].deadline = drel;
	param[i].priority = prio;
	param[i].dmiss = 0;

	// Initialize Attributes
	pthread_attr_init(&attributi);

	// Setting the scheduler
	pthread_attr_setinheritsched(&attributi, PTHREAD_EXPLICIT_SCHED);	//dico a linux che voglio settare lo scheduler
	pthread_attr_setschedpolicy(&attributi, SCHED_RR); 					//specifico lo scheduler che voglio (SCHED_OTHER è default)

	// Thread priority
	prio_task.sched_priority = param[i].priority;		// imposto il numero di priorità
	pthread_attr_setschedparam(&attributi, &prio_task);	// conferma impostazione di priorità

	tret = pthread_create(&tid[i], &attributi, task, (void*)(&param[i])); // è qui che creo il thread ed entra in ballo la mia funzione task
	if (tret != 0) printf("Unable to create thread %d\n", i);		// print se ci sono stati errori nella creazione del task

	// Destroy attributes
	pthread_attr_destroy(&attributi);
	return tret;	
}


// Funzione che legge il tempo corrente e calcola il prossimo istante di attivazione e la deadline assoluta del task
void 	set_activation(int i) // i è l'indice del task
{
	struct timespec t;						// utilizzerà la struct per salvarci il tempo corrente
	clock_gettime(CLOCK_MONOTONIC, &t); 	// copio il tempo nella struct
	time_copy(&(param[i].activ_time), t);	// copio il tempo letto nell' elemento i del vettore param di tipo struct task_par, campo activ_time 
	time_copy(&(param[i].dead_time), t);	// copio il tempo letto nell' elemento i del vettore param di tipo struct task_par, campo dead_time
	time_add_ms(&(param[i].activ_time), param[i].period);	// calcolo il prossimo tempo di attivazione: t + T_i
	time_add_ms(&(param[i].dead_time), param[i].period);	// calcolo la deadline assoluta del task attivato: t + D_i
}


//funzione che sospende il treadh fino alla prossima attivazione e aggiorna il tempo di attivazione e la deadline
void	wait_for_activation(int i)
{
	// sospende l' esecuzione del thread chiamante finchè il clock_monotonic non raggiunge il tempo specificato in activ_time
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &(param[i].activ_time), NULL); 
	// quando si risveglierà aggiorna il tempo di attivazione e la deadline con il periodo che avevo deciso in task_create
	time_add_ms(&(param[i].activ_time), param[i].period);
	time_add_ms(&(param[i].dead_time), param[i].period);
}


// Funzione che aspetta che il thread con indentificatore tid[i] termini
void	wait_for_task(int i)
{
	pthread_join(tid[i], NULL);	// wait for thread tid[i] termination
	return;
}


/// Aggiorno il conteggio delle deadline se vengono perse
int	deadline_miss(int i)
{

	struct	timespec now;

	clock_gettime(CLOCK_MONOTONIC, &now); 			// prende il tempo attuale e lo salva nella variabile timespec now
	if (time_cmp(now, param[i].dead_time) > 0) 		// se il tempo attuale è maggiore del tempo della deadline assoluta
	{												// del task di indice i
		param[i].dmiss++; 							//allora il numero di deadline miss del task di indice i aumenta di uno
		return 1;
	}
	return 0;
}


// Restituisce l' indice del task che è stored in tp->arg
int	get_task_index(void* arg)
{
struct	task_par *tpar;	// definisco un puntatore di tipo struct task_par

	tpar = (struct task_par *)arg; // effettuo un casting del puntatore a void di nome arg che diventa un puntatore a struct task_par
	return tpar->arg;
}


// Restituisce il periodo del task che è stored in tp->arg
int	get_task_period(void* arg)
{
struct	task_par *tpar;
	tpar = (struct task_par *)arg;
	return tpar->period;
}

