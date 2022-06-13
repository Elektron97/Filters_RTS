// TIME LIBRARY

#include <time.h>
#include "tlib.h"

// la funzione aggiunge un valore int ms (espresso in millisecondi) alla variabile tempo puntata da t

void time_add_ms(struct timespec *t, int ms) // timespec è il tipo (tv_sec è di tipo time_t [32 bit dip. architett], tv_nsec è long [32 bit])
{
	t->tv_sec += ms/1000;						// es: 2500 / 1000 = 2
	t->tv_nsec += (ms%1000)*1000000;			// es: 2500 div 1000 = 0.5 ms e lo converto in ns

	if (t->tv_nsec > 1000000000) {				// se durante la somma i ns convertiti sono 
		t->tv_nsec -= 1000000000;				// più grandi di 1 secondo trasformo
		t->tv_sec += 1;
	}
}


// funzione che copia una variabile tempo ts in una variabile tempo puntata da td

void time_copy(struct timespec *td, struct timespec ts)
{
	td->tv_sec = ts.tv_sec;			// a sinistra il puntatore ad un campo della struttura mentre a destra
	td->tv_nsec = ts.tv_nsec;		// una variabile del campo della struttura
}


// funzione che compara due variabili tempo tra di loro t1 e t2 e restituisce 0 SE t1=t2, -1 SE t1<t2, 1 SE t1>t2
// (serve per capire se ho superato o no la deadline)

int	time_cmp(struct timespec t1, struct timespec t2)
{
	if (t1.tv_sec > t2.tv_sec) return 1;		// prima compara i secondi, se i primi due if non sono verificati
	if (t1.tv_sec < t2.tv_sec) return -1;		// significa che t1=t2 nei secondi

	if (t1.tv_nsec > t2.tv_nsec) return 1;		// e allora compara i nanosecondi
	if (t1.tv_nsec < t2.tv_nsec) return -1;

	return 0; // se anche il terzo e quarto if non è verificato allora t1=t2 in tutto e per tutto
}
	
