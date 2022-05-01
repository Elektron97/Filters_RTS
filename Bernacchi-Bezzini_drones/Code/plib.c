#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include "plib.h"
#include "tlib.h"

int task_create (void* (*task)(void *), int i, int period, int drel, int prio);
void set_activation( int i);
void	wait_for_activation(int i);
void	wait_for_task(int i);
int	deadline_miss(int i);
int	get_task_index(void* arg);
int	get_task_period(void* arg);


/*-------------------------------------------------------------------------------------------------------------------------------*/
/*Funzione task_create() setta i valori della struttura param[i] e assegna gli attibuti (scheduler, priorità), e crea il thread  */
/*--------------------------------------------------------------------------------------------------------------------------------*/

int task_create (void* (*task)(void *), int i, int period, int drel, int prio)
{
	pthread_attr_t attributi; 
	struct sched_param prio_task; 
	int tret;			//valore di ritorno del thread
	
	param[i].arg = i;
	param[i].period = period;
	param[i].deadline = drel;
	param[i].priority = prio; 
	param[i].dmiss = 0;
	pthread_attr_init(&attributi); 
	pthread_attr_setinheritsched(&attributi, PTHREAD_EXPLICIT_SCHED); 
	pthread_attr_setschedpolicy(&attributi, SCHED_OTHER);		 
	prio_task.sched_priority = param[i].priority; 		
	pthread_attr_setschedparam(&attributi, &prio_task); 
	tret = pthread_create(&tid[i], &attributi, task, (void*)(&param[i])); 
	if (tret != 0) printf("Unable to create thread %d", i);
	pthread_attr_destroy(&attributi);
	return tret;
}


/*-------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*Funzione set_activation() legge il tempo corrente e calcola il prossimo tempo di attivazione e la deadline assoluta del task i cui parametri sono in param[i]*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void set_activation( int i) 						//i è l'indice del task
{
	struct timespec t; 						
	clock_gettime(CLOCK_MONOTONIC, &t); 				//leggo il tempo
	time_copy(&(param[i].activ_time), t);				//copio il tempo letto nella variabile struttura activ_time della struttura param
	time_copy(&(param[i].dead_time), t);				//copio il tempo letto nella variabile struttura dead_time della struttura param
	time_add_ms(&(param[i].activ_time), param[i].period); 	//aggiungo alla variabile struttura activ_time il periodo deciso
	time_add_ms(&(param[i].dead_time), param[i].period);

}



/*-------------------------------------------------------------------------------------------------------------------------------*/
/*Funzione wait_for_activation() sospende il task fino alla prossima attivazione e aggiorna il tempo di attivazione e la deadline*/
/*-------------------------------------------------------------------------------------------------------------------------------*/

void	wait_for_activation(int i)
{                                                                                       	//funzione della libreria time.h
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &(param[i].activ_time), NULL); 	//sospende il thread finchè il clock_monotonic non raggiunge il tempo specificato in activ_time
	time_add_ms(&(param[i].activ_time), param[i].period); 				//aggiungo alla variabile struttura activ_time il periodo
	time_add_ms(&(param[i].dead_time), param[i].period); 	
}



/*------------------------------------------------------*/
/*Funzione wait_for_task() aspetta che il tid[i] termini*/
/*------------------------------------------------------*/

void	wait_for_task(int i)
{

	pthread_join(tid[i], NULL);	// aspetto terminazione
	return;
}

/*------------------------------------------------------------------------------*/
/*Funzione deadline_miss() aggiorna il conteggio delle deadline se vengono perse*/
/*------------------------------------------------------------------------------*/

int	deadline_miss(int i)
{

	struct	timespec now;

	clock_gettime(CLOCK_MONOTONIC, &now); 		//prende il tempo attuale e lo salva nella variabile timespec now
	if (time_cmp(now, param[i].dead_time) > 0) { 		//se il tempo attuale è maggiore del tempo della deadline assoluta del task di indice i
		param[i].dmiss++; 				//allora il numero di deadline miss del task di indice i aumenta di uno
		return 1;
	}
	return 0;
}



/*------------------------------------------------------------*/
/*  Funzione get_task_index(*arg) ritorna l'indice del task  */
/*----------------------------------------------------------*/

int	get_task_index(void* arg)
{
struct	task_par *tpar;

	tpar = (struct task_par *)arg;
	return tpar->arg;
}



/*--------------------------------------------------------------*/
/*  Funzione get_task_period(*arg) ritorna il periodo del task  */
/*--------------------------------------------------------------*/

int	get_task_period(void* arg)
{
struct	task_par *tpar;
	tpar = (struct task_par *)arg;
	return tpar->period;
}

