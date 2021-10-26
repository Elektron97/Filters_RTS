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
#define BLACK 0							// 8 bit colors
#define YELLOW 14		
#define GREEN 2
#define WHITE 15
#define GRAY 8
#define L_GRAY 7
#define L_CYAN 11
#define RED 4
#define L_BLUE 9
#define L_RED 15
#define L_GREEN 10
#define MAGENTA 13
#define BROWN 6
#define XW 800							//x window resolution
#define YW 700							//y window resolution
#define XO 160							//x oscilloscope resolution
#define YO 140							//y oscilloscope resolution
#define XOP 20							//x oscilloscope position
#define YO1P YW/2						//y oscilloscope 1 position
#define YO2P (YW/2)+150					//y oscilloscope 2 position
#define BMAX 28							//drone longer lenght
#define H 8								//drone height
#define BMIN 18							//drone shorter lenght
#define XI 500							//drone initial coordinates
#define XN 100							//noise coordinates
#define YN 46
#define BV 80
#define AV 65
#define TL 20							//trail lenght
#define ZOOM_W 200						//zoom window
#define ZMAX 40							//zoom levels
#define ZMEDIUM 60
#define ZMIN 80
#define NR 1.5							//noise radius constant

/*KALMAN FILTER CONSTANTS*/
#define P0 0.9							//covariance matrix initial value
#define R0 15							//measurement noise initial value
#define Q0 0.009						//process noise

/*CONTROLLER CONSTANTS*/
#define KD1 0.999						//y controller gain
#define KP1 0.222
#define B1 1.14							//discretization coefficients
#define B2 0.306
#define K2 0.6951						//x controller gain
#define K3 0.211
#define DELTA_T 10						//time interval

/* MOTOR CONSTANTS*/	
#define K 0.5							

/*LEADER CONSTANTS*/
#define NO_LEADER 10					//no drone leader
#define LEADER_1 0						//number of drone leader
#define LEADER_2 1
#define LEADER_3 2
#define LEADER_4 3
#define LEADER_5 4
#define LEADER_6 5

/*THREAD CONSTANTS*/
#define _GNU_SOURCE						
#define NT 10							//max number of tasks
#define MAX_DRONI 6						//max number of drones
#define GRAPHIC_TASK_INDEX 6
#define USER_TASK_INDEX 7
#define PERIOD1 50						//tasks periods
#define PERIOD2 30
#define DREL1 50						//relative deadline
#define DREL2 30
#define PRIO1 23						//priority
#define PRIO2 20

/*FUNCTION PROTOTYPES*/
void init();								
void write_noise_update(BITMAP *screen_copy);  			
void change_param_filter();						
void show_mouse_noisy(BITMAP *sceen_copy);				
void write_deadline_miss(BITMAP *screen_copy);			
void write_toggle_update(BITMAP *screen_copy);			
void write_graphic(BITMAP *screen_copy);				
void init_drone(int i);						
void init_kalman_filter(int i);					
void init_controller(int i);
void init_motor(int i);						
void kalman_filter(int i);						
void handle_borders(int i);						
void interpret_command();						
void controller_drone(int i);						
void motor_drone(int i);
void computes_coordinates(int i);
void store_trail(int i);						
void draw_trail(BITMAP *screen_copy, int i);				
void draw_drone(BITMAP *screen_copy, int i);				
void draw_osc_1(BITMAP *osc_1, BITMAP *screen_copy);			
void draw_osc_2(BITMAP *osc_2, BITMAP *screen_copy);			
void change_leader(int i);						
void zoom(BITMAP *screen_copy);					
void *drone_task(void *arg);						
void *graphic_task(void *arg);					
void *user_task(void *arg);						
int main();

/* DRONE STRUCTURE*/
struct drone {
	int x_pos_now;		//actual position	
	int y_pos_now;
	int x_pos_prev;		//previous position		
	int y_pos_prev;
	int color;
	int leader;				
};

struct drone drone[MAX_DRONI];

/*CONTROL STRUCTURE*/
struct control {
	double x_des;		//desired coordinates
	double y_des;	
};

struct control controller[MAX_DRONI];

/* KALMAN FILTER STRUCTURE*/
struct k_f{
	double R;		//measurement noise
	double P;		//covariance matrix
	double Q;		//process noise
};

struct k_f kf[MAX_DRONI];

/*MOTOR STRUCTURE*/
struct drone_motor{
	float T_des;		//desired motor thrust
	float T_eff;		//effective motor thrust
	float T_eff_prev;	//previous effective motor thrust
}; 

struct drone_motor motor[MAX_DRONI];

/* TRAIL STRUCTURE*/
struct circular_buffer{
	int top;		//current element
	int x[TL];		
	int y[TL];				
};

struct circular_buffer trail[MAX_DRONI];

/* GLOBAL VARIABLES*/
int	end = 0; 				//end flag for drone_task and graphic_task termination	
int command = 0; 			//end flag for user_task termination
int	nad = 0;				//number of active drones
int dlm_graphic = 0;		//number of graphic_task deadline missed
int dlm_drone = 0;			//number of drone_task deadline missed
int dlm_user = 0;			//number of user_task deadline missed
int i_noise = 0;			//setting noise index
int	j_noise = 0;
int	noise_activated = 0;  	//mouse noise flag
int	trail_activated = 0;	//trail flag
int	zoom_in = 0;			//setting noise index
int	zoom_out = 0;
int	sym_time_1 = 1; 		//simulated time oscilloscope 1
int	sym_time_2 = 1;			//simulated time oscilloscope 2
int	lead = NO_LEADER;		//drone leader number
int	lead_activated = 0;		//leader flag

/*SEMAPHORES*/
pthread_mutex_t mux_kalman = PTHREAD_MUTEX_INITIALIZER; 	//mutex kalman filter data
pthread_mutex_t mux_drone = PTHREAD_MUTEX_INITIALIZER;		//mutex drone data
pthread_mutex_t mux_trail = PTHREAD_MUTEX_INITIALIZER; 		//mutex trail data

/*-----------------------------------*/
/*Function init() initializes allegro*/
/*-----------------------------------*/

void init()
{
	allegro_init();
	install_keyboard();
	install_mouse();
	set_color_depth(8);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED,XW, YW, BLACK,BLACK);
	clear_to_color(screen,0);
	show_mouse(screen);	
	
	return;
 }
   
/*----------------------------------------------------*/
/*Function write_noise_update() sets noise graphically*/
/*----------------------------------------------------*/

void write_noise_update (BITMAP *screen_copy)
{
int k;
	k = i_noise - j_noise;
 	if (k == 1){								//2 tacche
                scare_mouse();
                rectfill(screen_copy, XN-10, BV, XN+11, AV-10,BLACK);
                rectfill(screen_copy, XN-10, BV, XN-9, AV+10, WHITE);
                rectfill(screen_copy, XN-5, BV, XN-4, AV+5, WHITE);
                unscare_mouse();
	}
	else if (k == 2){							//3 tacche
                scare_mouse();
                rectfill(screen_copy, XN-10, BV, XN+11, AV-10,BLACK);
                rectfill(screen_copy, XN-10, BV, XN-9, AV+10, WHITE);
                rectfill(screen_copy, XN-5, BV, XN-4, AV+5, WHITE);
                rectfill(screen_copy, XN, BV, XN+1, AV, WHITE);
                unscare_mouse();
	}
	else if( k == 3){							//4 tacche
                scare_mouse();
                rectfill(screen_copy, XN-10, BV, XN+11, AV-10,BLACK);
                rectfill(screen_copy, XN-10, BV, XN-9, AV+10, WHITE);
                rectfill(screen_copy, XN-5, BV, XN-4, AV+5, WHITE);
                rectfill(screen_copy, XN, BV, XN+1, AV, WHITE);
                rectfill(screen_copy, XN+5, BV, XN+6, AV-5, WHITE);
                unscare_mouse();
	}
	else if( k == 0){							//1 tacca (initial value)
                scare_mouse();
                rectfill(screen_copy, XN-10, BV, XN+11, AV-10,BLACK);
                rectfill(screen_copy, XN-10, BV, XN-9, AV+10, WHITE);
                unscare_mouse();
	}
	else if ( k >= 4){ 							//reached maximum noise (4 tacche)
		i_noise = 3;
		j_noise = 0;
	}
	else if ( k <= 0){  						//reached minimun noise (1 tacca)
		i_noise = 0;
		j_noise = 0;
	}
	return;
}

/*---------------------------------------------------------------*/
/* Function change_param_filter() sets sensors noise numerically*/
/*--------------------------------------------------------------*/

void change_param_filter()
{
int k, i;
	k = i_noise - j_noise;
	if (k == 1){
		for(i = 0; i<MAX_DRONI; i++){
        		kf[i].R = 20+(20*i);			//2 tacche
        	}
	}
	else if (k == 2){
		for(i = 0; i<MAX_DRONI; i++){
            		kf[i].R = 30+(30*i);		//3 tacche
            	}
	}
	else if( k == 3){
		for(i=0; i<MAX_DRONI; i++){
			kf[i].R = 40+(40*i);				//4 tacche
               }
	} 
	else if( k == 0){
		for(i = 0; i<MAX_DRONI; i++){
            		kf[i].R = 10+(10*i);		//1 tacca (initial value)
            	} 
	}
	else if ( k >= 4){ 							//reached maximum noise value (4 tacche)
		i_noise = 3;
		j_noise = 0;
	} 
	else if ( k <= 0){  						//reached minimum noise value (1 tacca)
		i_noise = 0;
		j_noise = 0;
	}
	
	return;
}

/*--------------------------------------------------------*/
/* Function show_mouse_noisy() shows mouse noisy position*/
/*-------------------------------------------------------*/

void show_mouse_noisy(BITMAP *screen_copy)
{
	if (noise_activated == 1){
		int r_noise;
		r_noise = NR * kf[0].R;       				//noise radius
		circle(screen_copy, mouse_x, mouse_y, r_noise, WHITE);
	}
	return;
}

/*------------------------------------------------------------------*/
/* Function write_deadline_miss() shows number of deadline missed  */
/*-----------------------------------------------------------------*/

void write_deadline_miss(BITMAP *screen_copy)
{
char s1[10], s2[10], s3[10];
	sprintf(s1, "%d", dlm_graphic);
	sprintf(s2, "%d", dlm_drone);
	sprintf(s3, "%d", dlm_user);	
	textout_ex(screen_copy, font, s1, (XW/8)+45, (YW/4)+35, WHITE, BLACK);
	textout_ex(screen_copy, font, s2, (XW/8)+45, (YW/4)+50, WHITE, BLACK);
	textout_ex(screen_copy, font, s3, (XW/8)+45, (YW/4)+65, WHITE, BLACK);
	
	return;
}

/*-----------------------------------------------------------*/
/* Function write_toggle_update() shows ON/OFF of toggle keys*/
/*-----------------------------------------------------------*/

 void write_toggle_update(BITMAP *screen_copy)
{
int zoom_level = zoom_in - zoom_out;
	if (noise_activated == 0) textout_ex(screen_copy, font, "OFF", (XW/2)+330, (YW/40), L_RED, BLACK);		//noise toggle key
 	else textout_ex(screen_copy, font, "ON", (XW/2)+330, (YW/40), L_RED, BLACK);
 	
 	if (trail_activated == 0) textout_ex(screen_copy, font, "OFF", (XW/2)+330, (YW/20), L_RED, BLACK);		//trail toggle key
 	else textout_ex(screen_copy, font, "ON", (XW/2)+330, (YW/20), L_RED, BLACK);
 	
 	if (zoom_level < 1) textout_ex(screen_copy, font, "OFF", (XW/2)+330, (YW/20)+17, L_RED, BLACK);		//zomm toggle key
 	else textout_ex(screen_copy, font, "ON", (XW/2)+330, (YW/20)+17, L_RED, BLACK);
 
  	return; 	
}

/*--------------------------------------------------------------------*/
/*Function write_graphic() manages graphic routine and graphic updates*/
/*--------------------------------------------------------------------*/

void write_graphic(BITMAP *screen_copy)
{
	//graphic routine
	rect(screen_copy, 0, YW-1, XW-1, 0, L_RED); 								//frame
   	rect(screen_copy, 1, YW-2, XW-2, 1, L_RED); 
   	rect(screen_copy, 2, YW-3, XW-3, 2, L_RED);
   	rectfill(screen_copy, (XW/4), YW, (XW/4)+1, (YW/8)+19, L_RED); 				//vertical dividing line
   	rectfill(screen_copy, 0, (YW/8)+20, XW, (YW/8)+19, L_RED);					//horizontal dividing line
   	rectfill(screen_copy, XN-10, BV, XN-9, AV+10, WHITE); 						//initial noise value
	textout_centre_ex(screen_copy, font, "SENSOR NOISE", XN, (YW/40), L_RED, BLACK); 				//sensor noise 
   	textout_centre_ex(screen_copy, font, "-", XN-50 , BV-30, WHITE, BLACK);
   	textout_centre_ex(screen_copy, font, "LEFT", XN-50, BV-20, WHITE, BLACK);
   	textout_centre_ex(screen_copy, font, "+", XN+50, BV-30, WHITE, BLACK);
   	textout_centre_ex(screen_copy, font, "RIGHT", XN+50, BV-20, WHITE, BLACK); 
   	textout_centre_ex(screen_copy, font, "DEADLINES MISSED", (XW/8), (YW/4), L_RED, BLACK); 		//deadline
   	rect(screen_copy, (XW/8)-90, (YW/4)+80, (XW/4)-10, (YW/4)-10, WHITE); 			
   	textout_ex(screen_copy, font, "Graphic task:", (XW/8)-75, (YW/4)+35, WHITE, BLACK);
   	textout_ex(screen_copy, font, "Drone task:", (XW/8)-75, (YW/4)+50, WHITE, BLACK);
   	textout_ex(screen_copy, font, "User task:", (XW/8)-75, (YW/4)+65, WHITE, BLACK);
   	textout_ex(screen_copy, font, "PRESS SPACE TO EXIT", (XW/3.5), (YW/40), WHITE, BLACK);			//exit 
   	textout_ex(screen_copy, font, "PRESS ENTER TO CREATE A DRONE", (XW/3.5), (YW/20), WHITE, BLACK);//create drone
   	textout_ex(screen_copy, font, "N = MOUSE NOISE", (XW/2)+120, (YW/40), WHITE, BLACK);			//mouse noise
   	textout_ex(screen_copy, font, "T = TRAIL", (XW/2)+120, (YW/20), WHITE, BLACK);					//trail
   	textout_ex(screen_copy, font, "I/O = ZOOM IN/OUT", (XW/2)+120, (YW/20)+17, WHITE, BLACK);
   	textout_ex(screen_copy, font, "SELECT LEADER", (XW/2)+120, (YW/20)+34, WHITE, BLACK);			//leader
   	textout_ex(screen_copy, font, "1", (XW/2)+300, (YW/20)+34, L_BLUE, BLACK);
   	textout_ex(screen_copy, font, "2", (XW/2)+315, (YW/20)+34, L_GREEN, BLACK);
   	textout_ex(screen_copy, font, "3", (XW/2)+330, (YW/20)+34, L_CYAN, BLACK);
   	textout_ex(screen_copy, font, "4", (XW/2)+345, (YW/20)+34, L_RED, BLACK);
   	textout_ex(screen_copy, font, "5", (XW/2)+360, (YW/20)+34, MAGENTA, BLACK);
   	textout_ex(screen_copy, font, "6", (XW/2)+375, (YW/20)+34, YELLOW, BLACK);
   	 	
   	//graphic updates
   	write_noise_update(screen_copy);
   	write_toggle_update(screen_copy);
   	write_deadline_miss(screen_copy);
   	
   	return;
} 
   
/*----------------------------------------------------*/
/* Function init_drone() initializes drone's structure*/ 
/*----------------------------------------------------*/

void init_drone(int i)
{
	drone[i].x_pos_now = XI;
	drone[i].y_pos_now = YW;
	drone[i].x_pos_prev = 0;
	drone[i].y_pos_prev = 0;
	drone[i].color = L_BLUE+i;
	drone[i].leader = 1;
	return;	
}

/*--------------------------------------------------------*/
/*Function init_kalman_filter() initializes kalman filter*/
/*-------------------------------------------------------*/

void init_kalman_filter(int i)
{
	kf[i].R = R0 + (10*i);		
	kf[i].P = P0;			
	kf[i].Q = Q0;		
	return;
}

/*-------------------------------------------------*/
/*Function init_controller() initializes controllers*/
/*-------------------------------------------------*/    

void init_controller(int i)
{
	controller[i].x_des = XI;
	controller[i].y_des = YW;
	return;
}     
 
/*----------------------------------------*/
/*Function init_motor() initializes motors*/
/*----------------------------------------*/

void init_motor(int i)
{
	motor[i].T_des = 0;
	motor[i].T_eff = 0;
	motor[i].T_eff_prev = 0;
	return;
}
                                                                                                                                                                                                                                                                     
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Function kalman_filter() takes noisy mouse coordinates (or drone leader coordinates if it exists) and computes an estimate of them (controller.x_des and controller.y_des)*/
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void kalman_filter(int i)
{	 
	//prediction
	float x_p, y_p, p_p;
	x_p = controller[i].x_des;
	y_p = controller[i].y_des;
	p_p = (kf[i].P) + (kf[i].Q);
	
	//correction
	float x_c, y_c, p_c, k_c;
	if ( drone[i].leader == 1){ 					//correction with mouse coordinates	
		k_c = p_p/(p_p + kf[i].R);
		x_c = x_p + (k_c * (mouse_x - x_p));
		y_c = y_p + (k_c * (mouse_y - y_p));
		p_c = ((1 - k_c) * p_p);
	}
	else {
		k_c = p_p/(p_p + kf[i].R);				//correction with leader coordinates
		x_c = x_p + (k_c * (drone[lead].x_pos_now - x_p));
		y_c = y_p + (k_c * (drone[lead].y_pos_now - y_p));
		p_c = p_p - ((k_c * k_c) * (kf[i].R + p_p));
	}
	
	//update
	kf[i].P = p_c;
	controller[i].x_des = x_c; 
	controller[i].y_des = y_c;
	return;	
}

/*---------------------------------------------------------------------------------------------------*/
/*Function handles_borders() cheks if drone's coordinates exceed borders and than set them on borders*/
/*---------------------------------------------------------------------------------------------------*/

void handle_borders(int i)
{
int outl, outr, outt, outb; 					//out letf, out right, out top, out bottom
	pthread_mutex_lock(&mux_drone);
	outl = (drone[i].x_pos_now - (BMAX/2) <= (XW/4)); 
	outr = (drone[i].x_pos_now + (BMAX/2) >= XW);
	outt = (drone[i].y_pos_now - (H/2) <= ((YW/8)+20));
	outb = (drone[i].y_pos_now + (H/2) >= YW);
	
	if (outl) drone[i].x_pos_now = (XW/4) + (BMAX/2);
	if (outr) drone[i].x_pos_now = XW - (BMAX/2);
	if (outt) drone[i].y_pos_now = ((YW/8)+20) + (H/2);
	if (outb) drone[i].y_pos_now = YW - (H/2);
	pthread_mutex_unlock(&mux_drone);
	
	return;	
}

/*------------------------------------------------------*/
/*Function interpret_command() interprets user's command*/
/*------------------------------------------------------*/

void interpret_command()  				
{
	if (keypressed()) {					 
		command = readkey() >> 8;
		switch (command) {
			case KEY_ENTER:							
				if (nad < MAX_DRONI){
					task_create(drone_task, nad, PERIOD2, DREL2, PRIO1);		//create a new drone
					nad++;
				}
				else printf("max number of droni reached\n");
				command = 0;
				break;
			case KEY_RIGHT: 													//increase noise
				i_noise++;	
				command = 0;		
				break;	
			case KEY_LEFT:														//decrease noise
				j_noise++;
				command = 0; 
				break;			
			case KEY_N:
				if ( noise_activated == 0) noise_activated = 1; 				//enable/disable mouse noise
				else noise_activated = 0;
				command = 0;
				break;		
			case KEY_T:
				if (trail_activated == 0) trail_activated = 1;					//enable/disable trail
				else trail_activated = 0;
				command = 0;
				break;			
			case KEY_I:
				zoom_in++;														//increase zoom
				command = 0;
				break;			
			case KEY_O:
				zoom_out++;								//decrease zoom
				command = 0;
				break;	
			case KEY_1:	
				if (lead_activated == 0){
					lead_activated = 1;					//set drone number 1 as leader
					if (nad > 0) lead = LEADER_1;
					else lead = NO_LEADER;
				}
				else {
					lead_activated = 0;
					lead = NO_LEADER;
				}
				command = 0;
				break;
			case KEY_2:
				if (lead_activated == 0){
					lead_activated = 1;					//set drone number 2 as leader
					if ( nad > 1) lead = LEADER_2;
					else lead = NO_LEADER;
				} 
				else {
					lead_activated = 0;
					lead = NO_LEADER;
				}
				command = 0;
				break;
			case KEY_3:
				if (lead_activated == 0){
					lead_activated = 1;					//set drone number 3 as leader
					if(nad > 2) lead = LEADER_3;
					else lead = NO_LEADER;
				} 
				else {
					lead_activated = 0;
					lead = NO_LEADER;
				}
				command = 0;
				break;
			case KEY_4:
				if (lead_activated == 0){
					lead_activated = 1;	
					if(nad > 3) lead = LEADER_4;
					else lead = NO_LEADER;				//set drone number 4 as leader
				}
				else {
					lead_activated = 0;
					lead = NO_LEADER;
				}
				command = 0;
				break;
			case KEY_5:
				if (lead_activated == 0){
					lead_activated = 1;					//set drone number 5 as leader
					if (nad > 4) lead = LEADER_5;
					else lead = NO_LEADER;
				}
				else {
					lead_activated = 0;
					lead = NO_LEADER;
				}
				command = 0;
				break;
			case KEY_6:
				if (lead_activated == 0){
					lead_activated = 1;					//set drone number 6 as leader
					if (nad > 5) lead = LEADER_6;
					else lead = NO_LEADER;
				} 
				else {
					lead_activated = 0;
					lead = NO_LEADER;
				}
				command = 0;
				break;		
			case KEY_SPACE:
				printf("Exiting!\n");
				end = 1;								//graphic_task and drone_task termination
				command = 1;							//user_task termination							
				break;
			default:
				printf("key not recognized!\n\n");
				command = 0; 
				break;
		}
	}
	return;
}

/*-----------------------------------------------------------------------------------------------*/
/* Function control_drone() takes controller.y_des coordinate and computes necessary motor thrust*/
/*-----------------------------------------------------------------------------------------------*/

void controller_drone(int i)
{
	motor[i].T_des = (KD1 * ((drone[i].y_pos_now - drone[i].y_pos_prev) / DELTA_T )) + (KP1 * drone[i].y_pos_now ) - (KP1 * controller[i].y_des); 
	
	return;	 
}

/*-----------------------------------------------------------------------------------*/
/* Function motor_drone() takes desired motor thrust and computes real motor thrust  */
/*-----------------------------------------------------------------------------------*/

void motor_drone(int i)
{
	motor[i].T_eff = (K * motor[i].T_des) + ((1 - K) * motor[i].T_eff_prev);	//effective motor thrust
	motor[i].T_eff_prev = motor[i].T_eff;										//store value
		
	return;
}

/*------------------------------------------------------------*/
/*Function computes_coordinates() computes drone's coordinates*/
/*------------------------------------------------------------*/

void computes_coordinates(int i)
{
float y, py, px;
	y = (motor[i].T_eff + (KD1 * ((drone[i].y_pos_now - drone[i].y_pos_prev )/ DELTA_T)) + (KP1 * controller[i].y_des))/((KD1 / DELTA_T) + KP1);
	py = (B1 * y ) + (B2 * controller[i].y_des);  					//drone y position
	px  = (K2 * controller[i].x_des) + (K3 * drone[i].x_pos_now); 	//drone x position
	drone[i].x_pos_prev = drone[i].x_pos_now;  						//previous values saved
	drone[i].y_pos_prev = drone[i].y_pos_now;
	drone[i].x_pos_now = (int)px;  									//actual position
	drone[i].y_pos_now = (int)py;
	return;
}

/*-----------------------------------------------*/
/* Function store_trail() stores trail of drone i*/
/*-----------------------------------------------*/

void store_trail(int i)
{
int k;
	if (i >= MAX_DRONI) return;
	k = trail[i].top;
	k = (k + 1) % TL;
	pthread_mutex_lock(&mux_trail); 
	trail[i].x[k] = drone[i].x_pos_now;
	trail[i].y[k] = drone[i].y_pos_now;
	pthread_mutex_unlock(&mux_trail); 
	trail[i].top = k;
	return;
}

/*--------------------------------------------*/
/*Function draw_trail() draws trail of drone i*/
/*--------------------------------------------*/

void draw_trail(BITMAP *screen_copy, int i)
{
	if (trail_activated == 1){
		int j, k;  					//trail indexes
		int x, y; 
		for (j = 0; j<TL; j++){  			//draw trail made by TL past values
			k = (trail[i].top + TL - j ) % TL;
			pthread_mutex_lock(&mux_trail); 
			x = trail[i].x[k];
			y = trail[i].y[k];
			pthread_mutex_unlock(&mux_trail); 
			putpixel(screen_copy, x, y, YELLOW);
		}
	}
	return;
}

/*--------------------------------------------------------------------------------------------*/
/*Function draw_drone() draws drone on bitmap screen_copy and draws a circle around the leader*/
/*--------------------------------------------------------------------------------------------*/

void draw_drone(BITMAP *screen_copy, int i)
{
float xa, ya, xb, yb, xc, yc, xd, yd;
	xa = drone[i].x_pos_now - (BMAX/2);
	ya = drone[i].y_pos_now + (H/2);
	xb = drone[i].x_pos_now - (BMIN/2);
	yb = drone[i].y_pos_now - (H/2);
	xc = drone[i].x_pos_now + (BMIN/2);
	yc = drone[i].y_pos_now - (H/2);
	xd = drone[i].x_pos_now + (BMAX/2);
	yd = drone[i].y_pos_now + (H/2);
	int points[8] = {xa, ya, xb, yb, xc, yc, xd, yd}; 					
	polygon(screen_copy, 4, points, drone[i].color); 						//draws drone
	
	if (drone[i].leader == 1 && lead != NO_LEADER ) {						//if i drone is the leader draws a circle around it
		circle(screen_copy, drone[i].x_pos_now, drone[i].y_pos_now, BMAX/1.5, WHITE);
	}
	return;
}

/*--------------------------------------------*/
/* Function draw_osc_1() draws oscilloscope 1*/
/*-------------------------------------------*/

void draw_osc_1(BITMAP *osc_1, BITMAP *screen_copy)
{
	textout_ex(screen_copy, font, "X mouse (t)", (XO/2)-20 , (YW/2)-25, WHITE, BLACK);	
	textout_ex(screen_copy, font, "X drone (t)", (XO/2)-20 , (YW/2)-10, L_RED, BLACK);
	rect(osc_1, 1, YO-1, XO-1, 0, WHITE);						//main frame	
	line(osc_1, XO/4, 2, XO/4, YW-2, GRAY);						//vertical lines
	line(osc_1, XO/2, 2, XO/2, YW-2, GRAY);
	line(osc_1, (XO*3)/4, 2, (XO*3)/4, YW-2, GRAY);
	line(osc_1, 2, YO/4, XO-2, YO/4, GRAY);						//horizontal lines
	line(osc_1, 2, YO/2, XO-2, YO/2, GRAY);
	line(osc_1, 2, (YO*3)/4, XO-2, (YO*3)/4, GRAY);
	putpixel(osc_1, sym_time_1, ((YO - (mouse_x * YO)/XW)), WHITE); 				//draws mouse x position
	putpixel(osc_1, sym_time_1, ((YO - (drone[0].x_pos_now * YO)/XW)), L_BLUE);		//draws drone x position
			
	if(sym_time_1 <= XO ){										//time counter
		sym_time_1++;
	}
	else {
		sym_time_1 = 1;
		rectfill(osc_1, 2, YO-2, XO-2, 2, BLACK);
	}
	blit(osc_1, screen_copy, 0, 0, XOP, YO1P , XO, YO);
	
	return;
}

/*--------------------------------------------*/
/* Function draw_osc_2() draws oscilloscope 2*/
/*-------------------------------------------*/

void draw_osc_2(BITMAP *osc_2, BITMAP *screen_copy)
{
	textout_ex(screen_copy, font, "Y mouse (t)", (XO/2)-20 , (YW/2)+295, WHITE, BLACK);
	textout_ex(screen_copy, font, "Y drone (t)", (XO/2)-20 , (YW/2)+310, L_RED, BLACK);
	rect(osc_2, 1, YO-1, XO-1, 0, WHITE);								//main frame
	line(osc_2, XO/4, 2, XO/4, YW-2, GRAY);								//vertical lines
	line(osc_2, XO/2, 2, XO/2, YW-2, GRAY);
	line(osc_2, (XO*3)/4, 2, (XO*3)/4, YW-2, GRAY);
	line(osc_2, 2, YO/4, XO-2, YO/4, GRAY);								//horizontal lines
	line(osc_2, 2, YO/2, XO-2, YO/2, GRAY);
	line(osc_2, 2, (YO*3)/4, XO-2, (YO*3)/4, GRAY);
	putpixel(osc_2, sym_time_2, ((mouse_y * YO)/YW), WHITE); 							//draw mouse y position
	putpixel(osc_2, sym_time_2, ((drone[0].y_pos_now * YO)/YW), L_BLUE);				//draw drone y position
			
	if(sym_time_2 <= XO ){												//time counter
		sym_time_2++;
	} 
	else {
		sym_time_2 = 1;
		rectfill(osc_2, 2, YO-2, XO-2, 2, BLACK);
	}
	blit(osc_2, screen_copy, 0, 0, XOP, YO2P, XO, YO);
	
	return;
}

/*--------------------------------------------*/
/*Function zoom() manages zoom in and zoom out*/
/*--------------------------------------------*/

void zoom(BITMAP *screen_copy)
{
int zoom_level;
	zoom_level = zoom_in - zoom_out;	
	if (zoom_level == 1){
		rect(screen, mouse_x - ((ZOOM_W/2) + 1), mouse_y + ((ZOOM_W/2) + 1), mouse_x + ((ZOOM_W/2) + 1), mouse_y - ((ZOOM_W/2) + 1), L_RED);
		rect(screen, mouse_x - ((ZOOM_W/2) + 2), mouse_y + ((ZOOM_W/2) + 2), mouse_x + ((ZOOM_W/2) + 2), mouse_y - ((ZOOM_W/2) + 2), L_RED);
		stretch_blit(screen_copy, screen, mouse_x, mouse_y, ZMIN, ZMIN, mouse_x - (ZOOM_W/2), mouse_y - (ZOOM_W/2), ZOOM_W, ZOOM_W);
	}
	else if(zoom_level == 2){
		rect(screen, mouse_x - ((ZOOM_W/2) + 1), mouse_y + ((ZOOM_W/2) + 1), mouse_x + ((ZOOM_W/2) + 1), mouse_y - ((ZOOM_W/2) + 1), L_RED);
		rect(screen, mouse_x - ((ZOOM_W/2) + 2), mouse_y + ((ZOOM_W/2) + 2), mouse_x + ((ZOOM_W/2) + 2), mouse_y - ((ZOOM_W/2) + 2), L_RED);
		stretch_blit(screen_copy, screen, mouse_x, mouse_y, ZMEDIUM, ZMEDIUM, mouse_x - (ZOOM_W/2), mouse_y - (ZOOM_W/2), ZOOM_W, ZOOM_W);
	}
	else if(zoom_level == 3){
		rect(screen, mouse_x - ((ZOOM_W/2) + 1), mouse_y + ((ZOOM_W/2) + 1), mouse_x + ((ZOOM_W/2) + 1), mouse_y - ((ZOOM_W/2) + 1), L_RED);
		rect(screen, mouse_x - ((ZOOM_W/2) + 2), mouse_y + ((ZOOM_W/2) + 2), mouse_x + ((ZOOM_W/2) + 2), mouse_y - ((ZOOM_W/2) + 2), L_RED);
		stretch_blit(screen_copy, screen, mouse_x, mouse_y, ZMAX, ZMAX, mouse_x - (ZOOM_W/2), mouse_y - (ZOOM_W/2), ZOOM_W, ZOOM_W);
	}
	else if (zoom_level > 3){
		rect(screen, mouse_x - ((ZOOM_W/2) + 1), mouse_y + ((ZOOM_W/2) + 1), mouse_x + ((ZOOM_W/2) + 1), mouse_y - ((ZOOM_W/2) + 1), L_RED);
		rect(screen, mouse_x - ((ZOOM_W/2) + 2), mouse_y + ((ZOOM_W/2) + 2), mouse_x + ((ZOOM_W/2) + 2), mouse_y - ((ZOOM_W/2) + 2), L_RED);
		stretch_blit(screen_copy, screen, mouse_x, mouse_y, ZMAX, ZMAX, mouse_x - (ZOOM_W/2), mouse_y - (ZOOM_W/2), ZOOM_W, ZOOM_W);  		
		zoom_in = 3;
		zoom_out = 0;
	}
	else if (zoom_level < 1){
		zoom_in = 0;
		zoom_out = 0;		
	}
	return;
}

/*---------------------------------------------*/
/*Function change_leader() changes drone leader*/
/*---------------------------------------------*/

void change_leader(int i)
{
	if ( lead != NO_LEADER){ 
		if(i == lead){
			drone[i].leader = 1;
		} 
		else drone[i].leader = 0;
	} 
	else drone[i].leader = 1;
	return;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
/* Task drone_task every period computes and estimation of mouse (or leader) coordinates and controls drone to reach them*/
/*-----------------------------------------------------------------------------------------------------------------------*/

void *drone_task(void *arg)
{	
	int a = get_task_index(arg);								//initialization
	init_drone(a);
	init_kalman_filter(a);
	init_controller(a);
	init_motor(a);
	set_activation(a);
	while (!end) {
		pthread_mutex_lock(&mux_kalman);
		kalman_filter(a);										//coordinates estimation
		change_param_filter();									//changes filter parameters
		pthread_mutex_unlock(&mux_kalman); 
		pthread_mutex_lock(&mux_drone); 
		controller_drone(a);									//computes thrust
		motor_drone(a);											//computes real thrust
		computes_coordinates(a);								//computes drone's coordinates
		change_leader(a);										//changes leader
		pthread_mutex_unlock(&mux_drone);
		handle_borders(a);										//manages borders
		store_trail(a);								
		
		if (deadline_miss(a)){									//check dead line miss
			 printf("drone_task deadline miss %d\n", param[a].dmiss);
			 dlm_drone++;
			 }
		wait_for_activation(a);
	}
	return NULL;
}

/*------------------------------------------------------------------------------------*/
/*Task graphic_task() every period manages graphic routine, graphic updates and drones*/
/*-----------------------------------------------------------------------------------*/

void *graphic_task(void *arg)
{
	int i;
	BITMAP *screen_copy; 											//create bitmap
	BITMAP *osc_1;
	BITMAP *osc_2;
	BITMAP *paesaggio;
	paesaggio = load_bitmap("foresta24bit.bmp",NULL);				
	screen_copy = create_bitmap(XW, YW);
	osc_1 = create_bitmap(XO, YO);
	osc_2 = create_bitmap(XO, YO);
	set_activation(GRAPHIC_TASK_INDEX);
	while (!end) {  						
		scare_mouse();
		clear_to_color(screen_copy, BLACK);							//reset graphic
		blit(paesaggio,screen_copy, (XW/4)+1, (YW/8)+20, (XW/4)+1,(YW/8)+20,XW,YW);			
		pthread_mutex_lock(&mux_drone);
		for(i = 0; i<MAX_DRONI; i++){				
			draw_drone(screen_copy, i); 							//draws drones
			draw_trail(screen_copy, i);								//draws trails
		}
		pthread_mutex_unlock(&mux_drone);
		show_mouse_noisy(screen_copy);								//show mouse noisy
      		write_graphic(screen_copy);								//draws routine graphic and updates
      		draw_osc_1(osc_1, screen_copy);							//draws oscilloscopes
      		draw_osc_2(osc_2, screen_copy);
    		blit(screen_copy, screen, 0, 0, 0, 0, XW, YW);
      		zoom(screen_copy);					
            	unscare_mouse();
            	
            	if (deadline_miss(GRAPHIC_TASK_INDEX)) {			//check deadline miss
            		printf("graphic_task deadline miss %d\n", param[GRAPHIC_TASK_INDEX].dmiss);
            		dlm_graphic++;
            	}
		wait_for_activation(GRAPHIC_TASK_INDEX);
	}
	return NULL;
}

/*----------------------------------------------------------*/
/* Task user_task() every period manages user's commands */
/*----------------------------------------------------------*/

void *user_task(void *arg)
{	
	set_activation(USER_TASK_INDEX);
	while (command == 0){ 
		interpret_command();  											//interprets user's commands
		
		if (deadline_miss(USER_TASK_INDEX)){							//check deadline miss
	        	printf("user_task deadline miss %d\n", param[USER_TASK_INDEX].dmiss);
	        	dlm_user++;
	        }
	        wait_for_activation(USER_TASK_INDEX);
	}
	return NULL;
}

/*-----------------------------------------------------------------------------------*/
/*Function main() initializes Allegro, creates tasks and wait for their termination  */
/*-----------------------------------------------------------------------------------*/
	
int main()
{ 
	init(); 																	//initializes Allegro
	task_create(graphic_task, GRAPHIC_TASK_INDEX, PERIOD1, DREL1, PRIO1);		//create tasks
	task_create(user_task, USER_TASK_INDEX, PERIOD2, DREL2, PRIO2); 
	wait_for_task(USER_TASK_INDEX);												//wait for task termination
	allegro_exit();
	
    	return 0;
}
END_OF_MAIN();

