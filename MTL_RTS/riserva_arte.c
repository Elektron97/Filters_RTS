
// Title and description


// STANDARD LIBRARIES
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <math.h>
#include <allegro.h>

// #define NDEBUG
#include <assert.h> 	//assert(h > 3);

// PERSONAL LIBRARIES
#include "plib.h"
#include "tlib.h"


//----------------------------------------------------------------------------
// ------------------------------CONSTANTS------------------------------------
//----------------------------------------------------------------------------

#define _GNU_SOURCE		// first step to initialize mutex semaphores

//----------------------------------------------------------------------------
// GRAPHICS CONSTANTS
//----------------------------------------------------------------------------
#define XWIN 		1024		// window x resolution
#define YWIN 		768			// window y resolution
#define X_VERT_LINE 850			// distance of vertial line
#define Y_ORIZ_LINE 100			// distance of orizzontal line
#define FRAME_COL 	7			// color of the frame for the three parts of the application

#define LEDGE_MON_TITLE 	(X_VERT_LINE + 25)		// left edge monitor title
#define TEDGE_MON_TITLE 	10						// top edge monitor title
#define LEDGE_MON_TEXT	 	(X_VERT_LINE + 15)		// left edge monitor text
#define TEDGE_MON_TEXT1 	35						// top edge monitor text first line
#define TEDGE_MON_TEXT2 	55						// top edge monitor text second line
#define TEDGE_MON_TEXT3 	75						// top edge monitor text third line

#define BITS_COL	8			// number of bits used for color
#define BKG 		0			// background color
#define PCOLOR  	14			// prompt color
#define RED 		4
#define LIGHT_BLUE	9
#define BLUE 		1	
#define WHITE		15
#define GREEN 		2
#define MAGENTA 	5
#define LIGHT_GRAY 	7
#define CYAN 		3
//----------------------------------------------------------------------------
#define WWORLD 		848			// x length of graphic world (# pixels: [2 - 849])
#define HWORLD 		664 		// y length of graphic world (# pixels: [102 - 765])
#define XLWORLD		2			// left  graphic world x coordinate
#define XRWORLD		849			// right graphic world x coordinate
#define YTWORLD		102			// top  graphic world y coordinate
#define YBWORLD		765			// bottom graphic world y coordinate
#define WSKEET		61			// width of the skeet bitmap
#define HSKEET		61			// height of the skeet bitmap
#define WAIM		61			// side of the aim bitmap (# pixels: [0 - 60])
#define CAIM	((WAIM-1)/2)	// center of the aim: (30,30)
#define RAD_PNT_AIM 2			// radius of the point in the centre of the aim
#define SPC_LIN_AIM 10			// space between lines of the aim

//----------------------------------------------------------------------------
// TASK CONSTANTS
//----------------------------------------------------------------------------
#define N_MAX_TASK			10		// max number of tasks (in accord to library)

#define IDX_COM_INTERP		3		// task index of command interpreter "com_interp_task"
#define PRIO_COM_INTERP		3		// priority values
#define PER_COM_INTERP 		20		// task period [ms]
#define DREL_COM_INTERP 	20		// relative deadline [ms]

#define IDX_DISPLAY			4		// task index of "display_task"
#define PRIO_DISPLAY		4
#define PER_DISPLAY 		20
#define DREL_DISPLAY 		20

#define IDX_SKEET_TASK		1		// task index for skeet_task
#define PRIO_SKEET_TASK		5		// la priorità è la più alta anche per un discorso di variabili globali
#define PER_SKEET_TASK 		20		// il periodo deve essere più piccolo del display altrimenti il task del display si trova
#define DREL_SKEET_TASK 	20		// con lo stesso dato consecutivamente

#define IDX_EXPLOSION		5		// task index of "display_task"
#define PRIO_EXPLOSION		5		// try to put the same priority
#define PER_EXPLOSION		20
#define DREL_EXPLOSION		20

//----------------------------------------------------------------------------
// SKEET CONSTANTS
//----------------------------------------------------------------------------
#define N_MAX_SKEET			2
#define G 					9.81
#define RMIN 				10
#define RMAX 				25
#define HMIN				200 	// all theese parameters are refered to frame with the origin in (2,765)
#define HMAX 				400
#define VXMIN				120		// apparently velocities
#define VXMAX				180
#define VYMIN				50
#define VYMAX				60
#define TSCALE_SKEET		0.87	// time scale factor for skeet motion
//------------------------------------------------------------------------
const long SEC_WAIT_MIN	=	0;				// time values for task suspension [s]
const long SEC_WAIT_MAX	=	4;				// [s]
const time_t NSEC_WAIT_MIN= 0;				// [ns]
const time_t NSEC_WAIT_MAX= 999999999;		// [ns]

//----------------------------------------------------------------------------
// EXPLOSION CONSTANTS
//----------------------------------------------------------------------------
#define MAX_TYPES_DEBRIS_SMALL	10		// number of different types of small STEP_GEN_SMALL
#define SIZE_POLYG_SMALL 		10		// size of each array for debris small
#define STEP_GEN_SMALL 			2		// step to generate small debris along Rect generator
#define ALPHA_NORM 				12		// costant to link vx and vy to xc and yc		
#define TSCALE_DEBRIS			5		// time scale factor for debris motion
#define ADD_RAND_V 				20 		// add a random component on velocity [-ADD_RAND_V,+ADD_RAND_V]

//----------------------------------------------------------------------------
//-------------------------------STRUCTURES-----------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// SKEET STATE
//----------------------------------------------------------------------------
struct 	state 			// skeet structure 
{
		int 	c;		// color
		float 	r;		// radius [m]
		float 	x;		// x coordinate [m]
		float 	y;		// y coordinate [m]
		float 	vx;		// horizontal velocity [m/s]
		float 	vy;		// vertical velocity [m/s]

		int 	tot_dl_m;		// variable that remindes last deadline missed before clear the i-th task
};

struct state skeet[N_MAX_SKEET];	// skeet state array

//----------------------------------------------------------------------------
//	AIM COORDINATE
//----------------------------------------------------------------------------
struct 	coordinate
{
		int 	x;
		int 	y;
};

struct coordinate aim;				// aim coordinates

//----------------------------------------------------------------------------
// TIME INTERVAL OF DELAY FOR THE LAUNCH
//----------------------------------------------------------------------------
struct timespec delta[N_MAX_SKEET]; 		// create an array of struct for the delay launch of each skeet

//----------------------------------------------------------------------------
//	DEBRIS VECTORS
//----------------------------------------------------------------------------
struct 	debris_s
{
		int values[SIZE_POLYG_SMALL];		// elements of the vector rapresenting debris
		int xl;								// coordinate respect to explosion_copy frame of the x left angle of the BIPMAP
		int yt;								// coordinate respect to explosion_copy frame of the y top angle of the BITMAP

		int xc;								// coordinate respect to the center of explosion_copy frame of the x center of ther BITMAP
		int yc;								// coordinate respect to the center of explosion_copy frame of the y center of the BITMAP

		float vx;
		float vy;
};

struct debris_s deb_small[MAX_TYPES_DEBRIS_SMALL];		// number of the different types of vectors

//----------------------------------------------------------------------------
// GLOBAL VARIABLES
//----------------------------------------------------------------------------
int 	skeet_alive = 0;	// flag to describe the life-state of the skeet/skeets
int 	skeet_awake = 0;	// flag to inform when skeet_task wakes up from clock_nanosleep
int 	end = 0;			// end flag
int 	shot_f = 0; 		// flag is set to one if user clicks space	

//---------------------------------------------------------------------------
// GLOBAL BITMAPS
//---------------------------------------------------------------------------
BITMAP* aim_screen;
BITMAP* world_copy;

//---------------------------------------------------------------------------
// GLOBAL SEMAPHORES
//---------------------------------------------------------------------------
pthread_mutex_t			mux_skeet_state, mux_skeet_alive, mux_skeet_awake,		// define 4 mutex
						mux_aim_coordinate, mux_shot;

pthread_mutexattr_t 	matt;			// define mutex attributes

//----------------------------------------------------------------------------
// FUNCTIONS DECLARATION: possiamo dividerli tra ausiliarie e task
//----------------------------------------------------------------------------

void 	init();

void* 	com_interp_task(void* arg);

void*	display_task(void* arg);

void* 	explosion_task(void* arg);

void*	skeet_task(void* arg);

void 	init_skeet(int i);

void 	sleep_skeet_task(int i);

void handle_shot(int i, int skeet_x, int skeet_y, int skeet_r);

int 	irand(int min, int max);

float 	frand(float min, float max);

long 	lrand(long min, long max);

time_t 	trand(time_t min, time_t max);

void 	draw_skeet(int i, BITMAP* world_copy, BITMAP* skeet_image);

void 	draw_aim(BITMAP* world_copy);

//----------------------------------------------------------------------------
// IRAND: returns a random int in [min, max]
//----------------------------------------------------------------------------
int 	irand(int min, int max)
{
int result;

		result = rand() % (max + 1 - min) + min;
		return result;
}


//----------------------------------------------------------------------------
// FRAND: returns a random float in [min, max)
//----------------------------------------------------------------------------
float 	frand(float min, float max)
{
float 	r;

		r = rand() / (float) RAND_MAX; 	// rand in [0, 1)
		return min + (max - min) * r;
}

//----------------------------------------------------------------------------
// LRAND: returns a random long integer in [min, max]
//----------------------------------------------------------------------------
long 	lrand(long min, long max)
{
long result;

		result = rand() % (max + 1 - min) + min;
		return result;
}

//----------------------------------------------------------------------------
// TRAND: returns a random time_t integer in [min, max]
//----------------------------------------------------------------------------
time_t 	trand(time_t min, time_t max)
{
time_t result;

		result = rand() % (max + 1 - min) + min;
		return result;
}

//----------------------------------------------------------------------------
// INIT FUNCTION: initialize Allegro and Mutex semaphores
//----------------------------------------------------------------------------
void 	init()
{
char	s[30]; // it is used to make strings for textout
int i;

	allegro_init();													// initializes graphics data
	set_color_depth(BITS_COL);										// specifies n bits used for colors (8 is already default)
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);		// enters the graphic mode, for now virtual screen is not defined
	clear_to_color(screen, BKG);									// clear the screen making all pixels black
	install_keyboard();												// install the keyboard manager

	srand(time(NULL));												// initialize random generator

	//----------------------------------------------------------------------------
	// INITIALIZE GRAPHIC
	//----------------------------------------------------------------------------

	// organize graphic window
	rectfill(screen, 0, 0, 1, YWIN-1, FRAME_COL);					// sx side
	rectfill(screen, 0, YWIN-1, XWIN-1, YWIN-2, FRAME_COL);			// bottom side
	rectfill(screen, XWIN-1, YWIN-1, XWIN-2, 0, FRAME_COL);			// dx side
	rectfill(screen, 0, 0, XWIN-1, 1, FRAME_COL);					// top side

	rectfill(screen, X_VERT_LINE, 0, X_VERT_LINE+1, YWIN, FRAME_COL);	// vertical line
	rectfill(screen, 0, Y_ORIZ_LINE, X_VERT_LINE, Y_ORIZ_LINE+1, FRAME_COL);	// orizzontal line

	// dmiss and information about the skeet launch
	textout_ex(screen, font, "DEADLINES MISSED", LEDGE_MON_TITLE, TEDGE_MON_TITLE, WHITE, BKG);	// text of monitor title deadline

	for (i = 0; i < N_MAX_SKEET; i++) 
		skeet[i].tot_dl_m = 0;		// initialize che field of deadlines missed

	sprintf(s, "com_interp_task:%2u", skeet[1].tot_dl_m); 						// it is used a variable of 0
	textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT1, WHITE, BKG);	// text of monitor first line

	sprintf(s, "skeet_task:%2u", skeet[1].tot_dl_m);
	textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT2, WHITE, BKG);	// text of monitor second line

	sprintf(s, "display_task:%2u", skeet[1].tot_dl_m);
	textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT3, WHITE, BKG);	// text of monitor third line

	// scrivere comandi per informazione


	// initialize the aim
	aim_screen = create_bitmap(WAIM, WAIM);							// create a bitmap with a certain resolution
	clear_to_color(aim_screen, BKG);								// color the whole virtual screen for the graphic aim
	circlefill(aim_screen, CAIM, CAIM, 20, RED);
	circlefill(aim_screen, CAIM, CAIM, 17, BKG);							// create a circular crown with thick of 3 pixel
	rectfill(aim_screen, CAIM-1, 0, CAIM+1, CAIM-SPC_LIN_AIM, RED);			// vertical top line
	rectfill(aim_screen, CAIM-1, CAIM+SPC_LIN_AIM, CAIM+1, WAIM, RED);		// vertical bottom kine
	rectfill(aim_screen, 0, CAIM-1, CAIM-SPC_LIN_AIM, CAIM+1, RED);			// orizzonal left line
	rectfill(aim_screen, CAIM+SPC_LIN_AIM, CAIM-1, WAIM, CAIM+1, RED);		// orizzontal right line
	circlefill(aim_screen, CAIM, CAIM, RAD_PNT_AIM, RED);					// poin on the centre

	aim.x = 0;			// initial position for the aim: it is the letf top angle of the BITMAP, so
	aim.y = 0;			// we must consider the offset; the origin is on the left top

	//----------------------------------------------------------------------------
	// INITIALIZE semaphores
	//----------------------------------------------------------------------------
	pthread_mutexattr_init(&matt);
	pthread_mutexattr_setprotocol(&matt, PTHREAD_PRIO_INHERIT);
	// create 2 semaphores with Priority inheritance protocol
	pthread_mutex_init(&mux_skeet_state, &matt);
	pthread_mutex_init(&mux_skeet_alive, &matt);
	pthread_mutex_init(&mux_skeet_awake, &matt);
	pthread_mutex_init(&mux_aim_coordinate, &matt);
	pthread_mutex_init(&mux_shot, &matt);

	pthread_mutexattr_destroy(&matt); 	//destroy attributes

	//----------------------------------------------------------------------------
	// TASK CREATION
	//----------------------------------------------------------------------------

	task_create(com_interp_task, IDX_COM_INTERP, PER_COM_INTERP, DREL_COM_INTERP, PRIO_COM_INTERP);

	task_create(display_task, IDX_DISPLAY, PER_DISPLAY, DREL_DISPLAY, PRIO_DISPLAY);
	

	return;
}

//----------------------------------------------------------------------------
// INIT_SKEET: initialize skeet state
//----------------------------------------------------------------------------
void init_skeet(int i)
{
	pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state

	skeet[i].c = irand(LIGHT_BLUE, WHITE);			// color of the i-th skeet
	skeet[i].r = frand(RMIN, RMAX);					// radius of the i-th skeet

	skeet[i].x = -skeet[i].r - 2.0; 				// -2 is for the edge, it needs also -1 because the radius
	skeet[i].y = frand(HMIN, HMAX);

	skeet[i].vx = frand(VXMIN, VXMAX);
	skeet[i].vy = frand(VYMIN, VYMAX);

	pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state

	// skeet[i].v0 = sqrt(...);

	
}

//--------------------------------------------------------------------------------
// SLEEP_SKEET_TASK: sleep task of a casual time skeet before begin the true task
//--------------------------------------------------------------------------------
// after that all parameters of the task are initialized task will be suspended until the CLOCK_MONOTONIC reaches the time specified
void 	sleep_skeet_task(int i)
{
	// struct delta is global but it is not share among other tasks so tasks are not required
	delta[i].tv_sec = trand(SEC_WAIT_MIN, SEC_WAIT_MAX); 	// type of the field "tv_sec" is time_t (a long int of 32 bit)
	printf("%ld\n", delta[i].tv_sec);

	delta[i].tv_nsec = lrand(NSEC_WAIT_MIN, NSEC_WAIT_MAX); 	// type of the field "tv_nsec" is a long int (32 bit)
	printf("%ld\n", delta[i].tv_nsec);

	clock_nanosleep(CLOCK_MONOTONIC, 0, &delta[i], NULL);
}

//-----------------------------------------------------------------------------------------------------
// HANDLE_SHOT: compare global variables to see if the shot hit skeet and in case create explosion_task
//-----------------------------------------------------------------------------------------------------
void handle_shot(int i, int x_c, int y_c, int r) 	// x_c and y_c are the pixel coordinate of the centre of the skeet, r is its radius
{
int aim_x, aim_y;		// local variable to memorize aim coordinate
int dist_shot_sq;		// square of the distance of the aim respect to the skeet centre
int r_sq;				// square of the radius

	pthread_mutex_lock(&mux_aim_coordinate);		// wait on the mutex for aim coordinate
	aim_x = aim.x;
	aim_y = aim.y;
	pthread_mutex_unlock(&mux_aim_coordinate);		// wait on the mutex for aim coordinate

	// rotate the aim reference system into skeet reference system
	aim_x += CAIM;
	aim_y += -HWORLD + CAIM;
	aim_y = -aim_y;

	// calculate the distance between the aim and the centre of the skeet
	dist_shot_sq = (aim_x - x_c)*(aim_x - x_c) + (aim_y - y_c)*(aim_y - y_c);
	r_sq = r*r;

	if (dist_shot_sq <= r_sq)		// skeet hit!
	{
		pthread_mutex_lock(&mux_skeet_alive);		// wait on the mutex for skeet alive flag
		skeet_alive = 0;
		pthread_mutex_unlock(&mux_skeet_alive);		// signal on the mutex for skeet alive flag

		pthread_mutex_lock(&mux_skeet_awake);		// wait on the mutex for skeet awake flag
		skeet_awake = 0;
		pthread_mutex_unlock(&mux_skeet_awake);		// signal on the mutex for skeet awake flag

		// task_create();
	}
	else
		printf("miss\n");			// skeet missed
}

//----------------------------------------------------------------------------
// SKEET_TASK: compute the physic of the skeet/skeets 
//----------------------------------------------------------------------------
void* skeet_task(void* arg)
{
int i;				// task index
float dt;			// integration interval
int r, x, y;		// local variable to memorize global variables for the skeet!!!!!!!!!!!!!!!!!!!!!!!SPECIFICARE IL CASTING
char s[30];			// string for monitor deadline messed
int shot_f_local;	// to memorize global variable

	i = get_task_index(arg);
	init_skeet(i);		// initialize skeet with IDX_SKEET_TASK
	dt = TSCALE_SKEET * (float) get_task_period(arg) / 1000;

	// function that suspend skeet_task for a random time
	sleep_skeet_task(i);

	// when task awakes up set flags and begin the true task
	pthread_mutex_lock(&mux_skeet_awake);		// wait on the mutex for skeet alive flag
	skeet_awake = 1;
	pthread_mutex_unlock(&mux_skeet_awake);		// wait on the mutex for skeet alive flag

	set_activation(i);

	while(!end && skeet_alive)	// end is set to 0, also skeet_alive until task deads
	{
		pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state

		skeet[i].vy -= G * dt;
		skeet[i].x 	+= skeet[i].vx * dt;
		skeet[i].y 	+= skeet[i].vy * dt - G * dt * dt / 2;

		x = (int) skeet[i].x;
		y = (int) skeet[i].y;
		r = (int) skeet[i].r;
		pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state


		if (x-r >= XRWORLD)								// clear the skeet task when it disappears
		{	
			pthread_mutex_lock(&mux_skeet_alive);		// wait on the mutex for skeet alive flag
			skeet_alive = 0;
			pthread_mutex_unlock(&mux_skeet_alive);		// signal on the mutex for skeet alive flag

			pthread_mutex_lock(&mux_skeet_awake);		// wait on the mutex for skeet awake flag
			skeet_awake = 0;
			pthread_mutex_unlock(&mux_skeet_awake);		// signal on the mutex for skeet awake flag
		}

		pthread_mutex_lock(&mux_shot);		// wait on the mutex for shot flag
		shot_f_local = shot_f;
		pthread_mutex_unlock(&mux_shot);	// signal on the mutex for shot flag
		if (shot_f_local == 1){

			pthread_mutex_lock(&mux_shot);		// wait on the mutex for shot flag
			shot_f = 0;
			pthread_mutex_unlock(&mux_shot);	// signal on the mutex for shot flag

			handle_shot(i, x, y, r);
		}

		// qua andrò ad aggiornare la dimensione dello sckeet nel tempo
		// serve trovare un nuovo campo in maniera sintetica, da modificare per cambiare la forma del disco all' init
		// mentre un altro campo per ridurre la dimensione col tempo (questo può essere un di più)

		if (deadline_miss(i))
		{
			skeet[i].tot_dl_m ++;		// it is not possible to use param[i].dmiss because it clears every time the task is cleared
			sprintf(s, "skeet_task:%2u", skeet[i].tot_dl_m);
			textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT2, WHITE, BKG);	// text of monitor second line
		}

		wait_for_activation(i);

	}
	wait_for_task(i);	// when skeet is no more alive we must wait that the task is finished
	return 0;
}

//--------------------------------------------------------------------------------
// EXPLOSION_TASK: compute the physics of each bride after the dead of task_skeet
//--------------------------------------------------------------------------------
void* 	explosion_task(void* arg)
{
int r = 23;					// radius of the skeet	
int W_EXPL_BITMAP = 6*r; 	// side of the bitmap, it will not be a constant: 
							// ogni volta che lo skeet si carica possiamo preparare l' esplosione!!!!!
							// il codice di inizializzazione dell' esplosione possiamo girarlo durante il tempo di attesa dello skeet

int center = 3*r;		// coordinate of the center of explosion_copy

// coordinate del rettangolo generatore di frammenti (è dipendente dal raggio)
int RAD_GEN = r/3; 		// half side of the rect generator

// scriviamo le coordinate del quadrato magenta che genera gli skeet (frame della explosion BITMAP)
int rect_xl = center - RAD_GEN + 1;		// coordinate interne al rettangolo (i pixel che definiscono gli spigoli interni)
int rect_xr = center + RAD_GEN - 1;
int rect_yt = center - RAD_GEN + 1;
int rect_yb = center + RAD_GEN - 1;

int W_DEB_SMALL_BP = 3; 		// (si lavora nel frame dell' explosion BITMAP)

int deb_BP_xl; 					// coordinate x left of bitmap i-th generated on the side
int deb_BP_yt; 					// coordinate y top of bitmap i-th generated on the side
int deb_BP_xc; 					// coordinate x center of bitmap i-th generated along the side
int deb_BP_yc; 					// coordinate y center of bitmap i-th generated along the side

int dist_center_lt = (W_DEB_SMALL_BP-1)/2;	// distance from the center of the BITMAP to the left top pixel

	BITMAP* explosion_copy;
	explosion_copy = create_bitmap(W_EXPL_BITMAP, W_EXPL_BITMAP);		// create a bitmap with the resolution of the graphic world
	clear_to_color(explosion_copy, LIGHT_GRAY);							// color the whole virtual screen for the graphic world of black

	// GENERATION OF BITMAPS AROUND THE RECT GEN FOR EACH SIDE
	int n_debris = 100;				// number of the debris for bitmap
	BITMAP* debris_bp[n_debris];	// initializing the debris BITMAP
	int i; 							// index for cycle
	int j;							// index for cycle
	int type_extract;				// index that will extract random type

	// initializing different types of brides 
	int deb_type_small[10][10] = {	{0,0,0,0, 0,0, 0,2, 1,2}, 		// triangle L 
									{0,0,0,0, 0,1, 1,1, 1,0},		// square 2x2
									{0,0,0,0, 0,2, 2,2, 2,0},		// square 3x3
									{0,0,0,0, 0,1, 2,1, 1,0},		// trapezio
									{0,0,0,0, 0,0, 0,0, 0,1},		// line 2x1
									{0,0,0,0, 0,0, 0,0, 0,2},		// line 3x1
									{0,0,0,1, 1,2, 2,1, 2,0},
									{0,1,0,1, 0,1 ,2,1, 1,0},
									{0,0,0,0, 0,0 ,0,2, 2,2},
									{0,1,0,1, 0,1 ,0,2, 1,0} 	};

	// inserting the values of each vector in the AoSoA
	for(i = 0; i < 10; i++){
		for (j = 0; j < 10; j++){
			deb_small[i].values[j] = deb_type_small[i][j];
		}
	}
//---------------------------------------------------------------------------
	// CREATE BITMAP ALONG EACH SIDE OF THE RECT_GEN

	// reinitializing index for future use
	i = 0; 	// debris index 
	j = 0;

	// top side of rect
	// initializing local variables that wil updating slide along the side
	deb_BP_xl = rect_xl - W_DEB_SMALL_BP;		// x coordinate left for the first BITMAP
	deb_BP_yt = rect_yt - W_DEB_SMALL_BP;		// y coordinate top for the first BITMAP
	deb_BP_xc = deb_BP_xl + dist_center_lt;			// x coordinate center for the first BITMAP
	deb_BP_yc = deb_BP_yt + dist_center_lt;			// y coordinate center for the first BITMAP

	while(deb_BP_xl < rect_xr) 		// top side of rect
	{
		
		debris_bp[i] = create_bitmap(W_DEB_SMALL_BP, W_DEB_SMALL_BP);	// creating BITMAP
		clear_to_color(debris_bp[i], BKG);

		type_extract = irand(0,9);		// random extract the type of debris
		polygon(debris_bp[i], 5, deb_small[type_extract].values, RED);	// draw the type of bride extracted randomly into a BITMAP

		deb_small[i].xl = deb_BP_xl; 			// each bitmap memorize its position in explosion_copy frame
		deb_small[i].yt = deb_BP_yt;			// these two values will used to set the bitmap; there is a RELATION between yt and yc
		deb_small[i].xc = deb_BP_xc - center;	// coordinate of the centre of the BITMAP respect to the frame on the centre of the explosion_copy	
		deb_small[i].yc = -(deb_BP_yc - center);

		deb_small[i].vx = (float) deb_small[i].xc * ALPHA_NORM; 	// compute the x velocity component proportionally to 
		deb_small[i].vx += irand(-ADD_RAND_V,ADD_RAND_V);			// the distance from the center; add random component
		deb_small[i].vy = (float) deb_small[i].yc * ALPHA_NORM;		
		deb_small[i].vy += irand(-ADD_RAND_V,ADD_RAND_V);

		printf("xc: %d yc: %d\n", deb_small[i].xc, deb_small[i].yc);

		deb_BP_xl += STEP_GEN_SMALL;			// update the local variable as a window
		deb_BP_xc += STEP_GEN_SMALL;

		i++;							// update the index for the generation of BITMAP
	}
	//------------------------------------------------------------------------------
	// right side of rect

	deb_BP_xl = rect_xr + 1;					// x coordinate left for the first BITMAP
	deb_BP_yt = rect_yt - W_DEB_SMALL_BP;		// y coordinate top for the first BITMAP
	deb_BP_xc = deb_BP_xl + dist_center_lt;		// x coordinate center for the first BITMAP
	deb_BP_yc = deb_BP_yt + dist_center_lt;		// y coordinate center for the first BITMAP

	while(deb_BP_yt < rect_yb) 
	{
		
		debris_bp[i] = create_bitmap(W_DEB_SMALL_BP, W_DEB_SMALL_BP);	// create the i-th BITMAP
		clear_to_color(debris_bp[i], BKG);

		type_extract = irand(0,9);	// extract random type of debris
		polygon(debris_bp[i], 5, deb_small[type_extract].values, RED);	// draw the type of bride extracted randomly into a BITMAP



		deb_small[i].xl = deb_BP_xl;
		deb_small[i].yt = deb_BP_yt;
		deb_small[i].xc = deb_BP_xc - center;
		deb_small[i].yc = -(deb_BP_yc - center);

		deb_small[i].vx = (float) deb_small[i].xc * ALPHA_NORM;
		deb_small[i].vx += irand(-ADD_RAND_V,ADD_RAND_V);
		deb_small[i].vy = (float) deb_small[i].yc * ALPHA_NORM;
		deb_small[i].vy += irand(-ADD_RAND_V,ADD_RAND_V);

		printf("xc: %d yc: %d\n", deb_small[i].xc, deb_small[i].yc);

		deb_BP_yt += STEP_GEN_SMALL;
		deb_BP_yc += STEP_GEN_SMALL;

		i++;					// update the index for the generation of BITMAP
	}
//------------------------------------------------------------------------------
	// bottom side of rect

	deb_BP_xl = rect_xr + 1;					// x coordinate left for the first BITMAP
	deb_BP_yt = rect_yb + 1;					// y coordinate top for the first BITMAP
	deb_BP_xc = deb_BP_xl + dist_center_lt;		// x coordinate center for the first BITMAP
	deb_BP_yc = deb_BP_yt + dist_center_lt;		// y coordinate center for the first BITMAP

	while(deb_BP_xl > (rect_xl - W_DEB_SMALL_BP)) 
	{
		
		debris_bp[i] = create_bitmap(W_DEB_SMALL_BP, W_DEB_SMALL_BP);
		clear_to_color(debris_bp[i], BKG);

		type_extract = irand(0,9);
		polygon(debris_bp[i], 5, deb_small[type_extract].values, RED);	// draw the type of bride extracted randomly into a BITMAP

		deb_small[i].xl = deb_BP_xl;
		deb_small[i].yt = deb_BP_yt;
		deb_small[i].xc = deb_BP_xc - center;
		deb_small[i].yc = -(deb_BP_yc - center);

		printf("xc: %d yc: %d\n", deb_small[i].xc, deb_small[i].yc);

		deb_small[i].vx = (float) deb_small[i].xc * ALPHA_NORM;
		deb_small[i].vx += irand(-ADD_RAND_V,ADD_RAND_V);
		deb_small[i].vy = (float) deb_small[i].yc * ALPHA_NORM;
		deb_small[i].vy += irand(-ADD_RAND_V,ADD_RAND_V);

		deb_BP_xl -= STEP_GEN_SMALL;
		deb_BP_xc -= STEP_GEN_SMALL;

		i++;					// update the index for the generation of BITMAP
	}
	//------------------------------------------------------------------------------
	// left side of rect

	deb_BP_xl = rect_xl - W_DEB_SMALL_BP;		// x coordinate left for the first BITMAP
	deb_BP_yt = rect_yb + 1;					// y coordinate top for the first BITMAP
	deb_BP_xc = deb_BP_xl + dist_center_lt;		// x coordinate center for the first BITMAP
	deb_BP_yc = deb_BP_yt + dist_center_lt;		// y coordinate center for the first BITMAP

	while(deb_BP_yt > (rect_yt - W_DEB_SMALL_BP)) 
	{
		
		debris_bp[i] = create_bitmap(W_DEB_SMALL_BP, W_DEB_SMALL_BP);
		clear_to_color(debris_bp[i], BKG);

		type_extract = irand(0,9);
		polygon(debris_bp[i], 5, deb_small[type_extract].values, RED);	// draw the type of bride extracted randomly into a BITMAP

		deb_small[i].xl = deb_BP_xl;
		deb_small[i].yt = deb_BP_yt;
		deb_small[i].xc = deb_BP_xc - center;
		deb_small[i].yc = -(deb_BP_yc - center);

		deb_small[i].vx = (float) deb_small[i].xc * ALPHA_NORM;
		deb_small[i].vx += irand(-ADD_RAND_V,ADD_RAND_V);
		deb_small[i].vy = (float) deb_small[i].yc * ALPHA_NORM;
		deb_small[i].vy += irand(-ADD_RAND_V,ADD_RAND_V);

		printf("xc: %f yc: %f\n", deb_small[i].vx, deb_small[i].vy);

		deb_BP_yt -= STEP_GEN_SMALL;
		deb_BP_yc -= STEP_GEN_SMALL;

		i++;					// update the index for the generation of BITMAP
	}
	int debris_small_gen = i;
	printf("%d\n", i);

	float dt;
	dt = TSCALE_DEBRIS * (float) get_task_period(arg) / 1000;

	circlefill(explosion_copy, 3*r, 3*r, r, GREEN); //cerchio di raggio r
	rect(explosion_copy, rect_xl, rect_yt, rect_xr, rect_yb, BLUE); // quadrato di lato r/3

	int first_draw = 1;

	while(!end)
	{
		if (!first_draw){

			for(i = 0; i < debris_small_gen; i++)
			{
				deb_small[i].xc += deb_small[i].vx * dt;
				deb_small[i].xl  = deb_small[i].xc + center - dist_center_lt;
				deb_small[i].yc += deb_small[i].vy * dt;
				deb_small[i].yt  = -deb_small[i].yc + center - dist_center_lt;
				draw_sprite(explosion_copy, debris_bp[i], deb_small[i].xl, deb_small[i].yt);
				// printf(" i = %d => vx: %f  vy: %f\n", i, deb_small[i].vx, deb_small[i].vy);

			}
		}
		else
		{
			for(i = 0; i < debris_small_gen; i++)
				draw_sprite(explosion_copy, debris_bp[i], deb_small[i].xl, deb_small[i].yt);
			first_draw = 0;
		}
		// putpixel(explosion_copy, deb_small[i].xc + center, -deb_small[i].yc + center, WHITE);
		
		// copy the skeet image in the world_copy virtual screen
		blit(explosion_copy, world_copy, 0, 0, WWORLD/2-3*r, HWORLD/2-3*r, explosion_copy->w, explosion_copy->h);	
	}
	return 0;
}

//----------------------------------------------------------------------------
// DRAW_AIM: project the BITMAP aim_screen on another BITMAP: world_copy
//----------------------------------------------------------------------------
void 	draw_aim(BITMAP* world_copy)
{
int x, y;

	pthread_mutex_lock(&mux_aim_coordinate); 	// wait on the mutex for aim coordinate
	x = aim.x;
	y = aim.y;
	pthread_mutex_unlock(&mux_aim_coordinate); 	// signal on the mutex for aim coordinate

	// blit(aim_screen, world_copy, 0, 0, x, y, WAIM, WAIM);
	draw_sprite(world_copy, aim_screen, x, y); // the background color (BGK = BLACK) is transparent to the skeets
}

//----------------------------------------------------------------------------
// DRAW_SKEET: draw skeet i in graphic coordinates 
//----------------------------------------------------------------------------
void 	draw_skeet(int i, BITMAP* world_copy, BITMAP* skeet_image)
{
int skeet_col; 					// random color
float x, y, r;			// local varibale for skeet physic
float xl, yt;			// left top  coordinate for skeet_image p
				// ALLA FINE DEL CODICE CONTROLLARE SE REALMENTE SERVONO 2 BITMAP
	
	pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state
		// printf("%f\n", skeet[i].y);
	x = skeet[i].x;
	y = skeet[i].y;
	r = skeet[i].r;

	skeet_col = skeet[i].c;

	pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state 
	// NON È RIDONDANTE PERCHÈ POTREI DECIDERE DI RIDURRE COL TEMPO LE DIMENSIONI DEL CERCHIO!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	circlefill(skeet_image, (WSKEET-1)/2, (HSKEET-1)/2, r, skeet_col);	//copy on skeet_image virtual on the centre

	xl = x - ((WSKEET-1)/2);			// transform coordinates of skeet into left top point of skeet_image BITMAP
	yt = -(y - YBWORLD + YTWORLD + (HSKEET-1)/2);
	blit(skeet_image, world_copy, 0, 0, xl, yt, WSKEET, HSKEET);	// copy the skeet image in the world_copy virtual screen
	
	// METODO INIZIALE
	// x += 2.0;
	// y = -y + YBWORLD;

	// circlefill(world_copy, x, y, r, skeet_col);

}
//----------------------------------------------------------------------------
// DISPLAY_TASK (TASK): 
//----------------------------------------------------------------------------
void* display_task(void* arg)
{
int i;						// task index
int skeet_awake_local; 		// copy global variable in a local variable for semaphore
char s[30];				// string for monitor deadline messed

	// VOLENDO SAREBBE MEGLIO INIZIALIZZARE in init display!!
	// create bitmap for display task
	// BITMAP* world_copy;
	world_copy = create_bitmap(WWORLD, HWORLD);		// create a bitmap with the resolution of the graphic world
	clear_to_color(world_copy, BKG);				// color the whole virtual screen for the graphic world of black

	BITMAP* skeet_image;
	skeet_image = create_bitmap(WSKEET, HSKEET); 	// create a bitmap with the resolution of the biggest skeet!!!
	clear_to_color(skeet_image, BKG);				// color the whole virtual screen for the skeet of black

	i = get_task_index(arg);
	set_activation(i);				//NON SONO SICURO SE SI METTE QUA IL SET_ACTIVATION!

	while(!end)
	{

		clear_to_color(skeet_image, BKG);								// clear the virtual screen "skeet_image"

		clear_to_color(world_copy, BKG);								// clear the virtual screen "world_copy"

		pthread_mutex_lock(&mux_skeet_awake);		// wait on the mutex for skeet alive flag		
		skeet_awake_local = skeet_awake;
		pthread_mutex_unlock(&mux_skeet_awake);		// wait on the mutex for skeet alive flag
		// pthread_mutex_lock(&mux_skeet_alive);		// wait on the mutex for skeet alive flag		
		// skeet_alive_local = skeet_alive;
		// pthread_mutex_unlock(&mux_skeet_alive);		// wait on the mutex for skeet alive flag
		if (skeet_awake_local)
		{
			// FOR tutti gli skeet disponibili
			draw_skeet(IDX_SKEET_TASK, world_copy, skeet_image);
		}

		draw_aim(world_copy);

		// copy the rectangular area from world_copy (virtual graphic world) to screen (real graphic world)
		blit(world_copy, screen, 0, 0, XLWORLD, YTWORLD, WWORLD, HWORLD);

		// write_graphic inserisco valori aggiornati (es: dmiss, velocità skeet, distanza ecc..)

		if (deadline_miss(i))
		{
			sprintf(s, "display_task:%2u", param[i].dmiss);
			textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT3, WHITE, BKG);	// text of monitor third line
		}

		wait_for_activation(i);
	}

	return 0;
}

//----------------------------------------------------------------------------
// COM_INTERP_TASK (TASK): read the key pressed on keyboard
//----------------------------------------------------------------------------
void* com_interp_task(void* arg)
{
char scan;				// scan code that memorize the key pressed
int skeet_alive_local;	// local variable that memorize global variable
char s[30];				// string for monitor deadline messed
int i;					// task index
int ammo = 0;			// number of attempts to hit the skeet per skeet; it is initialized to 0 until first press of KEY_C is verified

// (draw_aim is printing the top left angle of the BITMAP!)
int yt_lim = -CAIM + 3;					// coordinate y top for the limit of the position of the aim
int yb_lim = -CAIM - 3 + HWORLD;		// coordinate y bottom for the limit of the position of the aim
int xl_lim = -CAIM + 3;					// coordinate x left for the limit of the position of the aim
int xr_lim = -CAIM - 3 + WWORLD;		// coordinate x right for the limit of the position of the aim
int step_aim = 4;

	i = get_task_index(arg);
	set_activation(i);

	do
	{	
		if(key[KEY_UP])
		{				
			pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
			if (aim.y >= yt_lim) 	aim.y -= step_aim;				// make sure that the aim don't go out of the screen
			pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		}

		if(key[KEY_DOWN])
		{
			pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
			if (aim.y <= yb_lim) 	aim.y += step_aim;				// make sure that the aim don't go out of the screen						
			pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		}

		if(key[KEY_LEFT])
		{
			pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
			if (aim.x >= xl_lim) 	aim.x -= step_aim;				// make sure that the aim don't go out of the screen	
			pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		}

		if(key[KEY_RIGHT])
		{
			pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
			if (aim.x <= xr_lim) 	aim.x += step_aim;				// you make sure that the aim don't go out of the screen	
			pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		}
					
		scan = 0;
		if (keypressed())	// returns true if there are characters in keyboard buffer, else false (it does not block the program execution)
		{
			scan = readkey() >> 8;		// shift right the high byte	
			switch (scan)
			{
				case KEY_TAB:		// CAMBIO MODALITA': bisogna introdurre regola che non puoi cambiare modalità mentre giochi

					break;

				case KEY_C:
					pthread_mutex_lock(&mux_skeet_alive);		// wait on the mutex for skeet alive flag
					skeet_alive_local = skeet_alive;
					pthread_mutex_unlock(&mux_skeet_alive);		// signal on the mutex for skeet alive flag
					if (skeet_alive_local == 0){				// if no skeet are in the window

						pthread_mutex_lock(&mux_skeet_alive);		// wait on the mutex for skeet alive flag
						skeet_alive++;
						pthread_mutex_unlock(&mux_skeet_alive);		// signal on the mutex for skeet alive flag
						ammo = 2;									// reload the rifle

						task_create(skeet_task, IDX_SKEET_TASK, PER_SKEET_TASK, DREL_SKEET_TASK, PRIO_SKEET_TASK); // idx becomes 1
						break;

					}
					else
					{
						printf("Out of Skeets\n");
						break;// INSERIRE TESTO PER UTENTE DEL TIPO "FINITI GLI SKEET", "SKEET NON ANCORA DISPONIBILI"
					}

				case KEY_SPACE:
					if (ammo > 0){
						ammo--;
						shot_f = 1;
						printf("shot! ammo left: %1d\n", ammo);
					}
					else printf("Out of Ammo\n");
					break;

				case KEY_M:
					task_create(explosion_task, IDX_EXPLOSION, PER_EXPLOSION, DREL_EXPLOSION, PRIO_EXPLOSION);
					break;

				default:	// wrong key pressed
					if (scan != KEY_ESC && scan != KEY_UP && scan != KEY_DOWN && scan != KEY_RIGHT && scan != KEY_LEFT)
						printf("Key not recognized\n");
					break;
				// BISOGNA INSERIRE COSA FARE SE CLICCO ALTRI TASTI
			}

		}
		if (deadline_miss(i))
		{
			sprintf(s, "com_interp_task:%2u", param[i].dmiss); 
			textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT1, WHITE, BKG);	// text of monitor first line
		}

	wait_for_activation(i);
	} while (scan != KEY_ESC);				// end of do{}
	end = 1;								// set end flag when user clicks EXIT key
	return 0;
}




int main()
{
int i;

	init();
	
	wait_for_task(IDX_COM_INTERP);			// wait until "com_interp_task" finishes (when user clicks ESC on keyboard)
//CLEAR ALL THE TASKS!!!!!!!!!!!!!!!!!!!!!!!
	for(i = 0; i < N_MAX_TASK; i++)
	{
		wait_for_task(i);
	}
	printf("Exit from the application\n");
	allegro_exit();


	return 0;
}