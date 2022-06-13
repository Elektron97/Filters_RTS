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

/*GRAPHIC CONSTANTS*/
#define BLACK 0						// 8 bit color
#define YELLOW 14		
#define GREEN 2
#define YELLOW 14
#define L_GREEN 10
#define XW 800							// x window resolution
#define YW 700							// y window resolution
#define XD 30							//x drone window resolution
#define YD 30							//y drone window resolution
#define BMAX 15						//base maggiore drone
#define H 8							//altezza drone
#define BMIN 10						//base minore drone
#define XN 100							//coordinate noise
#define YN 46
#define BV 80
#define AV 65

/*THREAD CONSTANTS*/
#define _GNU_SOURCE						//va fatto per i mutex
#define NT 10							//numero max task
#define MAX_DRONI 6						//numero max droni
#define GRAPHIC_TASK_INDEX 6
#define CONTROL_TASK_INDEX 7
#define PERIOD1 20
#define PERIOD2 30
#define DREL1 20
#define DREL2 30
#define HPRIO 80
#define MPRIO 50

/*FUNCTION PROTOTYPES*/
void init();								//inizializza allegro e mutex
void write_noise_update(BITMAP *screen_copy);  			//setta il noise graficamente
void change_param_filter();						//setta il noise numericamente 
void write_deadline_miss(BITMAP *screen_copy);			//contatore grafico delle deadline miss dei task
void write_graphic(BITMAP *screen_copy);				//disegna parte grafica di routine e aggiornamenti della grafica (qui dentro ho write_noise_update e write_deadline_miss)
void init_drone(int i);						//inizializza drone's structure
void init_kalman_filter(int i);					//inizializzo il filtro di kalman
void init_controller(int i);
void kalman_filter(int i);						//function kalman_filter() takes x,y noisy mouse coordinates and computes an estimate of them
void handle_borders(int i);						//funzione che evita il superamentro dei bordi cosi il drone resta nel suo spazio disponibile
void interpret_command();						//funzione che interpreta i tasti premuti dall'utente
void controller_drone(int i);
void draw_drone(BITMAP *d_drone, BITMAP *screen_copy, int i);	//funzione che disegna il drone (che è una bitmap) sulla bitmap(screen_copy) nelle coordinate uscenti dal KF
void *drone_task(void *arg);						//task drone - ogni periodo gestisce le coordinate del drone (qui dentro ho il Kalman-filter)
void *graphic_task(void *arg);					//task grafico - ogni periodo disegna il drone, la grafica di routine e i relativi aggiornamenti
void *control_task(void *arg);					//task di controllo - ogni periodo interpreta i tasti premuti e agisce
int main();

/* DRONE STRUCTURE*/
struct drone {

	int x_pos;		// coordinate in cui si posiziona il drone (le calcola il controllore)
	int y_pos;
	int a; 		//angolo (calcola il controllore)
	int color;	
			
};
struct drone drone[MAX_DRONI];


/* CONTROL STRUCTURE*/
struct control {

	double x_des;		//coordinate che voglio raggiungere (le calcola il KF)
	double y_des;
	

	
};
struct control controller[MAX_DRONI];


/* KALMAN FILTER STRUCTURE*/
struct k_f{
	double K;
	double R;		//rumore di misura (sensori)
	double P;		//matrice di cov
	double Q;		//disturbo di processo	
};
struct k_f kf[MAX_DRONI];


/* GLOBAL VARIABLES*/
int	end = 0; 						//osservazione per lu: ciclo while(!end) ovvero while(1),termina quando end=1
int 	command = 0; 
int	nad = 0;		//number of active drones
int 	dlm_graphic = 0;	//number of dead line miss graphic task
int 	dlm_drone = 0;		//number of dead line miss drone task
int 	dlm_control = 0;	//number of dead line miss control task
int 	i_noise = 0;		//indici per il settaggio del noise
int	j_noise = 0;


/*SEMAPHORES*/
//pthread_mutex_t	mux_drone;			//mutex drone's data
//pthread_mutex_t	mux_kalman;
//pthread_mutexattr_t	mux_attr; 				//attributi del mutex
pthread_mutex_t mux_kalman = PTHREAD_MUTEX_INITIALIZER; //controlla se basta questo anziche settare a mano 
pthread_mutex_t mux_drone = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mux_controller = PTHREAD_MUTEX_INITIALIZER; 
/*---------------------------*/
/*inizializzo allegro e mutex*/
/*---------------------------*/

void init()
{
	allegro_init();
	install_keyboard();
	install_mouse();
	set_color_depth(8);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED,XW, YW, BLACK,BLACK);
	clear_to_color(screen,0);
	show_mouse(screen);
	
	//pthread_mutexattr_init(&mux_attr); //inizializzo attributi del mutex
	//pthread_mutexattr_setprotocol(&mux_attr, PTHREAD_PRIO_INHERIT); //scelgo come protocollo nel mutex priority inheritance
	//pthread_mutex_init(&mux_drone, &mux_attr);
	//pthread_mutex_init(&mux_kalman, &mux_attr);
	
	return;
	
   }
   
/*---------------------------*/
/*setta il noise graficamente*/
/*---------------------------*/

void write_noise_update (BITMAP *screen_copy){

   	int k;
        k = i_noise - j_noise;
        if (k == 1){								//2 tacche
                scare_mouse();
                rectfill(screen_copy, XN-10, BV, XN+11, AV-10,BLACK);
                rectfill(screen_copy, XN-10, BV, XN-9, AV+10, YELLOW);
                rectfill(screen_copy, XN-5, BV, XN-4, AV+5, YELLOW);
                unscare_mouse();



            } else if (k == 2){						//3 tacche
                scare_mouse();
                rectfill(screen_copy, XN-10, BV, XN+11, AV-10,BLACK);
                rectfill(screen_copy, XN-10, BV, XN-9, AV+10, YELLOW);
                rectfill(screen_copy, XN-5, BV, XN-4, AV+5, YELLOW);
                rectfill(screen_copy, XN, BV, XN+1, AV, YELLOW);
                unscare_mouse();

            }
            else if( k == 3){							//4 tacche
                scare_mouse();
                rectfill(screen_copy, XN-10, BV, XN+11, AV-10,BLACK);
                rectfill(screen_copy, XN-10, BV, XN-9, AV+10, YELLOW);
                rectfill(screen_copy, XN-5, BV, XN-4, AV+5, YELLOW);
                rectfill(screen_copy, XN, BV, XN+1, AV, YELLOW);
                rectfill(screen_copy, XN+5, BV, XN+6, AV-5, YELLOW);
                unscare_mouse();

            } else if( k == 0){						//1 tacca (valore iniziale)
                scare_mouse();
                rectfill(screen_copy, XN-10, BV, XN+11, AV-10,BLACK);
                rectfill(screen_copy, XN-10, BV, XN-9, AV+10, YELLOW);
                unscare_mouse();

                 }
                else if ( k >= 4){ 						//raggiunto il limite di aumento noise (resto a 4 tacche)

                    i_noise = 3;
                    j_noise = 0;
                }
                else if ( k <= 0){  						//raggiunto il limite di diminuzione noise (resto a 1 tacca)

                    i_noise = 0;
                    j_noise = 0;
                }
                return;
}

/*--------------------------------------------------------------------------------------------------------------------------------------*/
/* funzione che cambia i parametri del filtro (per ora solo il noise, se non si deve cambiare altro, cambia nome alla funzione )*/
/*---------------------------------------------------------------------------------------------------------------------------------------*/

void change_param_filter()
{
int k, i;

	k = i_noise - j_noise;
	if (k == 1)
	{
		for(i = 0; i<MAX_DRONI; i++)
			kf[i].R = 20;		//2 tacche
	} 
	else if (k == 2)
	{
		for(i = 0; i<MAX_DRONI; i++)
			kf[i].R = 30;		//3 tacche
	} 
	else if( k == 3){
            	for(i=0; i<MAX_DRONI; i++)
            	{
               	kf[i].R = 40;		//4 tacche
               	}
            } else if( k == 0){
            	for(i = 0; i<MAX_DRONI; i++)
            	{
            		kf[i].R = 10;		//1 tacca (valore iniziale)
            		} 
            } else if ( k >= 4){ 	//raggiunto il limite di aumento noise (resto a 4 tacche)
		i_noise = 3;
		j_noise = 0;
            } else if ( k <= 0){  	//raggiunto il limite di diminuzione noise (resto a 1 tacca)
		i_noise = 0;
		j_noise = 0;
                }
                return;
}

/*---------------------------------------------------------------------*/
/* funzione che aggiorna graficamente il conteggio delle deadline miss */
/*---------------------------------------------------------------------*/
void write_deadline_miss(BITMAP *screen_copy)
{
	char s1[10], s2[10], s3[10];
	
	sprintf(s1, "%d", dlm_graphic);
	sprintf(s2, "%d", dlm_drone);
	sprintf(s3, "%d", dlm_control);
	
	textout_ex(screen_copy, font, s1, (XW/8)+45, (YW/4)+35, YELLOW, BLACK);
	textout_ex(screen_copy, font, s2, (XW/8)+45, (YW/4)+50, YELLOW, BLACK);
	textout_ex(screen_copy, font, s3, (XW/8)+45, (YW/4)+65, YELLOW, BLACK);
	
	return;
}

/*---------------------------------------------------------*/
/*inizializzazione parte grafica di routine e aggiornamenti*/
/*--------------------------------------------------------*/

void write_graphic(BITMAP *screen_copy)
{
	//PARTE GRAFICA DI ROUTINE
	rect(screen_copy, 0, YW-1, XW-1, 0, GREEN); 									//cornice
   	rect(screen_copy, 1, YW-2, XW-2, 1, GREEN); 
   	rect(screen_copy, 2, YW-3, XW-3, 2, GREEN);
   	rectfill(screen_copy, (XW/4), YW, (XW/4)+1, 0, GREEN); 							//retta divisoria
   	rectfill(screen_copy, XN-10, BV, XN-9, AV+10, YELLOW); 							//valore iniziale del noise
	textout_centre_ex(screen_copy, font, "SENSOR NOISE", XN, YN, L_GREEN, BLACK); 				//comandi rumore sensore
   	textout_centre_ex(screen_copy, font, "-", XN-50 , BV-10, YELLOW, BLACK);
   	textout_centre_ex(screen_copy, font, "LEFT", XN-50, BV, YELLOW, BLACK);
   	textout_centre_ex(screen_copy, font, "+", XN+50, BV-10, YELLOW, BLACK);
   	textout_centre_ex(screen_copy, font, "RIGHT", XN+50, BV, YELLOW, BLACK);
   	rect(screen_copy, (XW/8)-90, (YW/8)+10, (XW/4)-10, (YW/8)-55, YELLOW); 
   	textout_centre_ex(screen_copy, font, "DEADLINE MISS", (XW/8), (YW/4), L_GREEN, BLACK); 			//comandi dead line
   	rect(screen_copy, (XW/8)-90, (YW/4)+80, (XW/4)-10, (YW/4)-10, YELLOW); 			
   	textout_ex(screen_copy, font, "Graphic task:", (XW/8)-75, (YW/4)+35, YELLOW, BLACK);
   	textout_ex(screen_copy, font, "Drone task:", (XW/8)-75, (YW/4)+50, YELLOW, BLACK);
   	textout_ex(screen_copy, font, "Control task:", (XW/8)-75, (YW/4)+65, YELLOW, BLACK);
   	textout_ex(screen_copy, font, "PRESS SPACE TO EXIT", (XW/3), (YW/16), YELLOW, BLACK);			//comando uscita
   	textout_ex(screen_copy, font, "PRESS ENTER TO CREATE A DRONE", (XW/2)+80, (YW/16), YELLOW, BLACK);		//comando creazione drone
   
   	
   	//AGGIORNAMENTI GRAFICA
   	write_noise_update(screen_copy);
   	write_deadline_miss(screen_copy);
   	return;
} 
   
   
   
/*------------------------------------------------------*/
/* function init_drone() initializes drone's structure */ 
/*-----------------------------------------------------*/

void init_drone(int i)
{
	drone[i].x_pos = 500;
	drone[i].y_pos = YW;
	drone[i].color = GREEN+i;
	return;
}

/*-------------------------------*/
/*inizializzo il filtro di kalman*/
/*-------------------------------*/
void init_kalman_filter(int i)
{
	kf[i].R = 10;			//rumore di misura
	kf[i].P = 0.9;			//matrice aggiornamento
	kf[i].Q = 0.009;		//disturbo di processo
	
	return;
}

/*------------------------------*/
/* inizializzo control structure*/
/*------------------------------*/    
void init_controller(int i)
{
	controller[i].x_des = 500;
	controller[i].y_des = YW;
	
}     
                                                                                                                                                                                                                                                                       
/*----------------------------------------------------------------------------------------------------------------------*/
/* function kalman_filter() takes x,y noisy mouse coordinates and computes an estimate of them da mandare al controllore*/
/*----------------------------------------------------------------------------------------------------------------------*/

void kalman_filter(int i)
{
	 
	//predizione
	float x_p, y_p, p_p;
	x_p = controller[i].x_des;
	y_p = controller[i].y_des;
	p_p = (kf[i].P) + (kf[i].Q);
	
	//correzione (arriva la nuova misura rumorosa: mouse+Rumore e la uso per correggere la stima del passo sopra)
	float x_c, y_c, p_c, k_c;
	k_c = p_p/(p_p + kf[i].R);
	x_c = x_p + (k_c * (mouse_x + kf[i].R - x_p));
	y_c = y_p + (k_c * (mouse_y + kf[i].R - y_p));
	p_c = ((1 - k_c) * p_p) + (kf[i].Q);

	//quindi aggiorno il parametro della struttura kalman e le coordinate desiderate del drone
	kf[i].K = k_c;
	kf[i].P = p_c;
	
	pthread_mutex_lock(&mux_controller);
	controller[i].x_des = x_c;  //prendo solo la parte intera, tanto disegno per pixel
	controller[i].y_des = y_c;
	pthread_mutex_unlock(&mux_controller);
	return;
	
}

/*--------------------------------------------------------------------------------------------------------------*/
/*funzione che valuta se le coordinate del drone escono dai contorni e in caso affermativo le setta sui contorni*/
/*--------------------------------------------------------------------------------------------------------------*/

void handle_borders(int i)
{
	
	int outl, outr, outt, outb; //out letf, out right, out top, out bottom
	
	pthread_mutex_lock(&mux_drone);
	outl = (drone[i].x_pos <= (XW/4)); 
	outr = (drone[i].x_pos >= XW);
	outt = (drone[i].y_pos <= 0);
	outb = (drone[i].y_pos >= YW);
	
	if (outl) drone[i].x_pos = (XW/4);
	if (outr) drone[i].x_pos = XW;
	if (outt) drone[i].y_pos = 0;
	if (outb) drone[i].y_pos = YW;
	pthread_mutex_unlock(&mux_drone);
	return;
	
}
/*----------------------------------------------------*/
/*funzione che interpreta i tasti premuti dall'utente*/
/*--------------------------------------------------*/

void interpret_command()  				 //se viene premuto un tasto, questo viene salvato in command che è una var globale inizializzata a 0
{
if (keypressed()) {					//se viene premuto un tasto, attraverso keypre e readk salvo il codice del tasto premuto in command
		command = readkey() >> 8;
	switch (command) {
		case KEY_ENTER:
			if (nad < MAX_DRONI)
			{
				task_create(drone_task, nad, 30,30,80);
				nad++;
				
				}
			command = 0;
			break;
		case KEY_RIGHT: 			//utente vuole aumentare il noise
			i_noise++;	
			command = 0;		
			break;
			
		case KEY_LEFT:				//utente vuole diminuire il noise
			j_noise++;
			command = 0; 
			break;
		
		case KEY_SPACE:
		printf("Exiting!\n");
			end = 1;			//end = 1 means che esco dal ciclo del graphic_task e task_drone
			command = 1;			//command = 1 means che esco dal ciclo del control_task
							
			break;
		default:
			printf("key not recognized!\n\n");
			command = 0; 
			break;
	}
	}
	return;
}

/*-----------------------------------------------------------------------------------------------------------------*/
/* function control_drone prende in ingresso xdes e ydes in uscita dal filtro e controlla il drone affinchè vada li*/
/*-----------------------------------------------------------------------------------------------------------------*/

void controller_drone(int i)
{
	float px, py;
	
	//calcolo coordinate che dovrà avere il drone
	px = (0.6951 * controller[i].x_des) + (0.211 * drone[i].x_pos);
	py = (0.6951 * controller[i].y_des) + (0.211 * drone[i].y_pos);
	
	//calcolo angolo del drone
	

	//salvo risultato coordinate drone nella struttura drone, coordinate che verranno poi disegnate
	
	drone[i].x_pos = (int)px;  //prendo solo la parte intera, tanto disegno per pixel
	drone[i].y_pos = (int)py;
	
	return;
	 
}

/*-----------------------------------------------------------------------------------------------------*/
/*function draw_drone(d_drone, screen_copy) draws drone (che è una bitmap d_drone) on bitmap screen_copy*/
/*-----------------------------------------------------------------------------------------------------*/

void draw_drone(BITMAP *d_drone, BITMAP *screen_copy, int i)
{
	float xa, ya, xb, yb, xc, yc, xd, yd, a; 
	a = 0; //il valore dell'angolo lo recupero dalla struttura del drone e me lo calcola il controllore
	
	xa = (XD/2) - (cos(a)*(BMAX/2)) + (sin(a)* (H/2));	//coordinate trapezio all'interno della bitmap d_drone
	ya = (YD/2) + (sin(a)*(BMAX/2)) + (cos(a) * (H/2));
	xb = (XD/2) - (cos(a)*(BMIN/2)) - (sin(a) * (H/2));
	yb = (YD/2) + (sin(a)*(BMIN/2)) - (cos(a) * (H/2));
	xc = (XD/2) + (cos(a)*(BMIN/2)) - (sin(a) * (H/2));
	yc = (YD/2) - (sin(a)*(BMIN/2)) - (cos(a) * (H/2));
	xd = (XD/2) + (cos(a)*(BMAX/2)) + (sin(a) * (H/2));
	yd = (YD/2) - (sin(a)*(BMAX/2)) + (cos(a) * (H/2));
	int points[8] = {xa, ya, xb, yb, xc, yc, xd, yd}; 					
	polygon(d_drone, 4, points, drone[i].color); 
	
	blit(d_drone, screen_copy, 0, 0, drone[i].x_pos, drone[i].y_pos, XD, YD);
	
	return;
}

/*---------------------------------------------------------*/
/* Task drone_task manages drone's coordinates every period*/
/*---------------------------------------------------------*/

void *drone_task(void *arg)
{	
	int a = get_task_index(arg);
	init_drone(a);
	init_kalman_filter(a);
	init_controller(a);
	set_activation(a);
	while (!end) {
		
		pthread_mutex_lock(&mux_kalman);
		kalman_filter(a);
		change_param_filter();
		pthread_mutex_unlock(&mux_kalman); 
		
		pthread_mutex_lock(&mux_drone); 
		controller_drone(a);
		pthread_mutex_unlock(&mux_drone);
		
		handle_borders(a);
		
		if (deadline_miss(a)){
			 printf("drone_task deadline miss %d\n", param[a].dmiss);
			 dlm_drone++;
			 }
		wait_for_activation(a);
	
	}
	return NULL;
	
}


/*----------------------------------------------------------------------------------------------------------------------------------------------*/
/*Graphic task, gestisce la grafica ogni periodo (20 ms) (ovvero disegna il drone e disegna la grafica di routine, con i relativi aggiornamenti)*/
/*----------------------------------------------------------------------------------------------------------------------------------------------*/

void *graphic_task(void *arg)
{
	int i;
	BITMAP *screen_copy; 						//creo le bitmap per evitare lo sfarfallio
	BITMAP *d_drone;
	screen_copy = create_bitmap(XW, YW);
	d_drone = create_bitmap(XD, YD);
	clear_to_color(screen_copy, BLACK);
	clear_to_color(d_drone, BLACK);
	clear_to_color(screen_copy, BLACK);
	set_activation(GRAPHIC_TASK_INDEX);
	while (!end) {  						//end globalmente = 0, questo task termina quando end = 1; (ovvero premendo SPACE, grazie alla funzione interpret_command)	
		scare_mouse();
		clear_to_color(screen_copy, BLACK);			//per non avere la scia, ogni periodo faccio tutto nero e ridisegno
		pthread_mutex_lock(&mux_drone);
		for(i = 0; i<MAX_DRONI; i++){
			draw_drone(d_drone, screen_copy, i); 			//disegno i droni
		}
		pthread_mutex_unlock(&mux_drone);
      		write_graphic(screen_copy);				//disegno la grafica di routine e i relativi aggiornamenti
      		blit(screen_copy, screen, 0, 0, 0, 0, XW, YW);	//copio tutto sullo screen
            	unscare_mouse();
            	
            	if (deadline_miss(GRAPHIC_TASK_INDEX)) {
            		printf("graphic_task deadline miss %d\n", param[GRAPHIC_TASK_INDEX].dmiss);
            		dlm_graphic++;
            		}
		wait_for_activation(GRAPHIC_TASK_INDEX);
	}
	return NULL;
}


/*--------------------------------------------------------------------------*/
/* control_task ogni 20 millisecondi interpreta il tasto premuto dall'utente*/
/*--------------------------------------------------------------------------*/

void *control_task(void *arg)
{
	
	set_activation(CONTROL_TASK_INDEX);
	while (command == 0){ 
	
		interpret_command();  
		 				
	        if (deadline_miss(CONTROL_TASK_INDEX)) {
	        	printf("control_task deadline miss %d\n", param[CONTROL_TASK_INDEX].dmiss);
	        	dlm_control++;
	        	}
	        wait_for_activation(CONTROL_TASK_INDEX);
}
	return NULL;

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
int main()
{ 

	init(); 
	
	//creo i 2 task
	task_create(graphic_task, GRAPHIC_TASK_INDEX, 50, 50,80);
	task_create(control_task, CONTROL_TASK_INDEX, 30, 30, 50); //qui dentro creo il terzo task
	//i task vengono terminati dall'utente se preme SPACE perchè questo mi fa uscire dai cicli
	wait_for_task(CONTROL_TASK_INDEX);
	//pthread_mutexattr_destroy(&mux_attr); //loro non so dove vanno messi
	//pthread_mutex_destroy(&mux_drone);
	//pthread_mutex_destroy(&mux_kalman);
	allegro_exit();
    	return 0;
}
END_OF_MAIN();

