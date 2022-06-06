#include <time.h>
#include "tlib.h"

/*-----------------------------------------------------------------------------------------------------*/
/*Funzione time_add_ms() aggiunge un valore espresso in millisecondi alla variabile tempo puntata da t*/
/*----------------------------------------------------------------------------------------------------*/

void time_add_ms(struct timespec *t, int ms)
{

	t->tv_sec += ms/1000;
	t->tv_nsec += (ms%1000)*1000000;
	if (t->tv_nsec > 1000000000) {
		t->tv_nsec -= 1000000000;
		t->tv_sec += 1;
	}

}

/*---------------------------------------------------------------------------------------*/
/*Funzione time_copy() copia una variabile tempo ts in una variabile tempo puntata da td*/
/*---------------------------------------------------------------------------------------*/

void time_copy(struct timespec *td, struct timespec ts)
{

	td->tv_sec = ts.tv_sec;
	td->tv_nsec = ts.tv_nsec;
}

/*---------------------------------------------------------------------------------------------------------------------*/
/*Funzione time_cmp() compara due variabili tempo t1 e t2 e restituisce 0 SE t1=t2, -1 SE t1<t2, 1 SE t1>t2*/
/*----------------------------------------------------------------------------------------------------------------------*/

int	time_cmp(struct timespec t1, struct timespec t2)
{

	if (t1.tv_sec > t2.tv_sec) return 1; 			//prima compara i secondi, se i primi due if non sono verificati significa che t1=t2 nei secondi e allora compara i nanosecondi
	if (t1.tv_sec < t2.tv_sec) return -1;
	if (t1.tv_nsec > t2.tv_nsec) return 1;
	if (t1.tv_nsec < t2.tv_nsec) return -1;
	return 0; 						//se anche il terzo e quarto if non è verificato allora t1=t2 in tutto e per tutto
}
	
