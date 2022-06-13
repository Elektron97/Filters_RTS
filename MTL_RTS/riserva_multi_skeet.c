
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

#define LEDGE_RECT_DMISS	(X_VERT_LINE + 8)		// left edge rect dmiss
#define TEDGE_RECT_DMISS	12						// top edge rect dmiss
#define REDGE_RECT_DMISS	(X_VERT_LINE + 165)		// right edge rect dmiss
#define BEDGE_RECT_DMISS	140						// bottom edge rect dmiss
#define LEDGE_MON_TITLE 	(X_VERT_LINE + 25)		// left edge monitor title
#define TEDGE_MON_TITLE 	25						// top edge monitor title
#define LEDGE_MON_TEXT	 	(X_VERT_LINE + 15)		// left edge monitor text
#define TEDGE_MON_TEXT1 	45						// top edge monitor text first line
#define TEDGE_MON_TEXT2 	65						// top edge monitor text second line
#define TEDGE_MON_TEXT3 	85						// top edge monitor text third line
#define TEDGE_MON_TEXT4 	105						// top edge monitor text fourth line
#define TEDGE_MON_TEXT5 	125						// top edge monitor text fifth line
//------------------------------------------------------------------------------------
#define WPLOT_SHOT			161									// width (is equal to height) of rect plot_shot
#define C_PLOT_SHOT_BP		((WPLOT_SHOT-1)/2)					// coordinate centre x(=y) of plot shot (bitmap frame)
#define LEDGE_PLOT_SHOT		(X_VERT_LINE + 6)					// left edge rect plot_shot
#define TEDGE_PLOT_SHOT		600									// top edge rect plot_shot
#define REDGE_PLOT_SHOT		(LEDGE_PLOT_SHOT + WPLOT_SHOT)		// right edge rect plot_shot
#define BEDGE_PLOT_SHOT		(TEDGE_PLOT_SHOT + WPLOT_SHOT)		// bottom edge rect plot_shot
#define CXPLOT_SHOT 		(LEDGE_PLOT_SHOT + ((WPLOT_SHOT-1)/2))// coordinate x of the centre of plot_shot (screen frame)
#define CYPLOT_SHOT 		(TEDGE_PLOT_SHOT + ((WPLOT_SHOT-1)/2))// coordinate y of the centre of plot_shot (screen frame)
//------------------------------------------------------------------------------------
#define LCOM_TEXT1			10			// left edge command (1,1) text
#define TCOM_TEXT1			8			// top edge command (1,1) text
#define TCOM_TEXT2			28			// top edge command (2,1) text
#define TCOM_TEXT3			48			// top edge command (3,1) text
#define TCOM_TEXT4			68			// top edge command (4,1) text
#define TCOM_TEXT5			88			// top edge command (5,1) text

#define LMODE_TEXT1 		270 				// left edge mode (2,1) text
#define TMODE_TEXT1			(TCOM_TEXT1)		// top edge mode (2,1) text
#define LMODE_TEXT1L		(LMODE_TEXT2 + 110)	// top edge (2,1) low speed text
#define LMODE_TEXT1M		(LMODE_TEXT2 + 148)	// top edge (2,1) medium speed text
#define LMODE_TEXT1H		(LMODE_TEXT2 + 208)	// top edge (2,1) high speed text
// #define LDESCRIP_TEXT1		(LMODE_TEXT1-20)	// top edge description (2,1) text
// #define TDESCRIP_TEXT1		(TCOM_TEXT1+10)		// top edge description (2,1) text

#define LMODE_TEXT2 		(LMODE_TEXT1) 		// left edge mode (2,2) text
#define TMODE_TEXT2			(TCOM_TEXT2)		// top edge mode (2,2) text
#define LDESCRIP_TEXT2		(LMODE_TEXT2-20)	// left edge description (2,2) text
#define TDESCRIP_TEXT2		(TCOM_TEXT2+10)		// top edge description (2,2) text
//------------------------------------------------------------------------------------
#define LINFO_TEXT_SKEET	600			// left edge for ammo text
#define TINFO_TEXT_SKEET	10			// top edge for ammo text
#define LINFO_STATE_SKEET	700 		// left for info state (it's dynamic text)

#define T_MESSAGE_USER		30 				// top coordinate for user message
#define L_MESSAGE_USER		600 			// top coordinate for user message
#define N_MAX_FLASH			1 				// max number of text flash
const float STEP_FLASH_TEXT = 0.015;		// step for reducing the value of the color
#define MSG_USER_WAST_SHOT 	1	// flag to show flash text "wasted shot"
#define MSG_USER_MISS		2	// flag to show flash text "missed skeet"
#define MSG_USER_OOA		3	// flag to show flash text "out of ammo"
#define MSG_USER_OOS 		4	// flag to show flash text "reloading next skeet"
#define MSG_USER_WAIT_MODE 	5	// flag to show flash text "can't change mode now"
#define MSG_USER_WAIT_SPEED 6 	// flag to show flash text "can't change speed now"
#define MSG_USER_KNR 		7 	// flag to show flash text "key not recognized"

#define LINFO_TEXT_AMMO 	600 			// left edge for ammo text
#define TINFO_TEXT_AMMO 	60 				// top edge for ammo text
#define XL_RECT_AMMO1		648				// left edge for ammo frame 1
#define XR_RECT_AMMO1		670				// right edge for ammo frame 1
#define YT_RECT_AMMO	(TINFO_TEXT_AMMO-4)	// top edge for ammo frames 1-2
#define YB_RECT_AMMO		97				// bottom edge for ammo frames 1-2
#define XL_RECT_AMMO2		678				// left edge for ammo frame 2
#define XR_RECT_AMMO2		700				// right edge for ammo frame 2
#define XL_AMMO1_BP		(XL_RECT_AMMO1+3) 		// left coordinate for blit ammo1 BITMAP
#define XL_AMMO2_BP		(XL_RECT_AMMO2+3) 		// left coordinate for blit ammo2 BITMAP
#define YT_AMMO_BP		(YT_RECT_AMMO+2) 		// left coordinate for blit ammo BITMAP
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

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
#define BITS_COL		16			// number of bits used for color

#define FRAME_COL 		54938		// color of the frame for the three parts of the application (Light gray)
#define BKG 			0			// background color (Black)

#define BLACK 			0
#define RED 			63488 // (255, 0, 0) converted by makecol in colors.c
#define LIGHT_GRAY  	54938
#define BLUE 			31
#define BROWN 			41285

#define PINK 			63519 	// PINK color it's used for sprite backgrounds

#define CYAN 			2047
#define GREEN 			2016
#define ELECTRIC_BLUE 	32735
#define LIGHT_GREEN 	38863
#define ELECTRIC_GREEN 	53216
#define ELECTRIC_PURPLE	47135
#define NEON_CARROT 	64710
#define LIGHT_CYAN 		45055
#define LIGHT_RED 		64041
#define LIGHT_MAGENTA 	58460
#define YELLOW 			65504
#define WHITE 			65535

#define DIM_COLORS4SKEET 	12
const unsigned int COLORS4SKEET[DIM_COLORS4SKEET] = {2047, 2016, 32735, 38863, 53216, 47135 ,64710 ,45055 ,64041 ,58460 ,65504 ,65535};
//----------------------------------------------------------------------------
// TASK CONSTANTS
//----------------------------------------------------------------------------
#define N_MAX_TASK			10		// max number of tasks (in accord to library)

#define IDX_COM_INTERP		3		// task index of command interpreter "com_interp_task"
#define PRIO_COM_INTERP		2		// priority values
#define PER_COM_INTERP 		25		// task period [ms]
#define DREL_COM_INTERP 	25		// relative deadline [ms]

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

#define IDX_RECOIL			6
#define PRIO_RECOIL			1
#define PER_RECOIL			30
#define DREL_RECOIL			30
//----------------------------------------------------------------------------
// SKEET CONSTANTS
//----------------------------------------------------------------------------
#define N_MAX_SKEET			2		// int to create a vector of skeets
#define G 					9.81
#define RMIN 				10		//10
#define RMAX 				20		//20
#define HMIN				200 	// all theese parameters are refered to frame with the origin in (2,765)
#define HMAX 				400
#define VX_LOW				0		// difficulty mode for velocity
#define VX_MEDIUM			1
#define VX_HIGH				2
#define VXMIN_L				100		// apparently velocities (mode: easy)
#define VXMAX_L				180
#define VXMIN_M				180		// apparently velocities (mode: medium)
#define VXMAX_M				220
#define VXMIN_H				220		// apparently velocities (mode: hard)
#define VXMAX_H				260
#define VYMIN				50
#define VYMAX				80
#define TSCALE_SKEET		0.87	// time scale factor for skeet motion

//------------------------------------------------------------------------
const long SEC_WAIT_MIN	=	1;				// time values for task suspension [s]
const long SEC_WAIT_MAX	=	4;				// [s]
const time_t NSEC_WAIT_MIN= 0;				// [ns]
const time_t NSEC_WAIT_MAX= 999999999;		// [ns]

//-------------------------------------------------------------------------
#define ALPHA_FUT 			350	 	// proportional constant for computing the future position of skeet

//----------------------------------------------------------------------------
// EXPLOSION CONSTANTS
//----------------------------------------------------------------------------
#define MAX_TYPES_DEBRIS_SMALL	10		// number of different types of small STEP_GEN_SMALL
#define N_DEBRIS 				100		// number of possible small debris generated by the explosion
#define SIZE_POLYG_SMALL 		10		// size of each array for debris small
#define STEP_GEN_SMALL 			3		// step to generate small debris along Rect generator
#define ALPHA_NORM 				4		// costant to link vx and vy to xc and yc		
#define TSCALE_DEBRIS			0.8		// time scale factor for debris motion
#define ADD_RAND_V 				20 		// add a random component on velocity [-ADD_RAND_V,+ADD_RAND_V]
#define ALPHA_RAD_BOUND 		1.5		// scale factor to boundary radius of debris (for hide them)
#define MAX_RAND_BOUND			10 		// add a random component on radius boundary [0, MAX_RAND_BOUND]

//----------------------------------------------------------------------------
// AIM CONSTANTS
//----------------------------------------------------------------------------
// (draw_aim is printing the top left angle of the aim BITMAP!)
#define YT_LIM_AIM 		 	(-CAIM + 3)					// coordinate y top for the limit of the position of the aim
#define YB_LIM_AIM 		 (-CAIM - 3 + HWORLD)			// coordinate y bottom for the limit of the position of the aim
#define XL_LIM_AIM 			 (-CAIM + 3)				// coordinate x left for the limit of the position of the aim
#define XR_LIM_AIM 		 (-CAIM - 3 + WWORLD)			// coordinate x right for the limit of the position of the aim
#define STEP_AIM  				4						// amount of displacemente of the aim caused by arrow keys

//----------------------------------------------------------------------------
// SHOTGUN CONSTANTS
//----------------------------------------------------------------------------
#define SHOTGUN_FRAME_B 		58		// height of the bottom frame (black) of each shotgun sprite
#define ALPHAX 					2					// proportional constant to link aim x-coordinate with shotgun x-coordinate
#define ALPHAY 					7					// proportional constant to link aim y-coordinate with shotgun y-coordinate
#define BLIM_SHOTGUN 			380					// bottom limit of y-coordinate of the aim (in aim frame)
const int MAX_Y_SHOTGUN	= (BLIM_SHOTGUN/ALPHAY);	// constant limit of y-coordinate of the shotgun, if it reaches BLIM_SHOTGUN

//-----------------------------////////////////////-----------------------------
//-----------------------------//ANIMATIONS FILES//---------------------------
//-----------------------------////////////////////---------------------------
#define LCHAR_SEQ 				40 							// define lenght of each char file name for sequences
#define N_RELOAD_IMG 			14							// number of reload sequence images
// file name of shotgun reload sequence
const char 	reload_seqFN[N_RELOAD_IMG][LCHAR_SEQ] = {
"Reload_sequence_bmp/01p.bmp", "Reload_sequence_bmp/02p.bmp", "Reload_sequence_bmp/03p.bmp", "Reload_sequence_bmp/04p.bmp", "Reload_sequence_bmp/05p.bmp",
"Reload_sequence_bmp/06p.bmp", "Reload_sequence_bmp/07p.bmp", "Reload_sequence_bmp/08p.bmp", "Reload_sequence_bmp/09p.bmp", "Reload_sequence_bmp/10p.bmp",
"Reload_sequence_bmp/11p.bmp", "Reload_sequence_bmp/12p.bmp", "Reload_sequence_bmp/13p.bmp", "Reload_sequence_bmp/14p.bmp"};

#define N_SHOT_IMG 				13							// number of shot sequence images
// file name of shotgun shot sequence
const char 	shot_seqFN[N_SHOT_IMG][LCHAR_SEQ] = {
"Shot_sequence_bmp/01p.bmp", "Shot_sequence_bmp/02p.bmp", "Shot_sequence_bmp/03p.bmp", "Shot_sequence_bmp/04p.bmp", "Shot_sequence_bmp/05p.bmp",
"Shot_sequence_bmp/06p.bmp", "Shot_sequence_bmp/07p.bmp", "Shot_sequence_bmp/08p.bmp", "Shot_sequence_bmp/09p.bmp", "Shot_sequence_bmp/10p.bmp",
"Shot_sequence_bmp/11p.bmp", "Shot_sequence_bmp/12p.bmp", "Shot_sequence_bmp/13p.bmp"};
//-----------------------------//////////////-----------------------------------
//-----------------------------//STRUCTURES//---------------------------------
//-----------------------------//////////////-----------------------------------

//----------------------------------------------------------------------------
// SKEET STATE
//----------------------------------------------------------------------------
struct 	state 			// skeet structure 
{
		unsigned int 	c;		// color
		float 	r;		// radius [m]
		float 	x;		// x coordinate [m]
		float 	y;		// y coordinate [m]
		float 	vx;		// horizontal velocity [m/s]
		float 	vy;		// vertical velocity [m/s]
		int 	side_launch; 	// integer that informs about the side of the screen where skeet is launched (left:0, right:1)

		float 	x_last4expl;	// center of the future explosion
		float 	y_last4expl;

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

		float 	xl;					// coordinate respect to explosion_copy frame of the x left angle of the BIPMAP
		float 	yt;					// coordinate respect to explosion_copy frame of the y top angle of the BITMAP

		float 	xc;					// coordinate respect to the center of explosion_copy frame of the x center of ther BITMAP
		float 	yc;					// coordinate respect to the center of explosion_copy frame of the y center of the BITMAP

		float 	vx;
		float 	vy;

		int 	xbound; 			// coordinate x respect to the center of explosion_copy frame of the limit of the debris
		int 	ybound;				// coordinate y respect to the center of explosion_copy frame of the limit of the debris

		unsigned int 	col;
		int 	alive_stat;			// variable that describes if debris is hidden or not
};

struct debris_s deb_small[N_DEBRIS];		// number of the different types of vectors

//----------------------------------------------------------------------------
//	STRUCTURE FOR COLORS (local structure)
//----------------------------------------------------------------------------
struct rgb_hsv{
				int r, g, b;		// rgb values of color of user message (initialized as BLACK)
				float h, s, v;		// HSV of color of user message (initialized as BLACK)
};

//----------------------------------------------------------------------------
// GLOBAL VARIABLES
//----------------------------------------------------------------------------
int 	skeet_alive = 0;		// flag to describe the life-state of the skeet/skeets
int 	skeet_awake = 0;		// flag to inform when skeet_task wakes up from clock_nanosleep
int 	skeet_explosion = 0;	// flag to inform if skeet is hit
int 	shot_f = 0; 			// flag is set to one if user clicks space	
int 	end = 0;				// end flag

int 	anticip_shot_f = 0; 	// flag for mode: anticipate shot (default:0)
int 	handle_anticip_f = 0; 	// flag to say to skeet_task that skeet is hit but continue to draw it
int 	debris_small_gen;

int 	expl_tot_dl_m = 0;		// variable that remindes last deadline missed before clear

int 	vx_diffic_select = 1;		// difficulty mode select (0: easy, 1: medium [default,] 2: hard)

int 	flash_f = 0; 				// flag for auto-terminate text flash
int 	msg_user_new = 0;			// last message to inform user about something (int the flash text) (0 means no message)
int 	msg_user_prev = 0;			// previous message to inform user about something (int the flash text)

int 	reload_seq_f = 0;			// flag to comunicate to draw_task to draw reloading sequence
int 	shot_seq_f = 0; 			// flag to comunicate to draw_skeet to draw shot sequence
//---------------------------------------------------------------------------
// GLOBAL BITMAPS
//---------------------------------------------------------------------------
BITMAP* aim_screen;
BITMAP* explosion_copy;
int		W_EXPL_BITMAP; 	// side of the bitmap explosion_copy (it depends on skeet radius)

//---------------------------------------------------------------------------
// GLOBAL SEMAPHORES
//---------------------------------------------------------------------------
pthread_mutex_t			mux_skeet_state, mux_aim_coordinate, mux_debris_small; // define 3 mutex

pthread_mutexattr_t 	matt;			// define mutex attributes

// initialize the semaphore to synhronize skeet_explosion with the compute of debris physics
sem_t 	sem_sync_expl;

//----------------------------------------------------------------------------
// FUNCTIONS DECLARATION: possiamo dividerli tra ausiliarie e task
//----------------------------------------------------------------------------

void 	init();

void* 	com_interp_task(void* arg);

void*	display_task(void* arg);

void* 	explosion_task(void* arg);

void*	skeet_task(void* arg);

void*	recoil_task(void* arg);

void 	init_skeet(int i);

void 	sleep_skeet_task(int i);

void 	display_plot_shot(int i, int dist_x, int dist_y);

void 	handle_shot(int i, int skeet_x, int skeet_y, int skeet_r, float dt);

int 	irand(int min, int max);

float 	frand(float min, float max);

long 	lrand(long min, long max);

time_t 	trand(time_t min, time_t max);

void 	draw_skeet(int i, BITMAP* world_copy, BITMAP* skeet_image);

void 	draw_aim(BITMAP* world_copy);

void 	draw_explosion(int i, BITMAP* world_copy, BITMAP* explosion_copy);

void 	draw_reload_seq(BITMAP *world_copy, BITMAP *reload_seqBP[], int x, int y, int rel_seq_idx);

void 	draw_shot_seq(BITMAP *world_copy, BITMAP *shot_seqBP[], int x, int y, int shot_seq_idx);

void 	draw_shotgun(BITMAP* world_copy, BITMAP *shotgun_img, int x0, int y0);

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
	 // extern void *allegro_icon;
	 // set_icon(allegro_icon); 
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);		// enters the graphic mode, for now virtual screen is not defined
	clear_to_color(screen, BKG);									// clear the screen making all pixels black
	install_keyboard();												// install the keyboard manager

//********************************************************************************************************************************

	// if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0)
	// {
	// 	allegro_message("Error with sound");
	// 	return;
	// }
	// 	allegro_message("%s\n",allegro_error);

	// SAMPLE *shot_sound;
	// shot_sound = load_sample("shot.wav");
	// if (!shot_sound)
	// {
	// 	allegro_message("Error reading wav file");
	// 	return;
	// }
	
	// play_sample(shot_sound, 255, 128, 1000, 1);
	// printf("Finish\n");

//*********************************************************************************************************************************

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

	// initialize the aim image
	aim_screen = create_bitmap(WAIM, WAIM);							// create a bitmap with a certain resolution
	if (!aim_screen)
	{
		printf("Error creating aim_screen BITMAP\n");
		exit(1);
	}
	clear_to_color(aim_screen, PINK);								// color the whole virtual screen for the graphic aim
	circlefill(aim_screen, CAIM, CAIM, 20, RED);
	circlefill(aim_screen, CAIM, CAIM, 17, PINK);							// create a circular crown with thick of 3 pixel
	rectfill(aim_screen, CAIM-1, 0, CAIM+1, CAIM-SPC_LIN_AIM, RED);			// vertical top line
	rectfill(aim_screen, CAIM-1, CAIM+SPC_LIN_AIM, CAIM+1, WAIM, RED);		// vertical bottom kine
	rectfill(aim_screen, 0, CAIM-1, CAIM-SPC_LIN_AIM, CAIM+1, RED);			// orizzonal left line
	rectfill(aim_screen, CAIM+SPC_LIN_AIM, CAIM-1, WAIM, CAIM+1, RED);		// orizzontal right line
	circlefill(aim_screen, CAIM, CAIM, RAD_PNT_AIM, RED);					// poin on the centre

	aim.x = 0;			// initial position for the aim: it is the letf top angle of the BITMAP, so
	aim.y = 0;			// we must consider the offset; the origin is on the left top

	//----------------------------------------------------------------------------
	// INITIALIZE TEXT
	//----------------------------------------------------------------------------
	// task deadlines missed
	rect(screen, LEDGE_RECT_DMISS, TEDGE_RECT_DMISS, REDGE_RECT_DMISS, BEDGE_RECT_DMISS, RED);
	textout_ex(screen, font, "DEADLINES MISSED", LEDGE_MON_TITLE, TEDGE_MON_TITLE, RED, BKG);	// text of monitor title deadline

	for (i = 0; i < N_MAX_SKEET; i++) 
		skeet[i].tot_dl_m = 0;		// initialize che field of deadlines missed

	sprintf(s, "com_interp_task:%2u", 0); 						// it is used a variable of 0
	textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT1, WHITE, BKG);	// text of monitor first line

	sprintf(s, "skeet_task:%2u", 0);
	textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT2, WHITE, BKG);	// text of monitor second line

	sprintf(s, "display_task:%2u", 0);
	textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT3, WHITE, BKG);	// text of monitor third line

	sprintf(s, "explosion_task:%2u", 0);
	textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT4, WHITE, BKG);	// text of monitor fourth line

	sprintf(s, "recoil_task:%2u", 0);
	textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT5, WHITE, BKG);	// text of monitor fifth line

	// plot previous shot results
	rect(screen, LEDGE_PLOT_SHOT, TEDGE_PLOT_SHOT, REDGE_PLOT_SHOT, BEDGE_PLOT_SHOT, ELECTRIC_BLUE);

	// helper key command avaibles
	textout_ex(screen, font, "ARROW KEYS: move the aim", LCOM_TEXT1, TCOM_TEXT1, YELLOW, BKG);
	textout_ex(screen, font, "SPACE KEY: shoot", LCOM_TEXT1, TCOM_TEXT2, YELLOW, BKG);
	textout_ex(screen, font, "C KEY: launch one skeet", LCOM_TEXT1, TCOM_TEXT3, YELLOW, BKG);
	textout_ex(screen, font, "A KEY: - speed skeet", LCOM_TEXT1, TCOM_TEXT4, YELLOW, BKG);
	textout_ex(screen, font, "S KEY: + speed skeet", LCOM_TEXT1, TCOM_TEXT5, YELLOW, BKG);

	// helper mode avaibles
	textout_ex(screen, font, "Skeet speed:", LMODE_TEXT1, TMODE_TEXT1, YELLOW, BKG);
	textout_ex(screen, font, "Low", LMODE_TEXT1L, TMODE_TEXT1, LIGHT_GRAY, BKG);
	textout_ex(screen, font, "Medium", LMODE_TEXT1M, TMODE_TEXT1, YELLOW, BKG);
	textout_ex(screen, font, "High", LMODE_TEXT1H, TMODE_TEXT1, LIGHT_GRAY, BKG);
	textout_ex(screen, font, "X KEY: 'anticipate-shot' mode OFF", LMODE_TEXT2, TMODE_TEXT2, LIGHT_GRAY, BKG);

	// information for user about skeet
	textout_ex(screen, font, "skeet state:", LINFO_TEXT_SKEET, TINFO_TEXT_SKEET, WHITE, BKG);
	textout_ex(screen, font, "READY TO LAUNCH", LINFO_STATE_SKEET, TINFO_TEXT_SKEET, ELECTRIC_GREEN, BKG);

	// information for user about ammo
	textout_ex(screen, font, "ammo:", LINFO_TEXT_AMMO, TINFO_TEXT_AMMO, WHITE, BKG);
	rect(screen, XL_RECT_AMMO1, YT_RECT_AMMO, XR_RECT_AMMO1, YB_RECT_AMMO, NEON_CARROT);
	rect(screen, XL_RECT_AMMO2, YT_RECT_AMMO, XR_RECT_AMMO2, YB_RECT_AMMO, NEON_CARROT);

	//----------------------------------------------------------------------------
	// INITIALIZE semaphores
	//----------------------------------------------------------------------------
	// MUTEX PROTOCOLS
	pthread_mutexattr_init(&matt);
	pthread_mutexattr_setprotocol(&matt, PTHREAD_PRIO_INHERIT);
	// create 3 semaphores with Priority inheritance protocol
	pthread_mutex_init(&mux_skeet_state, &matt);
	pthread_mutex_init(&mux_aim_coordinate, &matt);
	pthread_mutex_init(&mux_debris_small, &matt);

	pthread_mutexattr_destroy(&matt); 	//destroy attributes

	// GENERAL SEMAPHORE FOR SYNCHRONIZATION
	sem_init(&sem_sync_expl, 0, 0);	// first 0  => sem shared among threads
									// second 0	=> sem for synchronization
	//----------------------------------------------------------------------------
	// TASK CREATION
	//----------------------------------------------------------------------------

	task_create(display_task, IDX_DISPLAY, PER_DISPLAY, DREL_DISPLAY, PRIO_DISPLAY);
	
	task_create(com_interp_task, IDX_COM_INTERP, PER_COM_INTERP, DREL_COM_INTERP, PRIO_COM_INTERP);

	return;
}

//----------------------------------------------------------------------------
// INIT_SKEET: initialize skeet state
//----------------------------------------------------------------------------
void init_skeet(int i)
{
int j; 	// index for COLORS4SKEET
float vx_extract; 	// local variable of vx

	// choose color of the skeet randomly
	j = irand(0, DIM_COLORS4SKEET-1);			// choose random color

	// choose range of vx by the difficulty choose by player
	switch(vx_diffic_select)
	{
		case VX_LOW:
			vx_extract = frand(VXMIN_L, VXMAX_L);
			break;
		case VX_MEDIUM:
			vx_extract = frand(VXMIN_M, VXMAX_M);
			break;
		case VX_HIGH:
			vx_extract = frand(VXMIN_H, VXMAX_H);
			break;
	}

	pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state

	// choose randmoly from which side of the screen skeet will be launched [0:left, 1:right]
	skeet[i].side_launch = irand(0,1);
	skeet[i].c = COLORS4SKEET[j];			// color of the i-th skeet
	skeet[i].r = frand(RMIN, RMAX);			// radius of the i-th skeet

	if (skeet[i].side_launch == 0)		// if skeet comes from left
	{
		skeet[i].x = -skeet[i].r - 2.0; 				// -2 is for the edge
		skeet[i].y = frand(HMIN, HMAX);

		// assign vx extract
		skeet[i].vx = vx_extract;
		// vy range is fixed
		skeet[i].vy = frand(VYMIN, VYMAX);		
	}

	if (skeet[i].side_launch == 1)		// if skeet comes from right
	{
		skeet[i].x = skeet[i].r + XRWORLD; 				
		skeet[i].y = frand(HMIN, HMAX);

		// assign vx extract
		skeet[i].vx = -vx_extract;

		// vy range is fixed
		skeet[i].vy = frand(VYMIN, VYMAX);		
	}

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
// DISPLAY_PLOT_SHOT
//-----------------------------------------------------------------------------------------------------
void 	display_plot_shot(int i, int dist_x, int dist_y)
{
int col; 			// skeet col
float r;			// skeet radius
BITMAP *plot_shot;

	pthread_mutex_lock(&mux_skeet_state);	 // wait on the mutex for skeet state
	col = skeet[i].c;
	r = skeet[i].r;
	pthread_mutex_unlock(&mux_skeet_state);	 // signal on the mutex for skeet state

	plot_shot = create_bitmap(WPLOT_SHOT-1, WPLOT_SHOT-1);
	if (!plot_shot)
	{
		printf("Error creating plot_shot bitmap\n");
		sem_post(&sem_sync_expl);		// unblock explosion_task
		exit(1);
	}
	clear_to_color(plot_shot, BKG);

	circle(plot_shot, C_PLOT_SHOT_BP, C_PLOT_SHOT_BP, r, col);
	circlefill(plot_shot, C_PLOT_SHOT_BP+dist_x, C_PLOT_SHOT_BP-dist_y, 2, RED);
	blit(plot_shot, screen, 0, 0, CXPLOT_SHOT-C_PLOT_SHOT_BP+1, CYPLOT_SHOT-C_PLOT_SHOT_BP+1, plot_shot->w, plot_shot->h);
	destroy_bitmap(plot_shot);
}
//-----------------------------------------------------------------------------------------------------
// HANDLE_SHOT: compare global variables to see if the shot hit skeet and in case create explosion_task
//-----------------------------------------------------------------------------------------------------
void handle_shot(int i, int xc, int yc, int r, float dt) 	// xc and yc are the pixel coordinate of the centre of the skeet, r is its radius
{
int aim_x, aim_y;		// local variable to memorize aim coordinate
int dist_shot_sq;		// square of the distance of the aim respect to the skeet centre
int r_sq;				// square of the radius
float vx, vy;			// velocities of skeet int the istant of the shoot
float v_mod; 			// velocity module
float vx_n, vy_n; 		// normalized velocities
int fut_xc, fut_yc; 	// coordinate of future centre (in skeet reference system)
int dist_x, dist_y; 	// distance from aim_x and xc

	pthread_mutex_lock(&mux_aim_coordinate);		// wait on the mutex for aim coordinate
	aim_x = aim.x;
	aim_y = aim.y;
	pthread_mutex_unlock(&mux_aim_coordinate);		// wait on the mutex for aim coordinate

	// when task_create reads aim coordinate it can begin recoil task
	task_create(recoil_task, IDX_RECOIL, PER_RECOIL, DREL_RECOIL, PRIO_RECOIL);

	// rotate the aim reference system into skeet reference system
	aim_x += CAIM;
	aim_y += -HWORLD + CAIM;
	aim_y = -aim_y;

	r_sq = r*r; 	// compute area of skeet

	// if MODE ANTICIPATE SHOT: OFF => calculate the squared distance between the aim and the centre of the skeet
	if (!anticip_shot_f)		// if mode anticipate shot: OFF
	{
		dist_x = aim_x - xc;
		dist_y = aim_y - yc;
		dist_shot_sq = (dist_x)*(dist_x) + (dist_y)*(dist_y);
	}

	// if MODE ANTICIPATE SHOT: ON => calculate the squared distance between the aim and the centre of the FURURE skeet
	if (anticip_shot_f)		// if mode anticipate shot: ON
	{
		//computes the future position of the skeet
		pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state
		vx = skeet[i].vx;
		vy = skeet[i].vy;
		pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state

		v_mod = sqrt(vx*vx + vy*vy);		// compute v module
		vx_n = vx/v_mod;					// compute vx versor
		vy_n = vy/v_mod;					// compute vy versor
		// compute future skeet centre: the smaller the radius the greater the distance and the future x,y center
		fut_xc = (xc + (int) ((float) ALPHA_FUT / (float) r) * vx_n);	// ALPHA_FUT is a proportional constant
		fut_yc = (yc + (int) ((float) ALPHA_FUT / (float) r) * vy_n);
		dist_x = aim_x - fut_xc;
		dist_y = aim_y - fut_yc;

		// printf("xc = %d, yc = %d\n fut_xc = %d, fut_yc = %d\n", xc, yc, fut_xc, fut_yc);
		dist_shot_sq = (dist_x)*(dist_x) + (dist_y)*(dist_y);

		// in any case (hit or missed) memorize fut_xc and fut_yc in the structure
		pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state
		skeet[i].x_last4expl = fut_xc;
		skeet[i].y_last4expl = fut_yc;
		pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state

	}	


	// if SKEET HIT!!!
	if (dist_shot_sq <= r_sq)
	{
		textout_ex(screen, font, "SKEET HIT!   ", LINFO_STATE_SKEET, TINFO_TEXT_SKEET, YELLOW, BKG);
		if (!anticip_shot_f)		// if mode anticipate shot: OFF
		{
			// it ends skeet_task and draw_skeet (display_task)
			skeet_awake = 0;

			// memorize last position of the skeet to bring information to explosion_draw
			pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state
			skeet[i].x_last4expl = skeet[i].x;
			skeet[i].y_last4expl = skeet[i].y;
			pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state

			// it say to explosion_task to start updating debris position
			skeet_explosion = 1;

			sem_post(&sem_sync_expl);					// UNLOCK thread explosion_task

			// plot shot result 
			display_plot_shot(i, dist_x, dist_y);
		}
		if (anticip_shot_f)		// if mode anticipate shot: ON
		{
			display_plot_shot(i, dist_x, dist_y);
			// continue skeet_task until explosion begin: for doing this activate a flag
			handle_anticip_f = 1;
			// memorize the position of the future explosion of the skeet
		}		
	}
	else
	{
		display_plot_shot(i, dist_x, dist_y);
		flash_f = 1;
		msg_user_new = MSG_USER_MISS;
	}
}

//----------------------------------------------------------------------------
// SKEET_TASK: compute the physic of the skeet/skeets 
//----------------------------------------------------------------------------
void* skeet_task(void* arg)
{
int i;								// task index
float dt;							// integration interval
int r, x, y;						// local variable to memorize global variables for the skeet (there is a CASTING!)
char s[30];							// string for monitor deadline messed
int side_launch_local[N_MAX_SKEET];	// local variable to memorize global variables for the skeet

	i = get_task_index(arg);
	dt = TSCALE_SKEET * (float) get_task_period(arg) / 1000;

	// initialize skeet with IDX_SKEET_TASK
	init_skeet(i);

	// before the sleep of skeet_task, explosion_task starts its initialization
	task_create(explosion_task, IDX_EXPLOSION, PER_EXPLOSION, DREL_EXPLOSION, PRIO_EXPLOSION);

	// function that suspend skeet_task for a random time
	sleep_skeet_task(i);

	// when task awakes up set flags and begin the true task
	skeet_awake = 1;
	textout_ex(screen, font, "UP IN THE AIR  ", LINFO_STATE_SKEET, TINFO_TEXT_SKEET, ELECTRIC_BLUE, BKG);

	set_activation(i);

	while(!end && skeet_awake)	// end is set to 0, also skeet_alive until task deads
	{
		pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state

		skeet[i].vy -= G * dt;
		skeet[i].x 	+= skeet[i].vx * dt;
		skeet[i].y 	+= skeet[i].vy * dt - G * dt * dt / 2;

		// printf("%f\n", skeet[i].y);
		x = (int) skeet[i].x;
		y = (int) skeet[i].y;
		r = (int) skeet[i].r;
		side_launch_local[i] = skeet[i].side_launch;	// memorize global variables
		pthread_mutex_unlock(&mux_skeet_state);			// signal on the mutex for skeet state

		// handling the istance when the user hit skeet in anticipate mode ON, until skeet explodes
		if (handle_anticip_f)
		// handle_ant_fun(int x, )
		{	// if skeet is beyond coordinate x where it is supposed to explose (whether it starts from the left or from the right screen side)
			if( ( (x > skeet[i].x_last4expl) && (side_launch_local[i] == 0) ) ||
				( (x < skeet[i].x_last4expl) && (side_launch_local[i] == 1) ) )
			{
				// it ends skeet_task and draw_skeet (display_task)
				skeet_awake = 0;

				// it say to explosion_task to start updating debris position
				skeet_explosion = 1;

				sem_post(&sem_sync_expl);					// UNLOCK thread explosion_task

				handle_anticip_f = 0; 		// reset flag!!
			}
		}

		// if skeet is out of the screen and it came from LEFT or from RIGHT, clear the skeet task when it disappears
		if ( ( (x-r >= XRWORLD) && (side_launch_local[i] == 0) ) || ( (x+r <= XLWORLD) && (side_launch_local[i] == 1) ) )
		{	
			skeet_alive = 0;
			textout_ex(screen, font, "READY TO LAUNCH", LINFO_STATE_SKEET, TINFO_TEXT_SKEET, ELECTRIC_GREEN, BKG);

			skeet_awake = 0;

			// don' do the explosion (REDONDANT)
			skeet_explosion = 0;
			
			sem_post(&sem_sync_expl);			// UNLOCK thread explosion_task
		}

		// if user click SKACE_KEY verify if the shoot hit skeet
		if (shot_f == 1){

			shot_f = 0;

			handle_shot(i, x, y, r, dt);
		}

		// qua andrò ad aggiornare la dimensione dello skeet nel tempo
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
unsigned int color_skeet; 
int r; 				// skeet radius

	pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state

	color_skeet = skeet[IDX_SKEET_TASK].c;
	r = skeet[IDX_SKEET_TASK].r;

	pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state

W_EXPL_BITMAP = 6*r;		// it is a global varible but it does not need a mutex, because it is write once per skeet init
explosion_copy = create_bitmap(W_EXPL_BITMAP, W_EXPL_BITMAP);		// create the explosion_copy BITMAP
clear_to_color(explosion_copy, LIGHT_GRAY);							

int center = 3*r;		// coordinate of the center of explosion_copy

// coordinate del rettangolo generatore di frammenti (è dipendente dal raggio)
int RAD_GEN = r/3; 		// half side of the rect generator

// scriviamo le coordinate del quadrato magenta che genera gli skeet (frame della explosion BITMAP)
int rect_xl = center - RAD_GEN + 1;		// coordinate interne al rettangolo (i pixel che definiscono gli spigoli interni)
int rect_xr = center + RAD_GEN - 1;
int rect_yt = center - RAD_GEN + 1;
int rect_yb = center + RAD_GEN - 1;

int W_DEB_SMALL_BP = 3; 		// (si lavora nel frame dell' explosion BITMAP)

int RAD_BOUND = ALPHA_RAD_BOUND*r;			// bound for desappear debris and to end the task 

int deb_BP_xl; 					// coordinate x left of bitmap i-th generated on the side
int deb_BP_yt; 					// coordinate y top of bitmap i-th generated on the side
int deb_BP_xc; 					// coordinate x center of bitmap i-th generated along the side
int deb_BP_yc; 					// coordinate y center of bitmap i-th generated along the side

int dist_center_lt = (W_DEB_SMALL_BP-1)/2;	// distance from the center of the BITMAP to the left top pixel


	// GENERATION OF BITMAPS AROUND THE RECT GEN FOR EACH SIDE

	int i; 							// index for cycle
	int j;							// index for cycle

	// initializing different types of brides 
	int deb_type_small[MAX_TYPES_DEBRIS_SMALL][SIZE_POLYG_SMALL] = {

									{0,0,0,0, 0,0, 0,2, 1,2}, 		// triangle L 
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
	for(i = 0; i < MAX_TYPES_DEBRIS_SMALL; i++){
		for (j = 0; j < SIZE_POLYG_SMALL; j++){
			deb_small[i].values[j] = deb_type_small[i][j];
		}
	}
//---------------------------------------------------------------------------
	// INITIALIZE VEL AND POS OF DEBRIS ALONG EACH SIDE OF THE RECT_GEN

	// reinitializing index for future use
	i = 0; 	// debris index 
	j = 0;

	// TOP side of rect
	// initializing local variables that wil updating slide along the side
	deb_BP_xl = rect_xl - W_DEB_SMALL_BP;		// x coordinate left for the first BITMAP
	deb_BP_yt = rect_yt - W_DEB_SMALL_BP;		// y coordinate top for the first BITMAP
	deb_BP_xc = deb_BP_xl + dist_center_lt;			// x coordinate center for the first BITMAP
	deb_BP_yc = deb_BP_yt + dist_center_lt;			// y coordinate center for the first BITMAP

	while(deb_BP_xl < rect_xr) 		// top side of rect
	{

		deb_small[i].xl = deb_BP_xl; 			// each bitmap memorize its position in explosion_copy frame
		deb_small[i].yt = deb_BP_yt;			// these two values will used to set the bitmap; there is a RELATION between yt and yc
		deb_small[i].xc = deb_BP_xc - center;	// coordinate of the centre of the BITMAP respect to the frame on the centre of the explosion_copy	
		deb_small[i].yc = -(deb_BP_yc - center);

		deb_small[i].vx = deb_small[i].xc * ALPHA_NORM; 	// compute the x velocity component proportionally to 
		deb_small[i].vx += irand(-ADD_RAND_V,ADD_RAND_V);			// the distance from the center; add random component
		deb_small[i].vy = deb_small[i].yc * ALPHA_NORM;		
		deb_small[i].vy += irand(-ADD_RAND_V,ADD_RAND_V);


		// initialize boundary coordinate for hidding debris when they are too far away (in centre of explosion_copy frame)
		deb_small[i].xbound =  RAD_BOUND - irand(0, MAX_RAND_BOUND);
		deb_small[i].ybound =  RAD_BOUND - irand(0, MAX_RAND_BOUND);

		printf("xc: %f yc: %f\n", deb_small[i].xc, deb_small[i].yc);
		printf("xbound: %d ybound: %d\n", deb_small[i].xbound, deb_small[i].ybound);

		deb_BP_xl += STEP_GEN_SMALL;			// update the local variable as a window
		deb_BP_xc += STEP_GEN_SMALL;

		i++;							// update the index for the generation of BITMAP
	}
	//------------------------------------------------------------------------------
	// RIGHT side of rect

	deb_BP_xl = rect_xr + 1;					// x coordinate left for the first BITMAP
	deb_BP_yt = rect_yt - W_DEB_SMALL_BP;		// y coordinate top for the first BITMAP
	deb_BP_xc = deb_BP_xl + dist_center_lt;		// x coordinate center for the first BITMAP
	deb_BP_yc = deb_BP_yt + dist_center_lt;		// y coordinate center for the first BITMAP

	while(deb_BP_yt < rect_yb) 
	{

		deb_small[i].xl = deb_BP_xl;
		deb_small[i].yt = deb_BP_yt;
		deb_small[i].xc = deb_BP_xc - center;
		deb_small[i].yc = -(deb_BP_yc - center);

		deb_small[i].vx = deb_small[i].xc * ALPHA_NORM;
		deb_small[i].vx += irand(-ADD_RAND_V,ADD_RAND_V);
		deb_small[i].vy = deb_small[i].yc * ALPHA_NORM;
		deb_small[i].vy += irand(-ADD_RAND_V,ADD_RAND_V);

		printf("vx: %3.1f vy: %3.1f\n", deb_small[i].vx, deb_small[i].vy);

		// initialize boundary coordinate for hidding debris when they are too far away (in centre of explosion_copy frame)
		deb_small[i].xbound =  RAD_BOUND - irand(0, MAX_RAND_BOUND);
		deb_small[i].ybound =  RAD_BOUND - irand(0, MAX_RAND_BOUND);

		deb_BP_yt += STEP_GEN_SMALL;
		deb_BP_yc += STEP_GEN_SMALL;

		i++;					// update the index for the generation of BITMAP
	}
//------------------------------------------------------------------------------
	// BOTTOM side of rect

	deb_BP_xl = rect_xr + 1;					// x coordinate left for the first BITMAP
	deb_BP_yt = rect_yb + 1;					// y coordinate top for the first BITMAP
	deb_BP_xc = deb_BP_xl + dist_center_lt;		// x coordinate center for the first BITMAP
	deb_BP_yc = deb_BP_yt + dist_center_lt;		// y coordinate center for the first BITMAP

	while(deb_BP_xl > (rect_xl - W_DEB_SMALL_BP)) 
	{

		deb_small[i].xl = deb_BP_xl;
		deb_small[i].yt = deb_BP_yt;
		deb_small[i].xc = deb_BP_xc - center;
		deb_small[i].yc = -(deb_BP_yc - center);

		deb_small[i].vx = deb_small[i].xc * ALPHA_NORM;
		deb_small[i].vx += irand(-ADD_RAND_V,ADD_RAND_V);
		deb_small[i].vy = deb_small[i].yc * ALPHA_NORM;
		deb_small[i].vy += irand(-ADD_RAND_V,ADD_RAND_V);

		printf("vx: %3.1f vy: %3.1f\n", deb_small[i].vx, deb_small[i].vy);

		// initialize boundary coordinate for hidding debris when they are too far away (in centre of explosion_copy frame)
		deb_small[i].xbound =  RAD_BOUND - irand(0, MAX_RAND_BOUND);
		deb_small[i].ybound =  RAD_BOUND - irand(0, MAX_RAND_BOUND);

		deb_BP_xl -= STEP_GEN_SMALL;
		deb_BP_xc -= STEP_GEN_SMALL;

		i++;					// update the index for the generation of BITMAP
	}
	//------------------------------------------------------------------------------
	// LEFT side of rect

	deb_BP_xl = rect_xl - W_DEB_SMALL_BP;		// x coordinate left for the first BITMAP
	deb_BP_yt = rect_yb + 1;					// y coordinate top for the first BITMAP
	deb_BP_xc = deb_BP_xl + dist_center_lt;		// x coordinate center for the first BITMAP
	deb_BP_yc = deb_BP_yt + dist_center_lt;		// y coordinate center for the first BITMAP

	while(deb_BP_yt > (rect_yt - W_DEB_SMALL_BP)) 
	{

		deb_small[i].xl = deb_BP_xl;
		deb_small[i].yt = deb_BP_yt;
		deb_small[i].xc = deb_BP_xc - center;
		deb_small[i].yc = -(deb_BP_yc - center);

		deb_small[i].vx = deb_small[i].xc * ALPHA_NORM;
		deb_small[i].vx += irand(-ADD_RAND_V,ADD_RAND_V);
		deb_small[i].vy = deb_small[i].yc * ALPHA_NORM;
		deb_small[i].vy += irand(-ADD_RAND_V,ADD_RAND_V);

		printf("vx: %3.1f vy: %3.1f\n", deb_small[i].vx, deb_small[i].vy);

		// initialize boundary coordinate for hidding debris when they are too far away (in centre of explosion_copy frame)
		deb_small[i].xbound =  RAD_BOUND - irand(0, MAX_RAND_BOUND);
		deb_small[i].ybound =  RAD_BOUND - irand(0, MAX_RAND_BOUND);

		deb_BP_yt -= STEP_GEN_SMALL;
		deb_BP_yc -= STEP_GEN_SMALL;

		i++;					// update the index for the generation of BITMAP
	}

	debris_small_gen = i;		// total number of generated debris
	printf("%d\n", i);

	// memorize color of each debris
	for (i = 0; i < debris_small_gen; i++)
	{
		deb_small[i].col = color_skeet;		// all debris are initialized with the skeet color
		deb_small[i].alive_stat = 1; 		// all debris are initialized as alive
	}

	float dt;
	dt = TSCALE_DEBRIS * (float) get_task_period(arg) / 1000;

	printf("LOCK EXPLOSION\n");
	sem_wait(&sem_sync_expl);		// block thread until user hit the skeet OR skeet disappears

	printf("UNLOCK EXPLOSION\n");

	int n_hide_deb = 0;		// number of the hide debris

	char s[30];			// string for monitor deadline messed
	int idx_expl;
	idx_expl = get_task_index(arg);
	set_activation(idx_expl);

	while(!end && skeet_explosion) 		// if skeet is lost thread will be unlock but it will skip itself
	{													// otherwise task will terminate after all debris are hide

		pthread_mutex_lock(&mux_debris_small); 			// wait on the mutex for debris_small

		for(i = 0; i < debris_small_gen; i++)
		{
			deb_small[i].xc += deb_small[i].vx * dt;
			deb_small[i].yc += deb_small[i].vy * dt;

			// if one of the bounds are cros and if bedris is still coloured
			if ( ( (abs(deb_small[i].xc) >= deb_small[i].xbound) ||
				 (abs(deb_small[i].yc >= deb_small[i].ybound)) ) && (deb_small[i].alive_stat) ){
				deb_small[i].col = BKG;
				deb_small[i].alive_stat = 0;
				n_hide_deb ++;
				printf("%d\n", n_hide_deb);
			}

			deb_small[i].xl  =  deb_small[i].xc + center - dist_center_lt;
			deb_small[i].yt  = -deb_small[i].yc + center - dist_center_lt;

			// printf(" i = %d => vx: %3.1f  vy: %3.1f\n", i, deb_small[i].vx, deb_small[i].vy);

		}
		pthread_mutex_unlock(&mux_debris_small); 			// wait on the mutex for debris_small

		// when all debris are hidden we can terminate explosion_task and draw_explosion, and user is free to ask for another skeet

		if(n_hide_deb >= debris_small_gen - 2)
		{
			// terminate draw_explosion and explosion task
			skeet_explosion = 0;

			// user is able to ask another skeet
			skeet_alive = 0;
			textout_ex(screen, font, "READY TO LAUNCH", LINFO_STATE_SKEET, TINFO_TEXT_SKEET, ELECTRIC_GREEN, BKG);
		}

		if (deadline_miss(idx_expl))
		{
			expl_tot_dl_m ++;		// it is not possible to use param[i].dmiss because it clears every time the task is cleared
			sprintf(s, "explosion_task:%2u", expl_tot_dl_m);
			textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT4, WHITE, BKG);	// text of monitor third line
		}
		wait_for_activation(idx_expl);
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
unsigned int skeet_col; 					// random color
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

void draw_explosion(int i, BITMAP* world_copy, BITMAP* explosion_copy)
{
int j;
int k;

	pthread_mutex_lock(&mux_debris_small); 			// wait on the mutex for debris_small

	for (j = 0; j < debris_small_gen; j++)
	{
		BITMAP* one_BP;
		one_BP = create_bitmap(3, 3);	// creating BITMAP
		if (!one_BP)
		{
			printf("Error creating one_BP BITMAP\n");
		}
		clear_to_color(one_BP, PINK);

		k = irand(0,9);						// random choice a type of debris to make a rotater illusion effect to the user
		polygon(one_BP, 5, deb_small[k].values, deb_small[j].col);
		draw_sprite(explosion_copy, one_BP, deb_small[j].xl, deb_small[j].yt);

		destroy_bitmap(one_BP);
	}

	pthread_mutex_unlock(&mux_debris_small); 		// signal on the mutex for debris_small

	blit(explosion_copy, world_copy, 0, 0, skeet[i].x_last4expl - W_EXPL_BITMAP/2, -(skeet[i].y_last4expl - YBWORLD + YTWORLD + W_EXPL_BITMAP/2),
	explosion_copy->w, explosion_copy->h);	

}

//----------------------------------------------------------------------------
// DRAW_SHOT_SEQ: draw the shot sequence shotgun
//----------------------------------------------------------------------------
void draw_shot_seq(BITMAP *world_copy, BITMAP *shot_seqBP[], int x, int y, int shot_seq_idx)
{
	int aim_x, aim_y; 	// memorize aim coordinate

	pthread_mutex_lock(&mux_aim_coordinate); 	// wait on the mutex for aim coordinate
	aim_x = aim.x;
	aim_y = aim.y;
	pthread_mutex_unlock(&mux_aim_coordinate); 	// signal on the mutex for aim coordinate

	// handle shotgun BP y bounds
	x += aim_x/ALPHAX;		// update x coordinate for shotgun BP (scaling the aim coordinate)
	if(aim_y >= 0 && aim_y <= BLIM_SHOTGUN)	// if shotgun BP doesn't touch bounds
		y += aim_y/ALPHAY;	// update y coordinate for shotgun BP (scaling the aim coordinate)
	else if(aim_y > BLIM_SHOTGUN) 	// if shotgun BP is too low
		y += MAX_Y_SHOTGUN;			// y coordinate reamins constant

	draw_sprite(world_copy, shot_seqBP[shot_seq_idx], x, y);
}

//----------------------------------------------------------------------------
// DRAW_RELOAD_SEQ: draw the reload sequence shotgun
//----------------------------------------------------------------------------
void draw_reload_seq(BITMAP *world_copy, BITMAP *reload_seqBP[], int x, int y, int rel_seq_idx)
{
	int aim_x, aim_y; 	// memorize aim coordinate

	pthread_mutex_lock(&mux_aim_coordinate); 	// wait on the mutex for aim coordinate
	aim_x = aim.x;
	aim_y = aim.y;
	pthread_mutex_unlock(&mux_aim_coordinate); 	// signal on the mutex for aim coordinate

	// handle shotgun BP y bounds
	x += aim_x/ALPHAX;		// update x coordinate for shotgun BP (scaling the aim coordinate)
	if(aim_y >= 0 && aim_y <= BLIM_SHOTGUN)	// if shotgun BP doesn't touch bounds
		y += aim_y/ALPHAY;	// update y coordinate for shotgun BP (scaling the aim coordinate)
	else if(aim_y > BLIM_SHOTGUN) 	// if shotgun BP is too low
		y += MAX_Y_SHOTGUN;			// y coordinate reamins constant

	draw_sprite(world_copy, reload_seqBP[rel_seq_idx], x, y);
}
//----------------------------------------------------------------------------
// DRAW_SHOTGUN: if no sequence are called draw classic shotgun bitmap
//----------------------------------------------------------------------------
void draw_shotgun(BITMAP *world_copy, BITMAP *shotgun_img, int x, int y)
{
int aim_x, aim_y; 	// memorize aim coordinate

	pthread_mutex_lock(&mux_aim_coordinate); 	// wait on the mutex for aim coordinate
	aim_x = aim.x;
	aim_y = aim.y;
	pthread_mutex_unlock(&mux_aim_coordinate); 	// signal on the mutex for aim coordinate

	// handle shotgun BP y bounds
	x += aim_x/ALPHAX;		// update x coordinate for shotgun BP (scaling the aim coordinate)
	if(aim_y >= 0 && aim_y <= BLIM_SHOTGUN)	// if shotgun BP doesn't touch bounds
		y += aim_y/ALPHAY;	// update y coordinate for shotgun BP (scaling the aim coordinate)
	else if(aim_y > BLIM_SHOTGUN) 	// if shotgun BP is too low
		y += MAX_Y_SHOTGUN;			// y coordinate reamins constant

	draw_sprite(world_copy, shotgun_img, x, y);
}
//----------------------------------------------------------------------------
// DISPLAY_TASK (TASK): 
//----------------------------------------------------------------------------
void* display_task(void* arg)
{
int i;						// task index
int j; 						// index for loading seq images
char s[30];					// string for monitor deadline messed
int x0_shotgun;				// initial position of shotgun
int y0_shotgun;

unsigned int col_msg; 					// color code 
int prev_col_f = 0;						// flag to inform wich color was (0:black, 1:white)
int iter = 0;							// number of cycles BLACK-WHITE-BLACK
struct rgb_hsv msg;						// initialize the local structure

int rel_seq_idx = 0; 			// index for reloading sequence images
int shot_seq_idx = 0;			// index fo shot sequence images

	msg.r = 0, msg.g = 0, msg.b = 0;	// rgb values of color of user message (initialized as BLACK)
	msg.h = 0, msg.s = 0, msg.v = 0; 	// HSV of color of user message (initialized as BLACK)

	BITMAP* world_copy;
	world_copy = create_bitmap(WWORLD, HWORLD);		// create a bitmap with the resolution of the graphic world
	if (!world_copy)
	{
		printf("Error creating world_copy BITMAP\n");
		sem_post(&sem_sync_expl);		// unblock explosion_task
		exit(1);
	}

	BITMAP* skeet_image;
	skeet_image = create_bitmap(WSKEET, HSKEET); 	// create a bitmap with the resolution of the biggest skeet!
	if (!skeet_image)
	{
		printf("Error creating skeet_image BITMAP\n");
		sem_post(&sem_sync_expl);		// unblock explosion_task
		exit(1);
	}
	clear_to_color(skeet_image, BKG);				// color the whole virtual screen for the skeet of black

	BITMAP *shotgun_img;
	shotgun_img = load_bitmap("shotgunP.bmp", NULL);
	if (!shotgun_img)
	{
		printf("Error loading shotgun\n");
		sem_post(&sem_sync_expl);		// unblock explosion_task
		exit(1);
	}
	x0_shotgun = XRWORLD - shotgun_img->w -125;
	y0_shotgun = YBWORLD - (shotgun_img->h + SHOTGUN_FRAME_B);

	// load bitmaps for reload shotgun sequence
	BITMAP *reload_seqBP[N_RELOAD_IMG];
	for(j = 0; j < N_RELOAD_IMG; j++)
	{
		reload_seqBP[j] = load_bitmap(reload_seqFN[j], NULL); // load the image of the sequence of index rel_seq_idx
		if (!reload_seqBP[j])
		{
			printf("Error loading reload sequence\n");
			sem_post(&sem_sync_expl);		// unblock explosion_task
			exit(1);
		}
	}
	// load bitmaps for shotgun shot sequence
	BITMAP *shot_seqBP[N_SHOT_IMG];
	for(j = 0; j < N_SHOT_IMG; j++)
	{
		shot_seqBP[j] = load_bitmap(shot_seqFN[j], NULL);
		if (!shot_seqBP[j])
		{
			printf("Error loading shot image number %d\n", shot_seq_idx);
			sem_post(&sem_sync_expl);		// unblock explosion_task
			exit(1);
		}
	}

	i = get_task_index(arg);
	set_activation(i);

	while(!end)
	{

		clear_to_color(world_copy, BKG);				// color the whole virtual screen for the graphic world of black

		// STARTS TO DRAW FROM HERE

		// if skeet is launched and it is awake draw_skeet	
		if (skeet_awake)
		{
			clear_to_color(skeet_image, BKG);				// color the whole virtual screen for the skeet of black

			draw_skeet(IDX_SKEET_TASK, world_copy, skeet_image);
		}

		// check if skeet is hit to draw it from data provided by explosion task
		if (skeet_explosion)
		{
			clear_to_color(explosion_copy, BKG);								// clear the virtual screen "skeet_image"

			draw_explosion(IDX_SKEET_TASK, world_copy, explosion_copy);
		}

		draw_aim(world_copy);

		// if skeet is called reload shotgun with the new ammo
		if(reload_seq_f)
		{
			draw_reload_seq(world_copy, reload_seqBP, x0_shotgun, y0_shotgun, rel_seq_idx);
			rel_seq_idx++;
			if (rel_seq_idx == N_RELOAD_IMG)
			{
				reload_seq_f = 0; 		// stop to draw reloading sequence
				rel_seq_idx = 0;		// restart reloading sequence index
			}
		}
		// if skeet is called reload shotgun with the new ammo
		else if(shot_seq_f)
		{
			draw_shot_seq(world_copy, shot_seqBP, x0_shotgun, y0_shotgun, shot_seq_idx);
			shot_seq_idx++;
			if (shot_seq_idx == N_SHOT_IMG)
			{
				shot_seq_f = 0; 		// stop to draw reloading sequence
				shot_seq_idx = 0;		// restart reloading sequence index
				// start reloading sequence after shot
				reload_seq_f = 1;
			}
		}
		// if no sequence are called, draw traditional shotgun
		else 
			draw_shotgun(world_copy, shotgun_img, x0_shotgun, y0_shotgun);


		// copy the rectangular area from world_copy (virtual graphic world) to screen (real graphic world)
		blit(world_copy, screen, 0, 0, XLWORLD, YTWORLD, WWORLD, HWORLD);

		// write_graphic inserisco valori aggiornati (es: dmiss, velocità skeet, distanza ecc..)
		if(flash_f)
		{
			rgb_to_hsv(msg.r, msg.g, msg.b, &msg.h, &msg.s, &msg.v);
			if(!prev_col_f) 	msg.v += STEP_FLASH_TEXT;	// if previous color was black
			else 				msg.v -= STEP_FLASH_TEXT;	// if previous color was white
			if(msg.v >= 1){ 	// if color has reached the white
				msg.v = 1;
				prev_col_f = 1;
			}
			if(msg.v <= 0){ 	// if color has reached the black
				msg.v = 0;
				prev_col_f = 0;
				iter++;
				if(iter >= N_MAX_FLASH){
					iter = 0;
					flash_f = 0; 	// terminate to flash text
				}
			}
			hsv_to_rgb(msg.h, msg.s, msg.v, &msg.r, &msg.g, &msg.b);
			col_msg = makecol(msg.r, msg.g, msg.b);

			// handle msg user (policy: show last message)
			switch (msg_user_new)
			{
				case MSG_USER_WAST_SHOT:	// wasted shot because skeet is not generated yet
					if (msg_user_new != msg_user_prev)
					{
						msg.r = 0; msg.g = 0; msg.b = 0; 	// restart color of the text and wait next task period
						textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
						iter = 0; 							// restart number of flash cycles
						msg_user_prev = msg_user_new;
						break;
					}
					textout_ex(screen, font, "Shot wasted", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
					msg_user_prev = MSG_USER_WAST_SHOT;
				break;

				case MSG_USER_MISS:	//missed skeet message
					if (msg_user_new != msg_user_prev)
					{
						msg.r = 0; msg.g = 0; msg.b = 0; 	// restart color of the text and wait next task period
						textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
						iter = 0; 							// restart number of flash cycles
						msg_user_prev = msg_user_new;
						break;
					}
					textout_ex(screen, font, "Ops, missed skeet", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
					msg_user_prev = MSG_USER_MISS;
				break;

				case MSG_USER_OOA: 	// out of ammo message
					if (msg_user_new != msg_user_prev)
					{
						msg.r = 0; msg.g = 0; msg.b = 0; 	// restart color of the text and wait next task period
						textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
						iter = 0; 							// restart number of flash cycles
						msg_user_prev = msg_user_new;
						break;
					}
					textout_ex(screen, font, "Out of ammo!", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
					msg_user_prev = MSG_USER_OOA;
				break;

				case MSG_USER_WAIT_MODE: 	// wait for change mode
					if (msg_user_new != msg_user_prev)
					{
						msg.r = 0; msg.g = 0; msg.b = 0; 	// restart color of the text and wait next task period
						textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
						iter = 0; 							// restart number of flash cycles
						msg_user_prev = msg_user_new;
						break;
					}
					textout_ex(screen, font, "can't change mode now", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
					msg_user_prev = MSG_USER_WAIT_MODE;
				break;

				case MSG_USER_WAIT_SPEED: 	// wait for change speed
					if (msg_user_new != msg_user_prev)
					{
						msg.r = 0; msg.g = 0; msg.b = 0; 	// restart color of the text and wait next task period
						textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
						iter = 0; 							// restart number of flash cycles
						msg_user_prev = msg_user_new;
						break;
					}
					textout_ex(screen, font, "can't change speed now", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
					msg_user_prev = MSG_USER_WAIT_SPEED;
				break;

				case MSG_USER_OOS: 		// wait for next skeet, while previous is exploding
					if (msg_user_new != msg_user_prev)
					{
						msg.r = 0; msg.g = 0; msg.b = 0; 	// restart color of the text and wait next task period
						textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
						iter = 0; 							// restart number of flash cycles
						msg_user_prev = msg_user_new;
						break;
					}
					textout_ex(screen, font, "reloading next skeet...", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
					msg_user_prev = MSG_USER_OOS;
				break;

				case MSG_USER_KNR: 		// key not recognized
					if (msg_user_new != msg_user_prev)
					{
						msg.r = 0; msg.g = 0; msg.b = 0; 	// restart color of the text and wait next task period
						textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
						iter = 0; 							// restart number of flash cycles
						msg_user_prev = msg_user_new;
						break;
					}
					textout_ex(screen, font, "key not recognized", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
					msg_user_prev = MSG_USER_KNR;
				break;

				default:
					break;
			}
		}

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
// RECOIL_TASK: compute the displacement of the aim after the shoot
//----------------------------------------------------------------------------
void*	recoil_task(void* arg)
{
char s[30];					// string for monitor deadline messed
int i;						// task index
int stop_f = 0; 			// flag to stop recoil_task
int displace_up = 10;		// number of pixel to move towards up
int displace_rand;		// number of pixel to move in a random direction (left:0 or right:1)
int direct;					// direction choose by irand
int iter;
int max_iter_up = 5;	 	// number of iterations and max iterations of recoil_task
int max_iter_rand;			// number of max iterations in random direction

	direct = irand(0,1);
	displace_rand = irand(1,3);
	max_iter_rand = irand(2,4);
	i = get_task_index(arg);
	set_activation(i);

	while (!end && !stop_f)
	{
		pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
		// you make sure that the aim don't go out of the screen
		if( (!direct) && (aim.x >= XL_LIM_AIM) && (iter != max_iter_rand)) aim.x -= displace_rand;	// left

		if( ( direct) && (aim.x <= XR_LIM_AIM) && (iter != max_iter_rand)) aim.x += displace_rand;	// right

		
		if (aim.y >= YT_LIM_AIM) aim.y -= displace_up;			// make sure that the aim don't go out of the screen

		pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate

		iter++;
		if(iter == max_iter_up) stop_f = 1;

		if (deadline_miss(i))
		{
			sprintf(s, "recoil_task:%2u", param[i].dmiss);
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
char s[30];				// string for monitor deadline messed
int i;					// task index
int ammo = 0;			// number of attempts to hit the skeet per skeet; it is initialized to 0 until first press of KEY_C is verified

	BITMAP *ammo_img;
	ammo_img = load_bitmap("ammo.bmp", NULL);
	if (!ammo_img)
	{
		printf("Error loading ammo\n");
		sem_post(&sem_sync_expl);		// unblock explosion_task
		exit(1);
	}

	i = get_task_index(arg);
	set_activation(i);

	do
	{	
		if(key[KEY_UP])
		{				
			pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
			if (aim.y >= YT_LIM_AIM) 	aim.y -= STEP_AIM;				// make sure that the aim don't go out of the screen
			pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		}

		if(key[KEY_DOWN])
		{
			pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
			if (aim.y <= YB_LIM_AIM) 	aim.y += STEP_AIM;				// make sure that the aim don't go out of the screen						
			pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		}

		if(key[KEY_LEFT])
		{
			pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
			if (aim.x >= XL_LIM_AIM) 	aim.x -= STEP_AIM;				// make sure that the aim don't go out of the screen	
			pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		}

		if(key[KEY_RIGHT])
		{
			pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
			if (aim.x <= XR_LIM_AIM) 	aim.x += STEP_AIM;				// you make sure that the aim don't go out of the screen	
			pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		}
					
		scan = 0;
		if (keypressed())	// returns true if there are characters in keyboard buffer, else false (it does not block the program execution)
		{
			scan = readkey() >> 8;		// shift right the high byte	
			switch (scan)
			{

				case KEY_A:
					if (skeet_alive == 0)
					{
						switch(vx_diffic_select){
							case VX_HIGH:
								textout_ex(screen, font, "High", LMODE_TEXT1H, TMODE_TEXT1, LIGHT_GRAY, BKG);
								textout_ex(screen, font, "Medium", LMODE_TEXT1M, TMODE_TEXT1, YELLOW, BKG);
								vx_diffic_select = VX_MEDIUM;
								break;
							case VX_MEDIUM:
								textout_ex(screen, font, "Medium", LMODE_TEXT1M, TMODE_TEXT1, LIGHT_GRAY, BKG);
								textout_ex(screen, font, "Low", LMODE_TEXT1L, TMODE_TEXT1, YELLOW, BKG);
								vx_diffic_select = VX_LOW;
								break;
						}
					}
					else{
					flash_f = 1;
					msg_user_new = 	MSG_USER_WAIT_SPEED;
					}
					break;

				case KEY_S:
					if (skeet_alive == 0)
					{
						switch(vx_diffic_select){
							case VX_LOW:
								textout_ex(screen, font, "Low", LMODE_TEXT1L, TMODE_TEXT1, LIGHT_GRAY, BKG);
								textout_ex(screen, font, "Medium", LMODE_TEXT1M, TMODE_TEXT1, YELLOW, BKG);
								vx_diffic_select = VX_MEDIUM;
								break;
							case VX_MEDIUM:
								textout_ex(screen, font, "Medium", LMODE_TEXT1M, TMODE_TEXT1, LIGHT_GRAY, BKG);
								textout_ex(screen, font, "High", LMODE_TEXT1H, TMODE_TEXT1, YELLOW, BKG);
								vx_diffic_select = VX_HIGH;
								break;							
						}
					}
					else{
					flash_f = 1;
					msg_user_new = 	MSG_USER_WAIT_SPEED;
					}

					break;

				case KEY_X:		// CHANGE MODE: bisogna anticipare il tiro rispetto allo skeet, solo se lo skeet non è in aria
					if ( (anticip_shot_f == 0) && (skeet_alive == 0)) 	// if mode is OFF change to ON
					{
						anticip_shot_f = 1;
						textout_ex(screen, font, "X_KEY: 'anticipate-shot' mode ON ", LMODE_TEXT2, TMODE_TEXT2, YELLOW, BKG);
						textout_ex(screen, font, "[calculate right time before shooting]", LDESCRIP_TEXT2, TDESCRIP_TEXT2, WHITE, BKG);
					}
					else if ( (anticip_shot_f == 1) && (skeet_alive == 0))	// if mode is ON change to OFF
					{
						anticip_shot_f = 0;
						textout_ex(screen, font, "X_KEY: 'anticipate-shot' mode OFF", LMODE_TEXT2, TMODE_TEXT2, LIGHT_GRAY, BKG);
						textout_ex(screen, font, "[calculate right time before shooting]", LDESCRIP_TEXT2, TDESCRIP_TEXT2, BKG, BKG);
					}
					else if (skeet_alive != 0)
					{
						flash_f = 1;
						msg_user_new = 	MSG_USER_WAIT_MODE;
					}
					break;

				case KEY_C:

					if (skeet_alive == 0)		// if no skeet are in the window
					{
						textout_ex(screen, font, "CALLED         ", LINFO_STATE_SKEET, TINFO_TEXT_SKEET, NEON_CARROT, BKG);
						ammo = 2;								// reload the rifle
						blit(ammo_img, screen, 0, 0, XL_AMMO1_BP, YT_AMMO_BP, ammo_img->w, ammo_img->h); // draw ammo BITMAP1
						blit(ammo_img, screen, 0, 0, XL_AMMO2_BP, YT_AMMO_BP, ammo_img->w, ammo_img->h); // draw ammo BITMAP2

						skeet_alive = 1;
						task_create(skeet_task, IDX_SKEET_TASK, PER_SKEET_TASK, DREL_SKEET_TASK, PRIO_SKEET_TASK); // idx becomes 1

						// draw the reloadin sequence of the shotgun 
						reload_seq_f = 1;

						// clear plot_shot
						rectfill(screen, LEDGE_PLOT_SHOT+1, TEDGE_PLOT_SHOT+1, REDGE_PLOT_SHOT-1, BEDGE_PLOT_SHOT-1, BKG);
						break;
					}
					else
					{
						flash_f = 1;
						msg_user_new = MSG_USER_OOS;
						break;
					}

				case KEY_SPACE:
					if ((ammo > 0) && (!handle_anticip_f))		// second condition is for an unlucky case : second shot pressed,
					{											// while first explosion has begin yet (in anticip mode)
						ammo--;

						// tells to draw_skeet to draw the shot sequence
						shot_seq_f = 1;

						if(ammo == 1)
						rectfill(screen, XL_AMMO2_BP, YT_AMMO_BP, XL_AMMO2_BP + ammo_img->w, YT_AMMO_BP + ammo_img->h, BKG); //cancel ammo BITMAP2
						if(ammo == 0)
						rectfill(screen, XL_AMMO1_BP, YT_AMMO_BP, XL_AMMO1_BP + ammo_img->w, YT_AMMO_BP + ammo_img->h, BKG); //cancel ammo BITMAP1

						if(!skeet_awake)	// wasted shot, no need to compute if player hit skeet
						{

							task_create(recoil_task, IDX_RECOIL, PER_RECOIL, DREL_RECOIL, PRIO_RECOIL);
							flash_f = 1;
							msg_user_new = MSG_USER_WAST_SHOT;
						}
						else				// if skeet is up in the air
						{
							shot_f = 1;		// comunicate to skeet_task to handle the shot
						}
					}
					else if ( (ammo == 1) && (handle_anticip_f) ){
						printf("Recharging\n"); // caso raro in cui con due colpi consecutivi centri il bersaglio doppiamente mentre skeet_task
						ammo--;					// aspetta che sia arrivato nella posizione futura
					}
					else
					{
						flash_f = 1;
						msg_user_new = MSG_USER_OOA;
						printf("Out of Ammo\n");
					}
					break;

				default:	// wrong key pressed
					if (scan != KEY_ESC && scan != KEY_UP && scan != KEY_DOWN && scan != KEY_RIGHT && scan != KEY_LEFT)
					{
						flash_f = 1;
						msg_user_new = MSG_USER_KNR;
					}
					break;
				// BISOGNA INSERIRE COSA FARE SE CLICCO ALTRI TASTI
			}

		}
								// clear_keybuf();
		if (deadline_miss(i))
		{
			sprintf(s, "com_interp_task:%2u", param[i].dmiss); 
			textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT1, WHITE, BKG);	// text of monitor first line
		}
	wait_for_activation(i);
	} while (scan != KEY_ESC);				// end of do{}
	// when user press KEY_ESC all threads must terminate!

	sem_post(&sem_sync_expl); 				// UNLOCK thread explosion_task
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