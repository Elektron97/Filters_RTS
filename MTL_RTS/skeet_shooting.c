
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
//-----------------------USER INFORMATION CONSTANT----------------------------------
#define XWIN 		1024		// window x resolution
#define YWIN 		768			// window y resolution
#define X_VERT_LINE 850			// distance of vertial line
#define Y_ORIZ_LINE 100			// distance of orizzontal line

//-----------------------DEADLINE MISSED INFO CONSTANT----------------------------------
#define LEDGE_RECT_DMISS	(X_VERT_LINE + 8)		// left edge rect dmiss
#define TEDGE_RECT_DMISS	5						// top edge rect dmiss
#define REDGE_RECT_DMISS	(X_VERT_LINE + 165)		// right edge rect dmiss
#define BEDGE_RECT_DMISS	158					// bottom edge rect dmiss
#define LEDGE_MON_TITLE 	(X_VERT_LINE + 25)		// left edge monitor title
#define TEDGE_MON_TITLE 	10						// top edge monitor title
#define LEDGE_MON_TEXT	 	(X_VERT_LINE + 15)		// left edge monitor text
#define TEDGE_MON_TEXT1 	25						// top edge monitor text first line
#define TEDGE_MON_TEXT2 	45						// top edge monitor text second line
#define TEDGE_MON_TEXT3 	65						// top edge monitor text third line
#define TEDGE_MON_TEXT4 	85						// top edge monitor text fourth line
#define TEDGE_MON_TEXT5 	105						// top edge monitor text fifth line
#define TEDGE_MON_TEXT6 	125						// top edge monitor text fifth line
#define TEDGE_MON_TEXT7 	145						// top edge monitor text fifth line
//-----------------------PLOT_MOTOR CONSTANT-------------------------------------------
// PLOT X
#define HPLOT_UX 			141							// height of input ux send to motor (ux)
#define WPLOT_UX 			161							// width of input ux send to motor (ux)

#define LPLOT_UX 			(X_VERT_LINE + 6)			// left edge of plot of input ux send to motor (ux)
#define TPLOT_UX 			170							// top edge of plot of input ux send to motor (ux)
#define RPLOT_UX 			(LPLOT_UX+WPLOT_UX)			// right edge of plot of input ux send to motor (ux)
#define BPLOT_UX 			(TPLOT_UX+HPLOT_UX)			// bottom edge of plot of input ux send to motor (ux)
#define LHORIZ_UX			LPLOT_UX					// left edge of horizontal axis (ux)
#define THORIZ_UX			(TPLOT_UX +(HPLOT_UX/2))	// top edge of horizontal axis (ux)
#define RHORIZ_UX			(LHORIZ_UX + WPLOT_UX) 		// right edge of horizontal axis (ux)

#define LINFO_UX			(LPLOT_UX)		// left edge of dynamic information about ux
#define TINFO_UX			(BPLOT_UX+8)	// right edge of dynamic information about ux

// PLOT Y
#define WPLOT_UY 			(WPLOT_UX)					// width of input uy send to motor (uy)
#define HPLOT_UY 			(HPLOT_UX)					// height of input ux send to motor (uy)

#define LPLOT_UY 			(LPLOT_UX)					// left edge of plot of input uy send to motor (uy)
#define TPLOT_UY 			(TPLOT_UX+HPLOT_UX + 20)	// top edge of plot of input uy send to motor (uy)
#define RPLOT_UY 			(LPLOT_UY+WPLOT_UY)			// right edge of plot of input uy send to motor (uy)
#define BPLOT_UY 			(TPLOT_UY+HPLOT_UY)			// bottom edge of plot of input uy send to motor (uy)
#define LHORIZ_UY			(LHORIZ_UX) 				// left edge of horizontal axis (uy)
#define THORIZ_UY			(TPLOT_UY + (WPLOT_UY)/2)	// top edge of horizontal axis (uy)
#define RHORIZ_UY			(LHORIZ_UY + WPLOT_UY) 		// right edge of horizontal axis (uy)

#define LINFO_UY			(LPLOT_UY)		// left edge of dynamic information about uy
#define TINFO_UY			(BPLOT_UY+8)	// right edge of dynamic information about uy

//-----------------------MOTOR PARAMETERS EDITABLES------------------------------------------
#define LPARAM_FSX			(LINFO_UX+90)	// right edge of dynamic information about factor scale x
#define TPARAM_FSX			(TINFO_UX)		// right edge of dynamic information about factor scale x
#define LPARAM_FSY		(LINFO_UY+90)					// right edge of dynamic information about factor scale y
#define TPARAM_FSY		(TINFO_UY)						// right edge of dynamic information about factor scale y
#define TPARAM1			(TINFO_UY + 20)						// top edge of first row of parameters
#define TPARAM2			(TPARAM1 + 15)						// top edge of second row of parameters
#define TPARAM3			(TPARAM1 + 40)						// top edge of second row of parameters
#define TPARAM4			(TPARAM1 + 55)						// top edge of second row of parameters
#define LPARAM_P		(X_VERT_LINE + 6)					// left edge for Px and Py parameters
#define LPARAM_I		(X_VERT_LINE + 60)					// left edge for Ix and Iy parameters
#define LPARAM_D		(X_VERT_LINE + 115)					// left edge for Dx and Dy parameters
#define LPARAM_UMAX		(X_VERT_LINE + 6)					// left edge for UX_SATMAX and UX_SATMAY parameters
#define LPARAM_SLP		(X_VERT_LINE + 96)					// left edge for SATX_SLP and SATY_SLP parameters

// define indicators about select parameter
#define FS_f 		0
#define P_f 		1
#define I_f 		2
#define D_f 		3
#define UMAX_f 		4
#define SLP_f 		5
//-----------------------PLOT_SHOT CONSTANT------------------------------------------
#define WPLOT_SHOT			161									// width (is equal to height) of rect plot_shot
#define C_PLOT_SHOT_BP		((WPLOT_SHOT-1)/2)					// coordinate centre x(=y) of plot shot (bitmap frame)
#define LEDGE_PLOT_SHOT		(X_VERT_LINE + 6)					// left edge rect plot_shot
#define TEDGE_PLOT_SHOT		600									// top edge rect plot_shot
#define REDGE_PLOT_SHOT		(LEDGE_PLOT_SHOT + WPLOT_SHOT)		// right edge rect plot_shot
#define BEDGE_PLOT_SHOT		(TEDGE_PLOT_SHOT + WPLOT_SHOT)		// bottom edge rect plot_shot
#define CXPLOT_SHOT 		(LEDGE_PLOT_SHOT + ((WPLOT_SHOT-1)/2))// coordinate x of the centre of plot_shot (screen frame)
#define CYPLOT_SHOT 		(TEDGE_PLOT_SHOT + ((WPLOT_SHOT-1)/2))// coordinate y of the centre of plot_shot (screen frame)
//-----------------------USER INFORMATION CONSTANT--------------------------------------
#define LCOM_TEXT1			10			// left edge command (1,1) text
#define TCOM_TEXT1			8			// top edge command (1,1) text
#define TCOM_TEXT2			28			// top edge command (2,1) text
#define TCOM_TEXT3			48			// top edge command (3,1) text
#define TCOM_TEXT4			68			// top edge command (4,1) text
#define TCOM_TEXT5			88			// top edge command (5,1) text
#define TCOM_TEXT5			88			// top edge command (5,1) text

#define LMODE_TEXT1 		290 				// left edge mode (2,1) text
#define TMODE_TEXT1			(TCOM_TEXT1)		// top edge mode (2,1) text
#define LMODE_TEXT1L		(LMODE_TEXT2 + 110)	// top edge (2,1) low speed text
#define LMODE_TEXT1M		(LMODE_TEXT2 + 148)	// top edge (2,1) medium speed text
#define LMODE_TEXT1H		(LMODE_TEXT2 + 208)	// top edge (2,1) high speed text

#define LMODE_TEXT2 		(LMODE_TEXT1) 		// left edge mode (2,2) text
#define TMODE_TEXT2			(TCOM_TEXT2)		// top edge mode (2,2) text
#define LDESCRIP_TEXT2		(LMODE_TEXT2-20)	// left edge description (2,2) text
#define TDESCRIP_TEXT2		(TCOM_TEXT2+10)		// top edge description (2,2) text

#define LMODE_TEXT3 		(LMODE_TEXT1)		// left edge mode (3,2) text
#define TMODE_TEXT3 		(TCOM_TEXT3+6)		// top edge mode (3,2) text

#define LMODE_TEXT4			(LMODE_TEXT1)		// left edge mode (4,2) text
#define TMODE_TEXT4			(TCOM_TEXT4+6)		// top edge mode (4,2) text
#define LDESCRIP_TEXT4		(LMODE_TEXT2-20)	// left edge description (4,2) text
#define TDESCRIP_TEXT4		(TMODE_TEXT4+10)		// top edge description (4,2) text

#define LCOM_TEXT2 			855				// left edge command keys in auto-shot mode 			
#define TCOM_TEXT6 			570				// top edge command keys in auto-shot mode
#define LCOM_TEXT3 			855				// left edge descript in auto-shot mode 			
#define TCOM_TEXT7 			580				// top edge descript keys in auto-shot mode
#define LCOM_TEXT4 			855				// left edge descript in auto-shot mode 	
#define TCOM_TEXT8 			590				// top edge descript keys in auto-shot mode

//-----------------------USER INFORMATION CONSTANT-----------------------------------
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
//---------------------WORLD CONSTANT------------------------------------
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
#define ELECTRIC_BLUE 	32735
#define GREEN 			2016
#define DARK_GREEN		800
#define ELECTRIC_GREEN 	53216
#define LIGHT_PINK	 	64319
#define LIGHT_GREEN 	38863
#define ELECTRIC_PURPLE	47135
#define NEON_CARROT 	64710
#define BURNT_ORANGE 	51872
#define LIGHT_CYAN 		45055
#define LIGHT_RED 		64041
#define LIGHT_MAGENTA 	58460
#define YELLOW 			65504
#define WHITE 			65535

#define DIM_COLORS4SKEET_N 	9
const unsigned int COLORS4SKEET_N[DIM_COLORS4SKEET_N] = { GREEN, LIGHT_PINK, ELECTRIC_PURPLE, NEON_CARROT, // skeet colors night
														RED, LIGHT_RED ,LIGHT_MAGENTA  ,YELLOW ,WHITE};
#define DIM_COLORS4SKEET_D 	8												
const unsigned int COLORS4SKEET_D[DIM_COLORS4SKEET_D] = {RED, BROWN, BLUE, NEON_CARROT, DARK_GREEN, ELECTRIC_PURPLE, // skeet colors day
														 BLACK, BURNT_ORANGE};
//----------------------------------------------------------------------------
// TASK CONSTANTS
//----------------------------------------------------------------------------
#define N_MAX_TASK			10		// max number of tasks (in accord to library)

#define IDX_COM_INTERP		3		// task index of command interpreter "com_interp_task"
#define PRIO_COM_INTERP		3		// priority values
#define PER_COM_INTERP 		30		// task period [ms]
#define DREL_COM_INTERP 	30		// relative deadline [ms]

#define IDX_DISPLAY			4		// task index of "display_task"
#define PRIO_DISPLAY		4
#define PER_DISPLAY 		20
#define DREL_DISPLAY 		20

#define IDX_SKEET_TASK		1		// task index for skeet_task
#define PRIO_SKEET_TASK		6		// la priorità è la più alta anche per un discorso di variabili globali
#define PER_SKEET_TASK 		20		// il periodo deve essere più piccolo del display altrimenti il task del display si trova
#define DREL_SKEET_TASK 	20		// con lo stesso dato consecutivamente

#define IDX_EXPLOSION		5		// task index of "display_task"
#define PRIO_EXPLOSION		6		// try to put the same priority
#define PER_EXPLOSION		20
#define DREL_EXPLOSION		20

#define IDX_FLICKER_AIM		7
#define PRIO_FLICKER_AIM	2
#define PER_FLICKER_AIM		30
#define DREL_FLICKER_AIM	30

#define IDX_RECOIL			6
#define PRIO_RECOIL			2
#define PER_RECOIL			30
#define DREL_RECOIL			30

#define IDX_AUTOM			8
#define PRIO_AUTOM			5
#define PER_AUTOM			20
#define DREL_AUTOM			20
//----------------------------------------------------------------------------
// SKEET CONSTANTS
//----------------------------------------------------------------------------
#define G 					9.81
#define RYMIN 				12		//10
#define RYMAX 				20		//20
#define DIFF_MIN			0		// minimum difference between semi-major axis and semi-mino axis
#define DIFF_MAX			6		// maximum difference between semi-major axis and semi-mino axis
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
#define VZMIN				2 
#define VZMAX 				15
#define ALPHA_REDUCE_RAD	650		// proportional constant that reduce skeet radius as skeet.z increses
#define MIN_RY 				6 		// minimu reducing of the minor semi-axis of skeet
#define TSCALE_SKEET		0.87	// time scale factor for skeet motion

//------------------------------------------------------------------------
const long SEC_WAIT_MIN	=	1;				// time values for task suspension [s]
const long SEC_WAIT_MAX	=	4;				// [s]
const time_t NSEC_WAIT_MIN= 0;				// [ns]
const time_t NSEC_WAIT_MAX= 999999999;		// [ns]

//-------------------------------------------------------------------------
#define ALPHA_FUT 			650	 	// proportional constant for computing the future position of skeet

//----------------------------------------------------------------------------
// EXPLOSION CONSTANTS
//----------------------------------------------------------------------------
#define MAX_TYPES_DEBRIS_SMALL	10		// number of different types of small STEP_GEN_SMALL
#define N_DEBRIS 				100		// number of possible small debris generated by the explosion
#define SIZE_POLYG_SMALL 		10		// size of each array for debris small
#define W_DEB_SMALL_BP 			5			// size of debris BITMAP squared
#define STEP_GEN_SMALL 			3		// step to generate small debris along Rect generator
#define ALPHA_NORM 				4		// costant to link vx and vy to xc and yc		
#define TSCALE_DEBRIS			0.8		// time scale factor for debris motion
#define ADD_RAND_V 				20 		// add a random component on velocity [-ADD_RAND_V,+ADD_RAND_V]
#define ALPHA_RAD_BOUND 		1.5		// scale factor to boundary radius of debris (for hide them)
#define MAX_RAND_BOUND			10 		// add a random component on radius boundary [0, MAX_RAND_BOUND]
#define ALPHA_REDUCE_NDEB		1.5 	// proportional constant to reduce number of debris if skeet dimension are reduced

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
// AIM_FLICKERING CONSTANTS
//----------------------------------------------------------------------------
#define BOUND_FLICK				5					// radius boundary of the instability
#define PI 						3.14 
#define TSCALE_FLICK 			2.5					// time scale factor for aim flickering motion (like a fly)
#define ANGLE_FLICK_DEV 		20 					// delta angle
#define V_FLICK					6					// flickering speed
//----------------------------------------------------------------------------
// RECOIL_TASK CONSTANTS
//----------------------------------------------------------------------------
#define MAX_ITER_UP 			5	 	// number of iterations and max iterations of recoil_task
#define DISPLACE_UP 		 	10		// number of pixel to move towards up

//----------------------------------------------------------------------------
// SHOTGUN CONSTANTS
//----------------------------------------------------------------------------
#define SHOTGUN_FRAME_B 		58		// height of the bottom frame (black) of each shotgun sprite
#define ALPHAX 					2					// proportional constant to link aim x-coordinate with shotgun x-coordinate
#define ALPHAY 					7					// proportional constant to link aim y-coordinate with shotgun y-coordinate
#define BLIM_SHOTGUN 			380					// bottom limit of y-coordinate of the aim (in aim frame)
const int MAX_Y_SHOTGUN	= (BLIM_SHOTGUN/ALPHAY);	// constant limit of y-coordinate of the shotgun, if it reaches BLIM_SHOTGUN

//----------------------------------------------------------------------------
// AUTOMATIC-SHOTGUN CONSTANTS
//----------------------------------------------------------------------------
#define X0_AUT 					(WWORLD/2)			// initial position of the automatic gun,  that robot must initially reach
#define Y0_AUT 					120				// (half of width of aim-BITMAP)

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
		float 	ry;		// semi-minor axis along y of ellipsoide [m]
		float 	rx;		// semi-major axis along x of ellipsoide [m]
		float 	x;		// x coordinate [m]
		float 	y;		// y coordinate [m]
		float 	z; 		// z coordinate [m] (it is a fictitious variable, used only to reduce dimension of skeet)
		float 	vx;		// horizontal velocity [m/s]
		float 	vy;		// vertical velocity [m/s]
		float 	vz; 	// velocity along z-axis
		int 	side_launch; 	// integer that informs about the side of the screen where skeet is launched (left:0, right:1)

		float 	x_last4expl;	// center of the future explosion
		float 	y_last4expl;

		int 	tot_dl_m;		// variable that remindes last deadline missed before clear the i-th task
};

struct state skeet;	// skeet state array

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
struct timespec delta; 		// create an array of struct for the delay launch of each skeet

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
//	STRUCTURE FOR COLORS (structure not shared among task)
//----------------------------------------------------------------------------
struct rgb_hsv{
				int r, g, b;		// rgb values of color of user message (initialized as BLACK)
				float h, s, v;		// HSV of color of user message (initialized as BLACK)
};

//----------------------------------------------------------------------------
// GLOBAL VARIABLES
//----------------------------------------------------------------------------
int 	skeet_alive_f = 0;		// flag to describe the life-state of the skeet
int 	skeet_awake_f = 0;		// flag to inform when skeet_task wakes up from clock_nanosleep
int 	skeet_explosion_f = 0;	// flag to inform if skeet is hit
int 	shot_f = 0; 			// flag is set to one if user clicks space	
int 	end = 0;				// end flag

int 	anticip_shot_f = 0; 	// flag for mode: anticipate shot (default:0)
int 	handle_anticip_f = 0; 	// flag to say to skeet_task that skeet is hit but continue to draw it
int 	debris_small_gen;

int 	expl_tot_dl_m = 0;		// variable that remindes last deadline missed before clear

int 	recoil_tot_dl_m = 0; 	// variable that remindes last deadline missed before clear

int 	vx_diffic_select = 1;		// difficulty mode select (0: easy, 1: medium [default,] 2: hard)

int 	flash_f = 0; 				// flag for auto-terminate text flash
int 	msg_user_new = 0;			// last message to inform user about something (int the flash text) (0 means no message)
int 	msg_user_prev = 0;			// previous message to inform user about something (int the flash text)

int 	reload_seq_f = 0;			// flag to comunicate to draw_task to draw reloading sequence
int 	shot_seq_f = 0; 			// flag to comunicate to draw_skeet to draw shot sequence

int 	night_f = 0; 				// flag to memorize night mode (0 => sunny, 1 => night)

int 	flick_f = 0;				// flag to memorize flickering mode	(0 => off, 1 => on)		
int 	flick_tot_dl_m = 0;			// variable that remindes last deadline missed before clear

//-------------------------------AUTOMATIC GUN MODE---------------------------------------------
int 	targx_init_f = 0;	 		// flag to inform if aim has reached initial target position X0_AUT
int 	targy_init_f = 0;	 		// flag to inform if aim has reached initial target position Y0_AUT
int 	autom_f = 0; 				// flag to memorize automatic gun mode (0 => off, 1 => on)
int 	targx_f = 0;	 			// flag to inform if aim has reached target position 
int 	targy_f = 0;	 			// flag to inform if aim has reached target position 
int 	autom_tot_dl_m = 0;			// variable that remindes last deadline missed before clear
float 	usat_x = 0; 					// value of dynamic saturation (it will be increas linearly simulating the motor inertia)
float 	usat_y = 0; 					// value of dynamic saturation (it will be increas linearly simulating the motor inertia)
float 	ex_prev = 0;				// previous error for Derivator
float 	ey_prev = 0;
int 	t_pltx = LHORIZ_UX; 		// x coordinate where pixel will be draw in motor plotx (screen-frame) 		
int 	t_plty = LHORIZ_UY; 		// x coordinate where pixel will be draw in motor ploty (screen-frame)	
int 	vert_sel_f = 0; 			// flag selection of x (1), y (-1) or both xy (0)
int 	oriz_sel_f = 5; 			// switch from all possible editable prameters

float 	FS_x 		=     8; 						// scale factor to draw ux uy u_satx and u_saty
float 	FS_y 		=     8; 						// scale factor to draw ux uy u_satx and u_saty
float 	P_x 		= 	2; 		//2				// proportional constant of control on x position
float	P_y 		= 	2;		 //2				// proportional constant of control on y position
float 	I_x 		= 	0;					// integravite constant of control on x position
float 	I_y 		= 	0; 					// integrative constant of control on y position
float 	D_x 		= 	0.8;	//0.82			// derivative action
float 	D_y 		= 	0.8;	//0.82			// derivative action
float 	UX_SATMAX 	= 	6; 					// max shift per iteration of robot gun [pixel/iteration]
float 	UY_SATMAX 	= 	10; 					// max shift per iteration of robot gun [pixel/iteration]
float 	SATX_SLP 	= 	9.5;				// ramp slope of speed profile (it is the acceleration)
float 	SATY_SLP 	= 	9.5;				// ramp slope of speed profile (it is the acceleration)

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

// initialize the semaphore to synhronize skeet_explosion_f with the compute of debris physics
sem_t 	sem_sync_expl;

//----------------------------------------------------------------------------
// FUNCTIONS DECLARATION: possiamo dividerli tra ausiliarie e task
//----------------------------------------------------------------------------

void 	init();

void* 	com_interp_task(void* arg);

void*	display_task(void* arg);

void* 	explosion_task(void* arg);

void*	skeet_task(void* arg);

void*	aim_flickering(void* arg);

void*	recoil_task(void* arg);

void*	autom_shotgun(void* arg);

void 	init_skeet();

void 	sleep_skeet_task();

void 	display_plot_shot(int dist_x, int dist_y);

void 	handle_shot(int skeet_x, int skeet_y, int rx, int ry, float dt);

void 	handle_anticip_fun(int x, int side_launch_local);

void 	handle_out_of_screen(int x, int r, int side_launch_local);

int 	irand(int min, int max);

float 	frand(float min, float max);

long 	lrand(long min, long max);

time_t 	trand(time_t min, time_t max);

int 	pre_handle_deb_hid(int ry, int n_hide_deb);

int 	handle_deb_hide(int i, int n_hide_deb);

int 	init_explosion(int ry, unsigned int color_skeet);

void 	draw_skeet(int i, BITMAP* world_copy, BITMAP* skeet_image);

void 	draw_aim(BITMAP* world_copy);

void 	draw_explosion(int i, BITMAP* world_copy, BITMAP* explosion_copy);

void 	draw_reload_seq(BITMAP *world_copy, BITMAP *reload_seqBP[], int x, int y, int rel_seq_idx);

void 	draw_shot_seq(BITMAP *world_copy, BITMAP *shot_seqBP[], int x, int y, int shot_seq_idx);

void 	draw_shotgun(BITMAP* world_copy, BITMAP *shotgun_img, int x0, int y0);

void 	change_value_col(struct rgb_hsv *msg, int *prev_col_f, int *iter, unsigned int *col_msg);

void 	handle_msg_user(struct rgb_hsv *msg, int *iter, unsigned int col_msg);

void 	handle_flick_bounce(float *x, float *y, float *a);

void 	plot_ux_usatx(float ux, float usat_x);

int 	control_aim_x(int ref_x, int targx_init_f);

void 	plot_uy_usaty(float uy, float usat_y);

int 	control_aim_y(int ref_y, int targy_init_f);

void 	move_arrow_keys();

void	handle_vert_FS();

void 	handle_vert_P();

void 	handle_vert_I();

void 	handle_vert_D();

void 	handle_vert_UMAX();

void 	handle_vert_SLP();

void 	edit_autom_param(float oper);

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

	// plot previous shot results
	rect(screen, LEDGE_PLOT_SHOT, TEDGE_PLOT_SHOT, REDGE_PLOT_SHOT, BEDGE_PLOT_SHOT, ELECTRIC_BLUE);

	// initialize plot axis motor ux input (autom_shotgun)
	rect(screen, LHORIZ_UX, THORIZ_UX, RHORIZ_UX, THORIZ_UX, WHITE);		// horizontal ux axis (time)
	rect(screen, LHORIZ_UY, THORIZ_UY, RHORIZ_UY, THORIZ_UY, WHITE);		// horizontal uy axis (time)

	// initialize info about ux and uy
	sprintf(s, "ux:%+4.1f", 0.0); 			
	textout_ex(screen, font, s, LINFO_UX, TINFO_UX, WHITE, BKG);	// text of monitor first line

	sprintf(s, "uy:%+4.1f", 0.0); 			
	textout_ex(screen, font, s, LINFO_UY, TINFO_UY, WHITE, BKG);	// text of monitor first line

	sprintf(s, "FS_x:%2.1f", FS_x); 			
	textout_ex(screen, font, s, LPARAM_FSX, TPARAM_FSX, WHITE, BKG);	// text of monitor first line

	sprintf(s, "FS_y:%2.1f", FS_y); 			
	textout_ex(screen, font, s, LPARAM_FSY, TPARAM_FSY, WHITE, BKG);	// text of monitor first line

	// initialize parameters editables
	sprintf(s, "Px:%2.f", P_x); 			
	textout_ex(screen, font, s, LPARAM_P, TPARAM1, WHITE, BKG);	// text of monitor first line

	sprintf(s, "Py:%2.f", P_y); 			
	textout_ex(screen, font, s, LPARAM_P, TPARAM2, WHITE, BKG);	// text of monitor first line

	sprintf(s, "Ix:%2.1f", I_x); 			
	textout_ex(screen, font, s, LPARAM_I, TPARAM1, WHITE, BKG);	// text of monitor first line

	sprintf(s, "Iy:%2.1f", I_y); 			
	textout_ex(screen, font, s, LPARAM_I, TPARAM2, WHITE, BKG);	// text of monitor first line

	sprintf(s, "Dx:%2.1f", D_x); 			
	textout_ex(screen, font, s, LPARAM_D, TPARAM1, WHITE, BKG);	// text of monitor first line

	sprintf(s, "Dy:%2.1f", D_y); 			
	textout_ex(screen, font, s, LPARAM_D, TPARAM2, WHITE, BKG);	// text of monitor first line

	sprintf(s, "vx_max:%2.f", UX_SATMAX); 			
	textout_ex(screen, font, s, LPARAM_UMAX, TPARAM3, WHITE, BKG);	// text of monitor first line

	sprintf(s, "vy_max:%2.f", UY_SATMAX); 			
	textout_ex(screen, font, s, LPARAM_UMAX, TPARAM4, WHITE, BKG);	// text of monitor first line

	sprintf(s, "ax:%2.1f", SATX_SLP); 			
	textout_ex(screen, font, s, LPARAM_SLP, TPARAM3, WHITE, DARK_GREEN);	// text of monitor first line

	sprintf(s, "ay:%2.1f", SATY_SLP); 			
	textout_ex(screen, font, s, LPARAM_SLP, TPARAM4, WHITE, DARK_GREEN);	// text of monitor first line

	//----------------------------------------------------------------------------
	// INITIALIZE TEXT
	//----------------------------------------------------------------------------
	// task deadlines missed
	rect(screen, LEDGE_RECT_DMISS, TEDGE_RECT_DMISS, REDGE_RECT_DMISS, BEDGE_RECT_DMISS, RED);
	textout_ex(screen, font, "DEADLINES MISSED", LEDGE_MON_TITLE, TEDGE_MON_TITLE, RED, BKG);	// text of monitor title deadline

	skeet.tot_dl_m = 0;		// initialize che field of deadlines missed

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

	sprintf(s, "aim_flickering:%2u", 0);
	textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT6, WHITE, BKG);	// text of monitor sixth line

	sprintf(s, "autom_shotgun:%2u", 0);
	textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT7, WHITE, BKG);	// text of monitor seventh line

	// helper key command avaibles (top left)
	textout_ex(screen, font, "ARROW KEYS: move the aim", LCOM_TEXT1, TCOM_TEXT1, YELLOW, BKG);
	textout_ex(screen, font, "SPACE KEY: shoot", LCOM_TEXT1, TCOM_TEXT2, YELLOW, BKG);
	textout_ex(screen, font, "C KEY: launch one skeet", LCOM_TEXT1, TCOM_TEXT3, YELLOW, BKG);
	textout_ex(screen, font, "S / A KEY: + / - speed skeet", LCOM_TEXT1, TCOM_TEXT4, YELLOW, BKG);
	textout_ex(screen, font, "Z KEY: change background", LCOM_TEXT1, TCOM_TEXT5, YELLOW, BKG);

	// helper key command avaibles (bottom right)
	textout_ex(screen, font, "ARROW and TAB KEYS:", LCOM_TEXT2, TCOM_TEXT6, YELLOW, BKG);			
	textout_ex(screen, font, "edit parameters", LCOM_TEXT3, TCOM_TEXT7, WHITE, BKG);			
	textout_ex(screen, font, "[auto-shot mode]", LCOM_TEXT4, TCOM_TEXT8, WHITE, BKG);			

	// helper mode avaibles
	textout_ex(screen, font, "Skeet speed:", LMODE_TEXT1, TMODE_TEXT1, YELLOW, BKG);
	textout_ex(screen, font, "Low", LMODE_TEXT1L, TMODE_TEXT1, LIGHT_GRAY, BKG);
	textout_ex(screen, font, "Medium", LMODE_TEXT1M, TMODE_TEXT1, YELLOW, BKG);
	textout_ex(screen, font, "High", LMODE_TEXT1H, TMODE_TEXT1, LIGHT_GRAY, BKG);
	textout_ex(screen, font, "X KEY: 'anticipate-shot' mode OFF", LMODE_TEXT2, TMODE_TEXT2, LIGHT_GRAY, BKG);
	textout_ex(screen, font, "D KEY: 'flickering_aim' mode OFF", LMODE_TEXT3, TMODE_TEXT3, LIGHT_GRAY, BKG);
	textout_ex(screen, font, "F KEY: 'auto-shot mode OFF", LMODE_TEXT4, TMODE_TEXT4, LIGHT_GRAY, BKG);

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
void init_skeet()
{
int j; 	// index for COLORS4SKEET
float vx_extract; 	// local variable of vx

	// choose color of the skeet randomly according to night/sunny background
	if (night_f)
		j = irand(0, DIM_COLORS4SKEET_N-1);			// choose random color (night set)
	else if (!night_f)
		j = irand(0, DIM_COLORS4SKEET_D-1);			// choose random color (sunny set)

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
	skeet.side_launch = irand(0,1);
	if (night_f)
		skeet.c = COLORS4SKEET_N[j];			// color of the i-th skeet
	else if (!night_f)
		skeet.c = COLORS4SKEET_D[j];			// color of the i-th skeet

	skeet.ry = frand(RYMIN, RYMAX);			// minor semi-axis of the i-th skeet
	skeet.rx = skeet.ry + frand(DIFF_MIN, DIFF_MAX); 	// x-semiaxis will be always bigger than y-semiaxis (ellipse)

	if (skeet.side_launch == 0)		// if skeet comes from left
	{
		skeet.x = -skeet.rx - 2.0; 				// -2 is for the edge
		skeet.y = frand(HMIN, HMAX);
		skeet.z = 0;

		// assign vx extract
		skeet.vx = vx_extract;
		// vy range is fixed
		skeet.vy = frand(VYMIN, VYMAX);
		skeet.vz = frand(VZMIN, VZMAX);		
	}

	if (skeet.side_launch == 1)		// if skeet comes from right
	{
		skeet.x = skeet.rx + XRWORLD; 				
		skeet.y = frand(HMIN, HMAX);
		skeet.z = 0;

		// assign vx extract
		skeet.vx = -vx_extract;

		// vy range is fixed
		skeet.vy = frand(VYMIN, VYMAX);
		skeet.vz = frand(VZMIN, VZMAX);		
	}

	pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state

	// skeet.v0 = sqrt(...);

	
}

//--------------------------------------------------------------------------------
// SLEEP_SKEET_TASK: sleep task of a casual time skeet before begin the true task
//--------------------------------------------------------------------------------
// after that all parameters of the task are initialized task will be suspended until the CLOCK_MONOTONIC reaches the time specified
void 	sleep_skeet_task()
{
	// struct delta is global but it is not share among other tasks so tasks are not required
	delta.tv_sec = trand(SEC_WAIT_MIN, SEC_WAIT_MAX); 	// type of the field "tv_sec" is time_t (a long int of 32 bit)
	printf("%ld\n", delta.tv_sec);

	delta.tv_nsec = lrand(NSEC_WAIT_MIN, NSEC_WAIT_MAX); 	// type of the field "tv_nsec" is a long int (32 bit)
	printf("%ld\n", delta.tv_nsec);

	clock_nanosleep(CLOCK_MONOTONIC, 0, &delta, NULL);
}

//-----------------------------------------------------------------------------------------------------
// DISPLAY_PLOT_SHOT
//-----------------------------------------------------------------------------------------------------
void 	display_plot_shot(int dist_x, int dist_y)
{
int col; 			// skeet col
float rx,ry;		// skeet semi-axis
BITMAP *plot_shot;

	pthread_mutex_lock(&mux_skeet_state);	 // wait on the mutex for skeet state
	col = skeet.c;
	rx = skeet.rx;
	ry = skeet.ry;
	pthread_mutex_unlock(&mux_skeet_state);	 // signal on the mutex for skeet state

	plot_shot = create_bitmap(WPLOT_SHOT-1, WPLOT_SHOT-1);
	if (!plot_shot)
	{
		printf("Error creating plot_shot bitmap\n");
		sem_post(&sem_sync_expl);		// unblock explosion_task
		exit(1);
	}
	clear_to_color(plot_shot, BKG);

	if (col == BKG) col = RED;
	ellipse(plot_shot, C_PLOT_SHOT_BP, C_PLOT_SHOT_BP, rx, ry, col);				// print border of skeet
	circlefill(plot_shot, C_PLOT_SHOT_BP+dist_x, C_PLOT_SHOT_BP-dist_y, 2, RED); 	// print where player shot
	blit(plot_shot, screen, 0, 0, CXPLOT_SHOT-C_PLOT_SHOT_BP+1, CYPLOT_SHOT-C_PLOT_SHOT_BP+1, plot_shot->w, plot_shot->h);
	destroy_bitmap(plot_shot);
}
//-----------------------------------------------------------------------------------------------------
// HANDLE_SHOT: compare global variables to see if the shot hit skeet and in case create explosion_task
//-----------------------------------------------------------------------------------------------------
void handle_shot(int xc, int yc, int rx, int ry, float dt) 	// xc and yc are the pixel coordinate of the centre of the skeet, r is its radius
{
int aim_x, aim_y;		// local variable to memorize aim coordinate
int dist_shot_wsq;		// square weighted of the distance of the aim respect to the skeet centre
int rx2ry2;			// multiplication of squared semi-axis
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

	rx2ry2 = (rx*rx)*(ry*ry); 	// compute area of skeet

	// if MODE ANTICIPATE SHOT: OFF => calculate the squared distance between the aim and the centre of the skeet
	if (!anticip_shot_f)		// if mode anticipate shot: OFF
	{
		dist_x = aim_x - xc;
		dist_y = aim_y - yc;
		dist_shot_wsq = (dist_x)*(dist_x)*(ry*ry) + (dist_y)*(dist_y)*(rx*rx);
	}

	// if MODE ANTICIPATE SHOT: ON => calculate the squared distance between the aim and the centre of the FURURE skeet
	if (anticip_shot_f)		// if mode anticipate shot: ON
	{
		//computes the future position of the skeet
		pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state
		vx = skeet.vx;
		vy = skeet.vy;
		pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state

		v_mod = sqrt(vx*vx + vy*vy);		// compute v module
		vx_n = vx/v_mod;					// compute vx versor
		vy_n = vy/v_mod;					// compute vy versor
		// compute future skeet centre: the smaller the radius the greater the distance and the future x,y center
		// [DISTANCE DEPENDS ONLY BY MINIMUM SEMI-AXIS (ALONG Y)]
		fut_xc = (xc + (int) ((float) ALPHA_FUT / (float) ry) * vx_n);	// ALPHA_FUT is a proportional constant
		fut_yc = (yc + (int) ((float) ALPHA_FUT / (float) ry) * vy_n);
		dist_x = aim_x - fut_xc;
		dist_y = aim_y - fut_yc;

		// printf("xc = %d, yc = %d\n fut_xc = %d, fut_yc = %d\n", xc, yc, fut_xc, fut_yc);
		dist_shot_wsq = (dist_x)*(dist_x)*(ry*ry) + (dist_y)*(dist_y)*(rx*rx);

		// in any case (hit or missed) memorize fut_xc and fut_yc in the structure
		pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state
		skeet.x_last4expl = fut_xc;
		skeet.y_last4expl = fut_yc;
		pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state
	}	


	// if SKEET HIT!!!
	if (dist_shot_wsq <= rx2ry2)
	{
		textout_ex(screen, font, "SKEET HIT!   ", LINFO_STATE_SKEET, TINFO_TEXT_SKEET, YELLOW, BKG);
		if (!anticip_shot_f)		// if mode anticipate shot: OFF
		{
			// it ends skeet_task and draw_skeet (display_task)
			skeet_awake_f = 0;

			// memorize last position of the skeet to bring information to explosion_draw
			pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state
			skeet.x_last4expl = skeet.x;
			skeet.y_last4expl = skeet.y;
			pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state

			// it say to explosion_task to start updating debris position
			skeet_explosion_f = 1;

			sem_post(&sem_sync_expl);					// UNLOCK thread explosion_task

			// plot shot result 
			display_plot_shot(dist_x, dist_y);
		}
		if (anticip_shot_f)		// if mode anticipate shot: ON
		{
			display_plot_shot(dist_x, dist_y);
			// continue skeet_task until explosion begin: for doing this activate a flag
			handle_anticip_f = 1;
			// memorize the position of the future explosion of the skeet
		}		
	}
	else
	{
		display_plot_shot(dist_x, dist_y);
		flash_f = 1;
		msg_user_new = MSG_USER_MISS;
	}
}

//------------------------------------------------------------------------------
// HANDLE_ANTICIP_FUN: wait until "skeet reaches the bullet" to  start explosion
//------------------------------------------------------------------------------
void handle_anticip_fun(int x, int side_launch_local)
{
	// if skeet is beyond coordinate x where it is supposed to explose (whether it starts from the left or from the right screen side)
	if( ( (x > skeet.x_last4expl) && (side_launch_local == 0) ) || ( (x < skeet.x_last4expl) && (side_launch_local == 1) ) )
	{
		// it ends skeet_task and draw_skeet (display_task)
		skeet_awake_f = 0;

		// it say to explosion_task to start updating debris position
		skeet_explosion_f = 1;

		sem_post(&sem_sync_expl);					// UNLOCK thread explosion_task

		handle_anticip_f = 0; 		// reset flag!!
	}
	// else continue to compute skeet trajectory
}

//-----------------------------------------------------------------------------------------------
// HANDLE_OUT_OF_SCREEN: if skeet is out of the screen reset flags and prepare for the new launch
//-----------------------------------------------------------------------------------------------
void handle_out_of_screen(int x, int r, int side_launch_local)
{
	if ( ( (x-r >= XRWORLD) && (side_launch_local == 0) ) || ( (x+r <= XLWORLD) && (side_launch_local == 1) ) )
	{	
		skeet_alive_f = 0;

		skeet_awake_f = 0;

		textout_ex(screen, font, "READY TO LAUNCH", LINFO_STATE_SKEET, TINFO_TEXT_SKEET, ELECTRIC_GREEN, BKG);

		// don' do the explosion (REDONDANT)
		skeet_explosion_f = 0;
			
		sem_post(&sem_sync_expl);			// UNLOCK thread explosion_task
		}

	// restart position of automatic shotgun
	if( (autom_f) && ( ( (x + 6*r >= XRWORLD) && (side_launch_local == 0) ) || ( (x - 6*r <= XLWORLD) && (side_launch_local == 1) ) ) ) 
	{
		targx_init_f = 0;
		targy_init_f = 0;
	}

}

//----------------------------------------------------------------------------
// SKEET_TASK: compute the physic of the skeet/skeets 
//----------------------------------------------------------------------------
void* skeet_task(void* arg)
{
int i;								// task index
float dt;							// integration interval
int rx, ry, x, y;						// local variable to memorize global variables for the skeet (there is a CASTING!)
char s[30];							// string for monitor deadline messed
int side_launch_local;	// local variable to memorize global variables for the skeet

	i = get_task_index(arg);
	dt = TSCALE_SKEET * (float) get_task_period(arg) / 1000;

	// initialize skeet with IDX_SKEET_TASK
	init_skeet();

	// before the sleep of skeet_task, explosion_task starts its initialization
	task_create(explosion_task, IDX_EXPLOSION, PER_EXPLOSION, DREL_EXPLOSION, PRIO_EXPLOSION);

	// function that suspend skeet_task for a random time
	sleep_skeet_task(i);

	// when task awakes up set flags and begin the true task
	skeet_awake_f = 1;
	textout_ex(screen, font, "UP IN THE AIR  ", LINFO_STATE_SKEET, TINFO_TEXT_SKEET, ELECTRIC_BLUE, BKG);

	set_activation(i);

	while(!end && skeet_awake_f)	// end is set to 0, also skeet_alive_f until task deads
	{
		pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state

		// update skeet motion
		skeet.vy -= G * dt;
		skeet.x += skeet.vx * dt;
		skeet.y += skeet.vy * dt - G * dt * dt / 2;
		skeet.z += skeet.vz * dt;
		// reduce elipse semi-axis
		if(skeet.ry >= MIN_RY){					// prevents to degenerate the ellipse to a line
		skeet.rx -= skeet.z/ALPHA_REDUCE_RAD; 
		skeet.ry -= skeet.z/ALPHA_REDUCE_RAD; 
		}

		// printf("%f\n", skeet.y);
		x = (int) skeet.x;
		y = (int) skeet.y;
		rx = (int) skeet.rx;
		ry = (int) skeet.ry;
		side_launch_local = skeet.side_launch;	// memorize global variables
		pthread_mutex_unlock(&mux_skeet_state);			// signal on the mutex for skeet state

		// monitoring the istance when the user hit skeet in anticipate mode ON, until skeet explodes
		if (handle_anticip_f)							// f stand for FLAG	
			handle_anticip_fun(x, side_launch_local);	// fun stands for FUNCTION	

		// if skeet is out of the screen and it came from LEFT or from RIGHT, clear the skeet task when it disappears
		handle_out_of_screen(x, rx, side_launch_local);

		// if user click SKACE_KEY verify if the shoot hit skeet
		if (shot_f == 1){
			shot_f = 0;		// turn off shot flag

			handle_shot(x, y, rx, ry, dt);	// compute if player hits skeet
		}

		if (deadline_miss(i))
		{
			skeet.tot_dl_m ++;		// it is not possible to use param[i].dmiss because it clears every time the task is cleared
			sprintf(s, "skeet_task:%2u", skeet.tot_dl_m);
			textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT2, WHITE, BKG);	// text of monitor second line
		}

		wait_for_activation(i);

	}
	wait_for_task(i);	// when skeet is no more alive we must wait that the task is finished
	return 0;
}

//---------------------------------------------------------------------------------------------------------------
// INIT_EXPLOSION: initialize debris struct around the centre of the skeet, forming a square, with random parameters
//---------------------------------------------------------------------------------------------------------------
int init_explosion(int ry, unsigned int color_skeet)
{


	W_EXPL_BITMAP = 6*ry;		// it is a global varible but it does not need a mutex, because it is write once per skeet init
	explosion_copy = create_bitmap(W_EXPL_BITMAP, W_EXPL_BITMAP);		// create the explosion_copy BITMAP
	clear_to_color(explosion_copy, BKG);

int center = 3*ry;		// coordinate of the center of explosion_copy

// coordinate del rettangolo generatore di frammenti (è dipendente dal raggio)
int RAD_GEN = ry/3; 		// half side of the rect generator

// scriviamo le coordinate del quadrato magenta che genera gli skeet (frame della explosion BITMAP)
int rect_xl = center - RAD_GEN + 1;		// coordinate interne al rettangolo (i pixel che definiscono gli spigoli interni)
int rect_xr = center + RAD_GEN - 1;
int rect_yt = center - RAD_GEN + 1;
int rect_yb = center + RAD_GEN - 1;

										// (si lavora nel frame dell' explosion BITMAP)

int RAD_BOUND = ALPHA_RAD_BOUND*ry;			// bound for desappear debris and to end the task 

int deb_BP_xl; 					// coordinate x left of bitmap i-th generated on the side
int deb_BP_yt; 					// coordinate y top of bitmap i-th generated on the side
int deb_BP_xc; 					// coordinate x center of bitmap i-th generated along the side
int deb_BP_yc; 					// coordinate y center of bitmap i-th generated along the side

int dist_center_lt = (W_DEB_SMALL_BP-1)/2;	// distance from the center of the debris BITMAP to the left top pixel

int center2lt = center ; 	// measure to transform coordinates of the centre of debris bitmap to its left top coordinate

// GENERATION OF BITMAPS AROUND THE RECT GEN FOR EACH SIDE

int i; 							// index for cycle
int j;							// index for cycle

// initializing different types of brides 
int deb_type_small[MAX_TYPES_DEBRIS_SMALL][SIZE_POLYG_SMALL] = {

									{0,0,0,0, 0,4, 3,4, 3,3}, 		// triangle L 
									{0,0,0,0, 0,1, 1,1, 1,0},		// square 2x2
									{0,0,0,0, 0,3, 3,3, 3,0},		// square 3x3
									{0,1,0,3, 2,4, 2,1, 1,0},		// trapezio
									{0,0,0,0, 0,2, 1,2, 1,0},		// line 2x1
									{0,0,0,0, 0,4, 1,4, 1,0},		// line 3x1
									{0,0,0,2, 2,4, 3,4, 4,2},
									{0,2,0,3, 4,3, 4,2, 3,0},
									{0,0,0,4, 3,4, 3,3, 1,1},
									{0,1,0,1, 0,3, 1,3, 2,1} 	};

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

		// printf("xc: %f yc: %f\n", deb_small[i].xc, deb_small[i].yc);
		// printf("xbound: %d ybound: %d\n", deb_small[i].xbound, deb_small[i].ybound);

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

		// printf("vx: %3.1f vy: %3.1f\n", deb_small[i].vx, deb_small[i].vy);

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

		// printf("vx: %3.1f vy: %3.1f\n", deb_small[i].vx, deb_small[i].vy);

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

		// printf("vx: %3.1f vy: %3.1f\n", deb_small[i].vx, deb_small[i].vy);

		// initialize boundary coordinate for hidding debris when they are too far away (in centre of explosion_copy frame)
		deb_small[i].xbound =  RAD_BOUND - irand(0, MAX_RAND_BOUND);
		deb_small[i].ybound =  RAD_BOUND - irand(0, MAX_RAND_BOUND);

		deb_BP_yt -= STEP_GEN_SMALL;
		deb_BP_yc -= STEP_GEN_SMALL;

		i++;					// update the index for the generation of BITMAP
	}

	// all debbris are generated

	debris_small_gen = i;		// total number of generated debris
	// printf("%d\n", i);

	// memorize color of each debris
	for (i = 0; i < debris_small_gen; i++)
	{
		deb_small[i].col = color_skeet;		// all debris are initialized with the skeet color
		deb_small[i].alive_stat = 1; 		// all debris are initialized as alive
	}
	// clear_to_color(explosion_copy, LIGHT_GRAY);							// color the whole virtual screen for the graphic world of black
	// circlefill(explosion_copy, 3*r, 3*r, r, GREEN); //cerchio di raggio r
	// rect(explosion_copy, rect_xl, rect_yt, rect_xr, rect_yb, BLUE); // quadrato di lato r/3
return center2lt;
}

//----------------------------------------------------------------
// PRE_HANDLE_DEB_HID: hide some random debris in function of how much ry of ellipse is reduced, in this way if the task appears
//					   very far explosion_draw will generated a part of total debris previous computed
//----------------------------------------------------------------
int pre_handle_deb_hid(int ry, int n_hide_deb)
{
int new_ry;			// new minor semi_axis along y
int diff_ry; 		// difference from the initial (ry) and the new minor semi_axis along y (ry_new) at the explosion istance
int n_prehide_deb; 	// number of prehide debris 
int i;				// debris (from struct) index
int j; 				// prehide debris counter

	pthread_mutex_lock(&mux_skeet_state); 		// wait on the mutex for skeet state
	new_ry = (int) skeet.ry;
	pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state
	diff_ry = ry - new_ry;
	n_prehide_deb = (int) diff_ry*ALPHA_REDUCE_NDEB;
	// printf("prehide_deb are: %d\n", n_prehide_deb);
	for(j = 0; j < n_prehide_deb; j++)
	{
		i = irand(0, debris_small_gen); // choose random number from 0 to total number of generated debris
		deb_small[i].col = PINK;
		if (deb_small[i].alive_stat == 1)
		{
		deb_small[i].alive_stat = 0;
		n_hide_deb ++;		
		}
	}
return n_hide_deb; // update the number of hiddden debris
}

//----------------------------------------------------------------
// HANDLE_DEB_HIDE: if debris has crossen boundary make it black
//----------------------------------------------------------------
int handle_deb_hide(int i, int n_hide_deb)
{
	// if boundary sides are crossen vertically or horizontally and if debris is still visible
	if ( ( (abs(deb_small[i].xc) >= deb_small[i].xbound) || (abs(deb_small[i].yc) >= deb_small[i].ybound) ) && (deb_small[i].alive_stat) ){
		deb_small[i].col = PINK;
		deb_small[i].alive_stat = 0;
		n_hide_deb ++;
		// printf("%d\n", n_hide_deb);
	}
return n_hide_deb;
}

//--------------------------------------------------------------------------------
// EXPLOSION_TASK: compute the physics of each bride after the dead of task_skeet
//--------------------------------------------------------------------------------
void* 	explosion_task(void* arg)
{	
char s[30];					// string for monitor deadline messed
int idx_expl;				// task index						
int i;
int center2lt;				// measure to transform coordinates of the centre of debris bitmap to its left top coordinate		
float dt; 					// integration interval
int n_hide_deb = 0;			// number of the hidden (obscured) debris
unsigned int color_skeet; 		// skeet color code
int ry; 						// actual minor semi-axis along y

	pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state

	ry = (int) skeet.ry;		// memorize actual minor semi-axis along y
	color_skeet = skeet.c;		// memorize skeet color code

	pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state

	// init functions returns the number used on the frame transformation below from center to left top coordinate
	center2lt = init_explosion(ry, color_skeet);		

	dt = TSCALE_DEBRIS * (float) get_task_period(arg) / 1000;

	printf("LOCK EXPLOSION\n");
	sem_wait(&sem_sync_expl);		// block thread until user hit the skeet OR skeet disappears
	printf("UNLOCK EXPLOSION\n");

	// hide some random debris in function of how much ry of ellipse is reduced
	n_hide_deb = pre_handle_deb_hid(ry, n_hide_deb);

	idx_expl = get_task_index(arg);
	set_activation(idx_expl);

	while(!end && skeet_explosion_f) 		// if skeet is lost thread will be unlock but it will skip itself
	{													// otherwise task will terminate after all debris are hide

		pthread_mutex_lock(&mux_debris_small); 			// wait on the mutex for debris_small

		for(i = 0; i < debris_small_gen; i++)
		{	
			// update physic of debris
			deb_small[i].xc += deb_small[i].vx * dt;
			deb_small[i].yc += deb_small[i].vy * dt;

			// if one of the bounds is crossed and if bedris is still coloured colour that debris by black to hide it
			n_hide_deb = handle_deb_hide(i, n_hide_deb);

			// update left top coordinate of each debris bitmap to be draw by draw_task
			deb_small[i].xl  =  deb_small[i].xc + center2lt;
			deb_small[i].yt  = -deb_small[i].yc + center2lt;

			// printf(" i = %d => vx: %3.1f  vy: %3.1f\n", i, deb_small[i].vx, deb_small[i].vy);
		}
		pthread_mutex_unlock(&mux_debris_small); 			// wait on the mutex for debris_small

		// when all debris are hidden we can terminate explosion_task and draw_explosion, and user is free to ask for another skeet
		if(n_hide_deb >= debris_small_gen)
		{
			// terminate draw_explosion and explosion task
			skeet_explosion_f = 0;

			// user is able to ask another skeet
			skeet_alive_f = 0;
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
	draw_sprite(world_copy, aim_screen, x, y); // the background color (BKG = BLACK) is transparent to the skeets
}

//----------------------------------------------------------------------------
// DRAW_SKEET: draw skeet i in graphic coordinates 
//----------------------------------------------------------------------------
void 	draw_skeet(int i, BITMAP* world_copy, BITMAP* skeet_image)
{
unsigned int skeet_col; 					// random color
float x, y, rx, ry;			// local varibale for skeet physic
float xl, yt;			// left top  coordinate for skeet_image p
				// ALLA FINE DEL CODICE CONTROLLARE SE REALMENTE SERVONO 2 BITMAP
	
	pthread_mutex_lock(&mux_skeet_state);		// wait on the mutex for skeet state
		// printf("%f\n", skeet.y);
	x = skeet.x;
	y = skeet.y;
	rx = skeet.rx;
	ry = skeet.ry;
	skeet_col = skeet.c;

	pthread_mutex_unlock(&mux_skeet_state);		// signal on the mutex for skeet state 
	// NON È RIDONDANTE PERCHÈ POTREI DECIDERE DI RIDURRE COL TEMPO LE DIMENSIONI DEL CERCHIO!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	ellipsefill(skeet_image, (WSKEET-1)/2, (HSKEET-1)/2, rx, ry, skeet_col);	//copy on skeet_image virtual on the centre

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

	blit(explosion_copy, world_copy, 0, 0, skeet.x_last4expl - W_EXPL_BITMAP/2, -(skeet.y_last4expl - YBWORLD + YTWORLD + W_EXPL_BITMAP/2),
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
// CHANGE_VALUE_COL: update progressively value of color of user message text
//----------------------------------------------------------------------------
void change_value_col(struct rgb_hsv* msg, int *prev_col_f, int *iter, unsigned int *col_msg)
{
	rgb_to_hsv(msg->r, msg->g, msg->b, &msg->h, &msg->s, &msg->v);
	if(!*prev_col_f) 	msg->v += STEP_FLASH_TEXT;	// if previous color was black
	else 				msg->v -= STEP_FLASH_TEXT;	// if previous color was white
	if(msg->v >= 1){ 	// if color has reached the white
		msg->v = 1;
		*prev_col_f = 1;
	}
	if(msg->v <= 0){ 	// if color has reached the black
		msg->v = 0;
		*prev_col_f = 0;
		*iter += 1;
		if(*iter >= N_MAX_FLASH){
			*iter = 0;
			flash_f = 0; 	// terminate to flash text
		}
	}
	hsv_to_rgb(msg->h, msg->s, msg->v, &msg->r, &msg->g, &msg->b);
	*col_msg = makecol(msg->r, msg->g, msg->b);	
}

//----------------------------------------------------------------------------
// HANDLE_MSG_USER: textout the user message called by the rispective istance
//----------------------------------------------------------------------------
void handle_msg_user(struct rgb_hsv* msg, int* iter, unsigned int col_msg)
{
	switch (msg_user_new)
	{
		case MSG_USER_WAST_SHOT:	// wasted shot because skeet is not generated yet
			if (msg_user_new != msg_user_prev)
			{
				msg->r = 0; msg->g = 0; msg->b = 0; 	// restart color of the text and wait next task period
				textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
				*iter = 0; 							// restart number of flash cycles
				msg_user_prev = msg_user_new;
				break;
			}
			textout_ex(screen, font, "Shot wasted", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
			msg_user_prev = MSG_USER_WAST_SHOT;
		break;

		case MSG_USER_MISS:	//missed skeet message
			if (msg_user_new != msg_user_prev)
			{
				msg->r = 0; msg->g = 0; msg->b = 0; 	// restart color of the text and wait next task period
				textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
				*iter = 0; 							// restart number of flash cycles
				msg_user_prev = msg_user_new;
				break;
			}
			textout_ex(screen, font, "Ops, missed skeet", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
			msg_user_prev = MSG_USER_MISS;
		break;

		case MSG_USER_OOA: 	// out of ammo message
			if (msg_user_new != msg_user_prev)
			{
				msg->r = 0; msg->g = 0; msg->b = 0; 	// restart color of the text and wait next task period
				textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
				*iter = 0; 							// restart number of flash cycles
				msg_user_prev = msg_user_new;
				break;
			}
			textout_ex(screen, font, "Out of ammo!", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
			msg_user_prev = MSG_USER_OOA;
		break;

		case MSG_USER_WAIT_MODE: 	// wait for change mode
			if (msg_user_new != msg_user_prev)
			{
				msg->r = 0; msg->g = 0; msg->b = 0; 	// restart color of the text and wait next task period
				textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
				*iter = 0; 							// restart number of flash cycles
				msg_user_prev = msg_user_new;
				break;
			}
			textout_ex(screen, font, "can't change mode now", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
			msg_user_prev = MSG_USER_WAIT_MODE;
		break;

		case MSG_USER_WAIT_SPEED: 	// wait for change speed
			if (msg_user_new != msg_user_prev)
			{
				msg->r = 0; msg->g = 0; msg->b = 0; 	// restart color of the text and wait next task period
				textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
				*iter = 0; 							// restart number of flash cycles
				msg_user_prev = msg_user_new;
				break;
			}
			textout_ex(screen, font, "can't change speed now", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
			msg_user_prev = MSG_USER_WAIT_SPEED;
		break;

		case MSG_USER_OOS: 		// wait for next skeet, while previous is exploding
			if (msg_user_new != msg_user_prev)
			{
				msg->r = 0; msg->g = 0; msg->b = 0; 	// restart color of the text and wait next task period
				textout_ex(screen, font, "-------------------------------", L_MESSAGE_USER, T_MESSAGE_USER, BKG, BKG); // restart text
				*iter = 0; 							// restart number of flash cycles
				msg_user_prev = msg_user_new;
				break;
			}
			textout_ex(screen, font, "reloading next skeet...", L_MESSAGE_USER, T_MESSAGE_USER, col_msg, BKG);
			msg_user_prev = MSG_USER_OOS;
		break;

		case MSG_USER_KNR: 		// key not recognized
			if (msg_user_new != msg_user_prev)
			{
				msg->r = 0; msg->g = 0; msg->b = 0; 	// restart color of the text and wait next task period
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
int prev_col_f = 0;						// flag to inform wich user msg text color was (0:black, 1:white)
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
	clear_to_color(world_copy, PINK);

	// load background images
	BITMAP *background_night;
	background_night = load_bitmap("background_night.bmp", NULL);
	if (!background_night)
	{
		printf("Error loading background_night\n");
		sem_post(&sem_sync_expl);		// unblock explosion_task
		exit(1);
	}
	BITMAP *background_day;
	background_day = load_bitmap("background_day.bmp", NULL);
	if (!background_day)
	{
		printf("Error loading background_day\n");
		sem_post(&sem_sync_expl);		// unblock explosion_task
		exit(1);
	}

	// load bitmap for skeet
	BITMAP* skeet_image;
	skeet_image = create_bitmap(WSKEET, HSKEET); 	// create a bitmap with the resolution of the biggest skeet!
	if (!skeet_image)
	{
		printf("Error creating skeet_image BITMAP\n");
		sem_post(&sem_sync_expl);		// unblock explosion_task
		exit(1);
	}
	clear_to_color(skeet_image, PINK);				// color the whole virtual screen for the skeet of black

	// load shotgun static image
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

	// start with the day mode (default)
	blit(background_day, screen, 0, 0, XLWORLD, YTWORLD, background_day->w, background_day->h);

	i = get_task_index(arg);
	set_activation(i);

	while(!end)
	{
		if(!night_f)		// if night mode is set
			blit(background_day, screen, 0, 0, XLWORLD, YTWORLD, background_day->w, background_day->h);
		else if(night_f)	// if sunny mode is set
			blit(background_night, screen, 0, 0, XLWORLD, YTWORLD, background_night->w, background_night->h);

		clear_to_color(world_copy, PINK);				// color the whole virtual screen for the graphic world of black
		// STARTS TO DRAW FROM HERE

		// if skeet is launched and it is awake draw_skeet	
		if (skeet_awake_f)
		{
			clear_to_color(skeet_image, PINK);				// color the whole virtual screen for the skeet of black
			draw_skeet(IDX_SKEET_TASK, world_copy, skeet_image);
		}

		// check if skeet is hit to draw it from data provided by explosion task
		if (skeet_explosion_f)
		{
			clear_to_color(explosion_copy, PINK);								// clear the virtual screen "skeet_image"
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

		// draw point reference for auto-gun mode
		if(autom_f){
			circlefill(world_copy, X0_AUT, -(Y0_AUT -HWORLD), 4, ELECTRIC_PURPLE);
		}


		// copy the rectangular area from world_copy (virtual graphic world) to screen (real graphic world)
		draw_sprite(screen, world_copy, XLWORLD, YTWORLD);

		// DRAW GRAPHIC INFORMATIONS TO THE USER (on the right edge) (semaphores are not used because i want always update msg)

		if(flash_f)
		{
			// change progressively value (hsv) of text hsv (that starts from black and reach white)
			change_value_col(&msg, &prev_col_f, &iter, &col_msg);

			// handle msg user (policy: show last message)
			handle_msg_user(&msg, &iter, col_msg);
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
// HANDLE_FLICK_BOUNCE: handle virtual bounces of the aim displacement
//----------------------------------------------------------------------------
void handle_flick_bounce(float *x, float *y, float *a)
{
int outL, outR, outT, outB;

	outL = (*x <= -BOUND_FLICK); 	// flag for touch left side
	outR = (*x >=  BOUND_FLICK);		// flag for touch right side
	outT = (*y >=  BOUND_FLICK);		// flag for touch top side
	outB = (*y <= -BOUND_FLICK);		// flag for touch bottom side

	if (outL) *x = -BOUND_FLICK;
	if (outR) *x =  BOUND_FLICK;
	if (outL || outR) *a = PI - *a; 	// angle realignment

	if (outT) *y =  BOUND_FLICK;
	if (outB) *y = -BOUND_FLICK;
	if (outT || outB) *a = - *a; 		// angle realignment
}
//----------------------------------------------------------------------------
// AIM_FLICKERING: move randomly aim (like a fly) to make instable the aim
//----------------------------------------------------------------------------
void*	aim_flickering(void* arg)
{
int 	i;				// task index
char 	s[30];			// string for monitor deadline messed
float 	dt;				// integration interval
float 	vx, vy; 		// speed components along x and y axis	
float 	a = 0;			// orientation angle
float 	da; 			// delta angle
float 	x = 0; 					// virtual position of the aim along a rect of semi-side BOUND_FLICK around the aim
float 	y = 0; 					// initial position is the centre of this rect
float 	x_prev, y_prev; 		// previous value of virtual position
int 	delta_x, delta_y;		// difference from previous to new virtual position

	i = get_task_index(arg);
	dt = TSCALE_FLICK * (float) get_task_period(arg) / 1000;
	set_activation(i);

	while(!end  && flick_f)
	{

		x_prev = x;
		y_prev = y;

		da = frand(-ANGLE_FLICK_DEV, ANGLE_FLICK_DEV);		// delta alpha to add [deg]
		a += da*PI/180;										//  angle [rad]

		vx = V_FLICK * cos(a);		// compute speed components
		vy = V_FLICK * sin(a);

		x += vx*dt; 			// update virtual position
		y += vy*dt;

		handle_flick_bounce(&x, &y, &a); 	// handle virtual bounces

		delta_x = (int) x - x_prev;		// it needs a casting because aim.x works in pixel displacement
		delta_y = (int) y - y_prev;

		// pass the incremente/decrement respect to previous virtual position
		pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
		aim.x += delta_x;
		aim.y += delta_y;
		pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate

		if (deadline_miss(i))
		{
			flick_tot_dl_m++;
			sprintf(s, "aim_flickering:%2u", flick_tot_dl_m);
			textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT6, WHITE, BKG);	// text of monitor third line
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
int displace_rand;		// number of pixel to move in a random direction (left:0 or right:1)
int direct;					// direction choose by irand
int iter;
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

		
		if (aim.y >= YT_LIM_AIM) aim.y -= DISPLACE_UP;			// make sure that the aim don't go out of the screen

		pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate

		iter++;
		if(iter == MAX_ITER_UP) stop_f = 1;

		if (deadline_miss(i))
		{
			recoil_tot_dl_m++;
			sprintf(s, "recoil_task:%2u", recoil_tot_dl_m);
			textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT5, WHITE, BKG);	// text of monitor third line
		}
		wait_for_activation(i);
	}
	return 0;
}



//------------------------------------------------------------------------------------------------------
// 	PLOT_UX_USATX:  put pixels of ux and usat_x in the next black pixel, if plot is all drawed, cancel it
//------------------------------------------------------------------------------------------------------
void plot_ux_usatx(float ux, float usat_x)
{
char s[10];
	
	//print ux
	sprintf(s, "ux:%+4.1f", ux); 			
	textout_ex(screen, font, s, LINFO_UX, TINFO_UX, WHITE, BKG);	// text of monitor first line

	// transform physic variable into graphic variable (change sign + scaling + translate)
	ux = (-ux * FS_x) + THORIZ_UX;
	usat_x = (-usat_x * FS_x) + THORIZ_UX;

	// usat will coincide almost always with ux, so only in drawing it will shift from ux
	if ( (usat_x == ux) && (ux >= THORIZ_UX) ) 			// if ux is negative
	{
		usat_x += 4;
		if ( !(usat_x >= BPLOT_UX) )					// if usat_x cross limits don't draw
			putpixel(screen, t_pltx, usat_x, RED);
		if ( !(ux >= BPLOT_UX) )						// if ux cross limits don't draw
			putpixel(screen, t_pltx, ux, ELECTRIC_BLUE); // draw variables like points
	}

	else if ( (usat_x == ux) && (ux < THORIZ_UX) )		// if ux is positive
	{
		usat_x -= 4;
		if ( !(usat_x < TPLOT_UX) )					// if usat_x cross limits don't draw
			putpixel(screen, t_pltx, usat_x, RED);
		if ( !(ux < TPLOT_UX) )						// if ux cross limits don't draw
			putpixel(screen, t_pltx, ux, ELECTRIC_BLUE); // draw variables like points
	}

	// highlight the action of derivator
	else if ( (usat_x != ux) && (ux >= THORIZ_UX) )
	{
		usat_x += 4;
		if ( !(usat_x >= BPLOT_UX) )					// if usat_x cross limits don't draw
			putpixel(screen, t_pltx, usat_x, RED);	
		if ( !(ux >= BPLOT_UX) )						// if ux cross limits don't draw
			rect(screen, t_pltx, THORIZ_UX, t_pltx, ux, ELECTRIC_BLUE); // draw variables like points
	}

	else if ( (usat_x != ux) && (ux < THORIZ_UX) )
	{
		usat_x -= 4;
		if ( !(usat_x < TPLOT_UX) )					// if usat_x cross limits don't draw
			putpixel(screen, t_pltx, usat_x, RED);		
		if ( !(ux < TPLOT_UX) )						// if ux cross limits don't draw
			rect(screen, t_pltx, THORIZ_UX, t_pltx, ux, ELECTRIC_BLUE); // draw variables like points
	}

	// update t variable
	t_pltx += 1;
	if(t_pltx > RHORIZ_UX-1) 
	{
		t_pltx = LHORIZ_UX; 		// reset x coordinate of plotting pixel
		rectfill(screen, LPLOT_UX, TPLOT_UX, LPLOT_UX+WPLOT_UX, TPLOT_UX+HPLOT_UX, BKG);  // reset plot
		rect(screen, LHORIZ_UX, THORIZ_UX, RHORIZ_UX, THORIZ_UX, WHITE);		// re-draw horizontal ux axis (time)
	}
}

//------------------------------------------------------------------------------------------
// CONTROL_AIM_X: compute position error to start feedback-control on x-axis (PD controller)
//------------------------------------------------------------------------------------------
int control_aim_x(int ref_x, int target_x)
{
float  	x;						// position of aim (will be transform in skeet-frame)
int  	ex;					// position error 
float 	dt = TSCALE_SKEET * (float) PER_SKEET_TASK / 1000;		// integration interval equal to the skeet_task
float 	ux;				 		// input along x and y axis that rapresents aim shift
int 	delta_x;				// input casting into integer [pixel]

	// acquire aim position (in aim frame)
	pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
	x = aim.x;
	pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate

	// convert aim-frame into skeet-frame
	x += CAIM;

	// compute position error from initial target
	ex = ref_x - x;

	// compute control with a PD controller
	ux = P_x * ex + D_x * (ex - ex_prev)/dt + I_x *( (ex + ex_prev)*dt )/2;
	if (ux > 0)
	{
		// update initial saturation of control until UX_SATMAX (it will progressivly increase even if initial control is high)
		// saturation (usat_x) starts from zero every time a target position is set
		if (usat_x < UX_SATMAX)		// if max saturation is not reached
			usat_x += SATX_SLP*dt;

		else if (usat_x >= UX_SATMAX) // if max saturation is reached
			usat_x = UX_SATMAX;


		// applicate saturation on control ux
		if (ux >= usat_x)	// if control exceeds saturation in that istance saturate it
			ux = usat_x;
	}

	// if ux_old was < 0, so it is necessary a displacement through left
	else if (ux < 0)
	{
		// update initial saturation of control until UX_SATMAX (it will progressivly increase even if initial control is high)
		if (usat_x > -UX_SATMAX) 		// if max saturation is not reached
			usat_x -= SATX_SLP*dt;

		else if (usat_x <= -UX_SATMAX)	// if max saturation is reached
			usat_x = -UX_SATMAX;

		// applicate saturation on control ux
		if (ux <= usat_x)	// if control exceeds saturation in that istance saturate it
			ux = usat_x;			
	}

	if ( x == ref_x ) // it means that aim has reached its destination at low speed
	{
		target_x = 1;	// don't enter again in control function
		usat_x = 0; 	// reset dynamic value of saturation

		return target_x;
	}
	ex_prev = ex;

	delta_x = (int) ux;		// it needs a casting because aim.x works in pixel displacement

	// put pixels of ux and usat_x
	plot_ux_usatx(ux, usat_x);

	pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
	aim.x += delta_x; 							// actuate motor (move aim)
	pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate	

return 0;
}

//------------------------------------------------------------------------------------------------------
// 	PLOT_UY_USATY:  put pixels of ux and usat_y in the next black pixel, if plot is all drawed, cancel it
//------------------------------------------------------------------------------------------------------
void plot_uy_usaty(float uy, float usat_y)
{
char s[10];

	// print uy
	sprintf(s, "uy:%+4.1f", uy); 			
	textout_ex(screen, font, s, LINFO_UY, TINFO_UY, WHITE, BKG);	// text of monitor first line

	// transform physic variable into graphic variable (change sign + scaling + translate)
	uy = (-uy * FS_y) + THORIZ_UY;
	usat_y = (-usat_y * FS_y) + THORIZ_UY;

	// usat will coincide almost always with uy, so only in drawing it will shift from uy
	if ( (usat_y == uy) && (uy >= THORIZ_UY) )
	{
		usat_y += 4;

		if ( !(usat_y >= BPLOT_UY) )		// if usat_y cross limits don't draw
			putpixel(screen, t_plty, usat_y, RED);		
		if ( !(uy >= BPLOT_UY) )			// if uy cross limits don't draw
			putpixel(screen, t_plty, uy, ELECTRIC_BLUE); // draw variables like points
	}

	else if ( (usat_y == uy) && (uy < THORIZ_UY) )
	{
		usat_y -= 4;
		if ( !(usat_y <= TPLOT_UY) )		// if usat_y cross limits don't draw
			putpixel(screen, t_plty, usat_y, RED);
		if ( !(uy <= TPLOT_UY) )			// if uy cross limits don't draw
			putpixel(screen, t_plty, uy, ELECTRIC_BLUE); // draw variables like points
	}

	// highlight the action of derivator
	else if ( (usat_y != uy) && (uy >= THORIZ_UY) )
	{
		usat_y += 4;
		if ( !(usat_y >= BPLOT_UY) )		// if usat_y cross limits don't draw
			putpixel(screen, t_plty, usat_y, RED);		
		if ( !(uy >= BPLOT_UY) )			// if uy cross limits don't draw
			rect(screen, t_plty, THORIZ_UY, t_plty, uy, ELECTRIC_BLUE); // draw variables like points
	}

	else if ( (usat_y != uy) && (uy < THORIZ_UY) )
	{
		usat_y -= 4;
		if ( !(usat_y <= TPLOT_UY) )		// if usat_y cross limits don't draw
			putpixel(screen, t_plty, usat_y, RED);		
		if ( !(uy <= TPLOT_UY) )			// if uy cross limits don't draw
			rect(screen, t_plty, THORIZ_UY, t_plty, uy, ELECTRIC_BLUE); // draw variables like points
	}

	// update t variable
	t_plty += 1;
	if(t_plty > RHORIZ_UY-1) 
	{
		t_plty = LHORIZ_UY; 		// reset x coordinate of plotting pixel
		rectfill(screen, LPLOT_UY, TPLOT_UY, RPLOT_UY, BPLOT_UY, BKG);  // reset plot
		rect(screen, LHORIZ_UX, THORIZ_UY, RHORIZ_UY, THORIZ_UY, WHITE);		// re-draw horizontal uy axis (time)
	}
}

//------------------------------------------------------------------------------------------
// CONTROL_AIM_Y: compute position error to start feedback-control on y-axis (PD controller)
//------------------------------------------------------------------------------------------
int control_aim_y(int ref_y, int target_y)
{		
float  	y;						// position of aim (will be transform in skeet-frame)
int  	ey;					// position error 
float 	dt = TSCALE_SKEET * (float) PER_SKEET_TASK / 1000;		// integration interval equal to the skeet_task
float 	uy;				 		// input along y axis that rapresents aim shift
int 	delta_y;				// input casting into integer [pixel]

	// acquire aim position (in aim frame)
	pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
	y = aim.y;
	pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate

	// convert aim-frame into skeet-frame
	y += -HWORLD + CAIM;
	y = -y;

	// compute position error from initial target
	ey = ref_y - y;

	// compute control with a PD controller
	uy = P_y * ey + D_y *(ey - ey_prev)/dt + I_y *( (ey + ey_prev)*dt )/2;
	if (uy > 0)
	{
		// update initial saturation of control until UY_SATMAX (it will progressivly increase even if initial control is high)
		// saturation (usat_y) starts from zero every time a target position is set
		if (usat_y < UY_SATMAX)		// if max saturation is not reached
			usat_y += SATY_SLP*dt;

		else if (usat_y >= UY_SATMAX) // if max saturation is reached
			usat_y = UY_SATMAX;


		// applicate saturation on control uy
		if (uy >= usat_y)	// if control exceeds saturation in that istance saturate it
			uy = usat_y;
	}

	// if uy_old was < 0, so it is necessary a displacement through left
	else if (uy < 0)
	{
		// update initial saturation of control until UY_SATMAX (it will progressivly increase even if initial control is high)
		if (usat_y > -UY_SATMAX) 		// if max saturation is not reached
			usat_y -= SATY_SLP*dt;

		else if (usat_y <= -UY_SATMAX)	// if max saturation is reached
			usat_y = -UY_SATMAX;

		// applicate saturation on control ux
		if (uy <= usat_y)	// if control exceeds saturation in that istance saturate it
			uy = usat_y;			
	}

	if ( y == ref_y ) // it means that aim has reached its destination at low speed
	{
		target_y = 1;	// don't enter again in control function
		usat_y = 0; 	// reset dynamic value of saturation
		ey_prev = 0;	// reset previous error
		return target_y;
	}
	
	ey_prev = ey;

	delta_y = (int) uy;		// it needs a casting because aim.y works in pixel displacement

	// put pixels of ux and usat_y
	plot_uy_usaty(uy, usat_y);

	pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
	aim.y -= delta_y; 							// actuate motor (move aim)
	pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate	

return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// AUTOM_SHOTGUN: simulation of a robot that predict where the target will be and shoot at the right time, with a certain speed profile of the "motors"
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void*	autom_shotgun(void* arg)
{
int 	i;							// task index
char 	s[30];						// string for monitor deadline messed
int  	aim_x, aim_y;				// value of aim
int 	ref_x, ref_y;		// ref to follow

	// every time task is created automatic-gun mode is set ON, so gun must be on X0_AUT and YO_AUT position	
	targx_init_f = 0;	 		// flag to inform if aim has reached initial target position X0_AUT
	targy_init_f = 0;	 		// flag to inform if aim has reached initial target position Y0_AUT

	usat_x = 0; 				// global variable to initialize control function for aimx
	usat_y = 0; 				// global variable to initialize control function for aimy

	// reset two plot x
	t_pltx = LHORIZ_UX; 		// reset x coordinate of plotting pixel
	rectfill(screen, LPLOT_UX, TPLOT_UX, LPLOT_UX+WPLOT_UX, TPLOT_UX+HPLOT_UX, BKG);  // reset plot
	rect(screen, LHORIZ_UX, THORIZ_UX, RHORIZ_UX, THORIZ_UX, WHITE);		// re-draw horizontal ux axis (time)

	// reset two plot y
	t_plty = LHORIZ_UY; 		// reset x coordinate of plotting pixel
	rectfill(screen, LPLOT_UY, TPLOT_UY, RPLOT_UY, BPLOT_UY, BKG);  // reset plot
	rect(screen, LHORIZ_UY, THORIZ_UY, RHORIZ_UY, THORIZ_UY, WHITE);		// re-draw horizontal ux axis (time)

	// during robot positioning user can' t launch skeet!! for example: SKEET NOT READY like msg_user
	// togliere il TREMOLIO anche

	i = get_task_index(arg);
	set_activation(i);

	while (!end && autom_f)
	{				

		// if initial position not reached by the automatic gun
		if (!targx_init_f && !targy_init_f)
		{
			targx_init_f = control_aim_x(X0_AUT, targx_init_f);
			targy_init_f = control_aim_y(Y0_AUT, targy_init_f);
		}
		else if (!targx_init_f || !targy_init_f) 		// if only one coordinate has reached continue control
		{
			targx_init_f = 0;
			targy_init_f = 0;
		}
		else
		{
			targx_init_f = 1;
			targy_init_f = 1;
		}

		// else if initial position is reached and skeet is in the air

		// read actual x, y of aim
		pthread_mutex_lock(&mux_aim_coordinate);		// wait on the mutex for aim coordinate
		aim_x = aim.x;
		aim_y = aim.y;
		pthread_mutex_unlock(&mux_aim_coordinate);		// signal on the mutex for aim coordinate

		// rotate the aim reference system into skeet reference system
		aim_x += CAIM;
		aim_y += -HWORLD + CAIM;
		aim_y = -aim_y;

		if ( (!targx_f && !targy_f) && skeet_awake_f)
		{
			pthread_mutex_lock(&mux_skeet_state);
			ref_x = skeet.x;
			ref_y = skeet.y;
			pthread_mutex_unlock(&mux_skeet_state);

			control_aim_x(ref_x, targx_f);
			control_aim_y(ref_y, targy_f);
		}
		// if aim is near centre of the skeet shot it
		if ( (skeet_awake_f) && ( (abs(aim_x - ref_x) <= 4) ) && (abs(aim_y - ref_y) <= 4 ) )
		{
			// stop control aim
			targx_f = 1;
			targy_f = 1;
			// shot
			shot_f = 1;
			// activate shot sequence
			shot_seq_f = 1;
			// reset position
			targx_init_f = 0;
			targy_init_f = 0;
		}
		if ( (aim_x > XRWORLD) || (aim_x < XLWORLD) )
		{
			targx_f = 1;
			targy_f = 1;
		}
		else
		{
			// continue to looking for
			targx_f = 0;
			targy_f = 0;
		}
		// else if ( ( (!targx_f && targy_f) || (targx_f && !targy_f) ) && skeet_awake_f) 		// if only one coordinate has reached continue control
		// {
		// 	targx_f = 0;
		// 	targy_f = 0;
		// }
		// // check if the aim is near the centre of the skeet
		// else if (skeet_awake_f)
		// automatic gun starts to acquire information about skeet motion after a specific coordinate

		if (deadline_miss(i))
		{
			autom_tot_dl_m++;
			sprintf(s, "autom_shotgun:%2u", autom_tot_dl_m);
			textout_ex(screen, font, s, LEDGE_MON_TEXT, TEDGE_MON_TEXT7, WHITE, BKG);	// text of monitor third line
		}
		wait_for_activation(i);
	}
	return 0;
}


//--------------------------------------------------------
// MOVE_ARROW_KEYS: read key and translate in aim movement
//--------------------------------------------------------
void move_arrow_keys()
{
int u = key[KEY_UP];
int d = key[KEY_DOWN];
int l = key[KEY_LEFT];
int r = key[KEY_RIGHT];

	// prevent problems if user clicks non natural arrow keys
	if ( (l && u && r) || (u && r && d) || (r && d && l) || (d && l && r) || (l && u && r && d))
	{
		l = 0; u = 0; r = 0; d = 0;
		return;
	}

	else if (u && l)
	{			
		pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
		if (aim.y >= YT_LIM_AIM) 	aim.y -= STEP_AIM;				// make sure that the aim don't go out of the screen
		if (aim.x >= XL_LIM_AIM) 	aim.x -= STEP_AIM;				// make sure that the aim don't go out of the screen	
		pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		u = 0; l = 0;	
	}

	else if (u && r)
	{	
		pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
		if (aim.y >= YT_LIM_AIM) 	aim.y -= STEP_AIM;				// make sure that the aim don't go out of the screen
		if (aim.x <= XR_LIM_AIM) 	aim.x += STEP_AIM;				// you make sure that the aim don't go out of the screen	
		pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		u = 0; r = 0;			
	}

	else if (u)
	{				
		pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
		if (aim.y >= YT_LIM_AIM) 	aim.y -= STEP_AIM;				// make sure that the aim don't go out of the screen
		pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		u = 0;
	}

	else if (d && l)
	{
		pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
		if (aim.y <= YB_LIM_AIM) 	aim.y += STEP_AIM;				// make sure that the aim don't go out of the screen
		if (aim.x >= XL_LIM_AIM) 	aim.x -= STEP_AIM;				// make sure that the aim don't go out of the screen									
		pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		d = 0; l = 0;
	}

	else if (d && r)
	{
		pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
		if (aim.y <= YB_LIM_AIM) 	aim.y += STEP_AIM;				// make sure that the aim don't go out of the screen	
		if (aim.x <= XR_LIM_AIM) 	aim.x += STEP_AIM;				// you make sure that the aim don't go out of the screen								
		pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		d = 0; r = 0;
	}

	else if (d)
	{
		pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
		if (aim.y <= YB_LIM_AIM) 	aim.y += STEP_AIM;				// make sure that the aim don't go out of the screen						
		pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		d = 0;
	}

	else if (l)
	{
		pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
		if (aim.x >= XL_LIM_AIM) 	aim.x -= STEP_AIM;				// make sure that the aim don't go out of the screen	
		pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		l = 0;
	}

	else if (r)
	{
		pthread_mutex_lock(&mux_aim_coordinate);		// wait on mux for aim coordinate
		if (aim.x <= XR_LIM_AIM) 	aim.x += STEP_AIM;				// you make sure that the aim don't go out of the screen	
		pthread_mutex_unlock(&mux_aim_coordinate);		// signal on mux for aim coordinate
		r = 0;
	}	
}

//----------------------------------------------
// HANDLE_VERT_FS: change color of FS parameter
//----------------------------------------------
void handle_vert_FS()
{
char s[20];

	if (vert_sel_f == 1){ 	// only y
		sprintf(s, "FS_y:%2.1f", FS_y); 			
		textout_ex(screen, font, s, LPARAM_FSY, TPARAM_FSY, WHITE, BKG);	// text of monitor first line
		sprintf(s, "FS_x:%2.1f", FS_x); 			
		textout_ex(screen, font, s, LPARAM_FSX, TPARAM_FSX, WHITE, DARK_GREEN);	// text of monitor first line
	}
	if (vert_sel_f == 0){	// from x to xy
		sprintf(s, "FS_x:%2.1f", FS_x); 			
		textout_ex(screen, font, s, LPARAM_FSX, TPARAM_FSX, WHITE, DARK_GREEN);	// text of monitor first line
		sprintf(s, "FS_y:%2.1f", FS_y); 			
		textout_ex(screen, font, s, LPARAM_FSY, TPARAM_FSY, WHITE, DARK_GREEN);	// text of monitor first line
	}
	if (vert_sel_f == -1){	// from xy to y
		sprintf(s, "FS_x:%2.1f", FS_x); 			
		textout_ex(screen, font, s, LPARAM_FSX, TPARAM_FSX, WHITE, BKG);	// text of monitor first line	
		sprintf(s, "FS_y:%2.1f", FS_y); 			
		textout_ex(screen, font, s, LPARAM_FSY, TPARAM_FSY, WHITE, DARK_GREEN);	// text of monitor first line						
	}
}

//----------------------------------------------
// HANDLE_VERT_P: change color of Pf parameter
//----------------------------------------------
void handle_vert_P()
{
char s[20];

		if (vert_sel_f == 1){ 	// from y to x
			sprintf(s, "Py:%2.f", P_y); 			
			textout_ex(screen, font, s, LPARAM_P, TPARAM2, WHITE, BKG);	// text of monitor first line
			sprintf(s, "Px:%2.f", P_x); 			
			textout_ex(screen, font, s, LPARAM_P, TPARAM1, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == 0){	// from x to xy
			sprintf(s, "Px:%2.f", P_x); 			
			textout_ex(screen, font, s, LPARAM_P, TPARAM1, WHITE, DARK_GREEN);	// text of monitor first line
			sprintf(s, "Py:%2.f", P_y); 			
			textout_ex(screen, font, s, LPARAM_P, TPARAM2, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == -1){	// from xy to y
			sprintf(s, "Px:%2.f", P_x); 			
			textout_ex(screen, font, s, LPARAM_P, TPARAM1, WHITE, BKG);	// text of monitor first line	
			sprintf(s, "Py:%2.f", P_y); 			
			textout_ex(screen, font, s, LPARAM_P, TPARAM2, WHITE, DARK_GREEN);	// text of monitor first line						
		}
}

//----------------------------------------------
// HANDLE_VERT_I: change color of I parameter
//----------------------------------------------
void handle_vert_I()
{
char s[20];

		if (vert_sel_f == 1){ 	// from y to x
			sprintf(s, "Iy:%2.1f", I_y); 			
			textout_ex(screen, font, s, LPARAM_I, TPARAM2, WHITE, BKG);	// text of monitor first line
			sprintf(s, "Ix:%2.1f", I_x); 			
			textout_ex(screen, font, s, LPARAM_I, TPARAM1, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == 0){	// from x to xy
			sprintf(s, "Ix:%2.1f", I_x); 			
			textout_ex(screen, font, s, LPARAM_I, TPARAM1, WHITE, DARK_GREEN);	// text of monitor first line
			sprintf(s, "Iy:%2.1f", I_y); 			
			textout_ex(screen, font, s, LPARAM_I, TPARAM2, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == -1){	// from xy to y
			sprintf(s, "Ix:%2.1f", I_x); 			
			textout_ex(screen, font, s, LPARAM_I, TPARAM1, WHITE, BKG);	// text of monitor first line	
			sprintf(s, "Iy:%2.1f", I_y); 			
			textout_ex(screen, font, s, LPARAM_I, TPARAM2, WHITE, DARK_GREEN);	// text of monitor first line						
		}
}

//----------------------------------------------
// HANDLE_VERT_D: change color of D parameter
//----------------------------------------------
void handle_vert_D()
{
char s[20];

	if (vert_sel_f == 1){ 	// from y to x
		sprintf(s, "Dy:%2.1f", D_y); 			
		textout_ex(screen, font, s, LPARAM_D, TPARAM2, WHITE, BKG);	// text of monitor first line
		sprintf(s, "Dx:%2.1f", D_x); 			
		textout_ex(screen, font, s, LPARAM_D, TPARAM1, WHITE, DARK_GREEN);	// text of monitor first line
	}
	else if (vert_sel_f == 0){	// from x to xy
		sprintf(s, "Dx:%2.1f", D_x); 			
		textout_ex(screen, font, s, LPARAM_D, TPARAM1, WHITE, DARK_GREEN);	// text of monitor first line
		sprintf(s, "Dy:%2.1f", D_y); 			
		textout_ex(screen, font, s, LPARAM_D, TPARAM2, WHITE, DARK_GREEN);	// text of monitor first line
	}
	else if (vert_sel_f == -1){	// from xy to y
		sprintf(s, "Dx:%2.1f", D_x); 			
		textout_ex(screen, font, s, LPARAM_D, TPARAM1, WHITE, BKG);	// text of monitor first line	
		sprintf(s, "Dy:%2.1f", D_y); 			
		textout_ex(screen, font, s, LPARAM_D, TPARAM2, WHITE, DARK_GREEN);	// text of monitor first line						
	}
}

//----------------------------------------------
// handle_vert_UMAX: change color of UMAX parameter
//----------------------------------------------
void handle_vert_UMAX()
{
char s[20];

	if (vert_sel_f == 1){ 	// from y to x
		sprintf(s, "vy_max:%2.f", UY_SATMAX); 			
		textout_ex(screen, font, s, LPARAM_UMAX, TPARAM4, WHITE, BKG);	// text of monitor first line
		sprintf(s, "vx_max:%2.f", UX_SATMAX); 			
		textout_ex(screen, font, s, LPARAM_UMAX, TPARAM3, WHITE, DARK_GREEN);	// text of monitor first line
	}
	else if (vert_sel_f == 0){	// from x to xy
		sprintf(s, "vx_max:%2.f", UX_SATMAX); 			
		textout_ex(screen, font, s, LPARAM_UMAX, TPARAM3, WHITE, DARK_GREEN);	// text of monitor first line
		sprintf(s, "vy_max:%2.f", UY_SATMAX); 			
		textout_ex(screen, font, s, LPARAM_UMAX, TPARAM4, WHITE, DARK_GREEN);	// text of monitor first line
	}
	else if (vert_sel_f == -1){	// from xy to y
		sprintf(s, "vx_max:%2.f", UX_SATMAX); 			
		textout_ex(screen, font, s, LPARAM_UMAX, TPARAM3, WHITE, BKG);	// text of monitor first line
		sprintf(s, "vy_max:%2.f", UY_SATMAX); 			
		textout_ex(screen, font, s, LPARAM_UMAX, TPARAM4, WHITE, DARK_GREEN);	// text of monitor first line
	}
}

//----------------------------------------------
// HANDLE_VERT_SLP: change color of SLP parameter
//----------------------------------------------
void handle_vert_SLP()
{
char s[20];

	if (vert_sel_f == 1){ 	// from y to x
		sprintf(s, "ay:%2.1f", SATY_SLP); 			
		textout_ex(screen, font, s, LPARAM_SLP, TPARAM4, WHITE, BKG);	// text of monitor first line
		sprintf(s, "ax:%2.1f", SATX_SLP); 			
		textout_ex(screen, font, s, LPARAM_SLP, TPARAM3, WHITE, DARK_GREEN);	// text of monitor first line
	}
	else if (vert_sel_f == 0){	// from x to xy
		sprintf(s, "ax:%2.1f", SATX_SLP); 			
		textout_ex(screen, font, s, LPARAM_SLP, TPARAM3, WHITE, DARK_GREEN);	// text of monitor first line
		sprintf(s, "ay:%2.1f", SATY_SLP); 			
		textout_ex(screen, font, s, LPARAM_SLP, TPARAM4, WHITE, DARK_GREEN);	// text of monitor first line
	}
	else if (vert_sel_f == -1){	// from xy to y
		sprintf(s, "ax:%2.1f", SATX_SLP); 			
		textout_ex(screen, font, s, LPARAM_SLP, TPARAM3, WHITE, BKG);	// text of monitor first line
		sprintf(s, "ay:%2.1f", SATY_SLP); 			
		textout_ex(screen, font, s, LPARAM_SLP, TPARAM4, WHITE, DARK_GREEN);	// text of monitor first line
	}
}
//--------------------------------------------------------------------------------------
// EDIT_AUTOM_PARAM: edit parameters; oper stands for operator (1 OR -1)
//--------------------------------------------------------------------------------------
void edit_autom_param(float oper)
{
char s[30];

	if (oriz_sel_f == FS_f)
	{
		if (vert_sel_f == 1){
			FS_x += 0.5*oper;
			sprintf(s, "FS_x:%2.1f", FS_x); 			
			textout_ex(screen, font, s, LPARAM_FSX, TPARAM_FSX, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == 0){
			FS_x += 0.5*oper;
			sprintf(s, "FS_x:%2.1f", FS_x); 			
			textout_ex(screen, font, s, LPARAM_FSX, TPARAM_FSX, WHITE, DARK_GREEN);	// text of monitor first line
			FS_y += 0.5*oper;
			sprintf(s, "FS_y:%2.1f", FS_y); 			
			textout_ex(screen, font, s, LPARAM_FSY, TPARAM_FSY, WHITE, DARK_GREEN);	// text of monitor first line

		}
		else if (vert_sel_f == -1){
			FS_y += 0.5*oper;
			sprintf(s, "FS_y:%2.1f", FS_y); 			
			textout_ex(screen, font, s, LPARAM_FSY, TPARAM_FSY, WHITE, DARK_GREEN);	// text of monitor first line
		}


	}
	if (oriz_sel_f == P_f)
	{
		if (vert_sel_f == 1){
			P_x += oper;
			sprintf(s, "Px:%2.f", P_x); 			
			textout_ex(screen, font, s, LPARAM_P, TPARAM1, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == 0){
			P_x += oper;
			sprintf(s, "Px:%2.f", P_x); 			
			textout_ex(screen, font, s, LPARAM_P, TPARAM1, WHITE, DARK_GREEN);	// text of monitor first line
			P_y += oper;
			sprintf(s, "Py:%2.f", P_y); 			
			textout_ex(screen, font, s, LPARAM_P, TPARAM2, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == -1){
			P_y += oper;
			sprintf(s, "Py:%2.f", P_y); 			
			textout_ex(screen, font, s, LPARAM_P, TPARAM2, WHITE, DARK_GREEN);	// text of monitor first line
		}
	}
	if (oriz_sel_f == I_f)
	{
		if (vert_sel_f == 1){
			I_x += 0.1*oper;
			sprintf(s, "Ix:%2.1f", I_x); 			
			textout_ex(screen, font, s, LPARAM_I, TPARAM1, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == 0){
			I_x += 0.1*oper;
			sprintf(s, "Ix:%2.1f", I_x); 			
			textout_ex(screen, font, s, LPARAM_I, TPARAM1, WHITE, DARK_GREEN);	// text of monitor first line
			I_y += 0.1*oper;
			sprintf(s, "Iy:%2.1f", I_y); 			
			textout_ex(screen, font, s, LPARAM_I, TPARAM2, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == -1){
			I_y += 0.1*oper;
			sprintf(s, "Iy:%2.1f", I_y); 			
			textout_ex(screen, font, s, LPARAM_I, TPARAM2, WHITE, DARK_GREEN);	// text of monitor first line

		}
	}
	if (oriz_sel_f == D_f)
	{
		if (vert_sel_f == 1){
			D_x += 0.1*oper;
			sprintf(s, "Dx:%2.1f", D_x); 			
			textout_ex(screen, font, s, LPARAM_D, TPARAM1, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == 0){
			D_x += 0.1*oper;
			sprintf(s, "Dx:%2.1f", D_x); 			
			textout_ex(screen, font, s, LPARAM_D, TPARAM1, WHITE, DARK_GREEN);	// text of monitor first line
			D_y += 0.1*oper;
			sprintf(s, "Dy:%2.1f", D_y); 			
			textout_ex(screen, font, s, LPARAM_D, TPARAM2, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == -1){
			D_y += 0.1*oper;
			sprintf(s, "Dy:%2.1f", D_y); 			
			textout_ex(screen, font, s, LPARAM_D, TPARAM2, WHITE, DARK_GREEN);	// text of monitor first line
		}
	}
	if (oriz_sel_f == UMAX_f)
	{
		if (vert_sel_f == 1){
			UX_SATMAX += oper;
			sprintf(s, "vx_max:%2.f", UX_SATMAX); 			
			textout_ex(screen, font, s, LPARAM_UMAX, TPARAM3, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == 0){
			UX_SATMAX += oper;
			sprintf(s, "vx_max:%2.f", UX_SATMAX); 			
			textout_ex(screen, font, s, LPARAM_UMAX, TPARAM3, WHITE, DARK_GREEN);	// text of monitor first line
			UY_SATMAX += oper;
			sprintf(s, "vy_max:%2.f", UY_SATMAX); 			
			textout_ex(screen, font, s, LPARAM_UMAX, TPARAM4, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == -1){
			UY_SATMAX += oper;
			sprintf(s, "vy_max:%2.f", UY_SATMAX); 			
			textout_ex(screen, font, s, LPARAM_UMAX, TPARAM4, WHITE, DARK_GREEN);	// text of monitor first line
		}
	}
	if (oriz_sel_f == SLP_f)
	{
		if (vert_sel_f == 1){
			SATX_SLP += 0.1*oper;
			sprintf(s, "ax:%2.1f", SATX_SLP); 			
			textout_ex(screen, font, s, LPARAM_SLP, TPARAM3, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == 0){
			SATX_SLP += 0.1*oper;
			sprintf(s, "ax:%2.1f", SATX_SLP); 			
			textout_ex(screen, font, s, LPARAM_SLP, TPARAM3, WHITE, DARK_GREEN);	// text of monitor first line
			SATY_SLP += 0.1*oper;
			sprintf(s, "ay:%2.1f", SATY_SLP); 			
			textout_ex(screen, font, s, LPARAM_SLP, TPARAM4, WHITE, DARK_GREEN);	// text of monitor first line
		}
		else if (vert_sel_f == -1){
			SATY_SLP += 0.1*oper;
			sprintf(s, "ay:%2.1f", SATY_SLP); 			
			textout_ex(screen, font, s, LPARAM_SLP, TPARAM4, WHITE, DARK_GREEN);	// text of monitor first line
		}
	}

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
		// if automatic-shot mode is not activate
		if (!autom_f)
			move_arrow_keys();
		// if automatic-shot mode is ON arrow keys are used to editing parameter
					
		scan = 0;
		if (keypressed())	// returns true if there are characters in keyboard buffer, else false (it does not block the program execution)
		{
			scan = readkey() >> 8;		// shift right the high byte	
			switch (scan)
			{

				case KEY_A:
					if (skeet_alive_f == 0)
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
					if (skeet_alive_f == 0)
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
					if ( (anticip_shot_f == 0) && (skeet_alive_f == 0)) 	// if mode is OFF change to ON
					{
						anticip_shot_f = 1;
						textout_ex(screen, font, "X_KEY: 'anticipate-shot' mode ON ", LMODE_TEXT2, TMODE_TEXT2, YELLOW, BKG);
						textout_ex(screen, font, "[calculate right time before shooting]", LDESCRIP_TEXT2, TDESCRIP_TEXT2, WHITE, BKG);
					}
					else if ( (anticip_shot_f == 1) && (skeet_alive_f == 0))	// if mode is ON change to OFF
					{
						anticip_shot_f = 0;
						textout_ex(screen, font, "X_KEY: 'anticipate-shot' mode OFF", LMODE_TEXT2, TMODE_TEXT2, LIGHT_GRAY, BKG);
						textout_ex(screen, font, "--------------------------------------", LDESCRIP_TEXT2, TDESCRIP_TEXT2, BKG, BKG);
					}
					else if (skeet_alive_f != 0)
					{
						flash_f = 1;
						msg_user_new = 	MSG_USER_WAIT_MODE;
					}
					break;

				case KEY_Z:
					if ( (!night_f) && (!skeet_alive_f)) 	// if background was sunny change to night
					{
						textout_ex(screen, font, "Z KEY: change background", LCOM_TEXT1, TCOM_TEXT5, LIGHT_GRAY, BKG);
						night_f = 1;
					}
					else if ( (night_f) && (!skeet_alive_f)) 	// if background was night change to sunny
					{
						textout_ex(screen, font, "Z KEY: change background", LCOM_TEXT1, TCOM_TEXT5, YELLOW, BKG);
						night_f = 0;
					}
					else if (skeet_alive_f)
					{
						flash_f = 1;
						msg_user_new = 	MSG_USER_WAIT_MODE;
					}
					break;

				case KEY_D:
					if (!flick_f)		// if flickering aim mode is OFF turn it ON
					{
						flick_f = 1;
						task_create(aim_flickering, IDX_FLICKER_AIM, PER_FLICKER_AIM, DREL_FLICKER_AIM, PRIO_FLICKER_AIM);
						textout_ex(screen, font, "D KEY: 'flickering_aim' mode ON ", LMODE_TEXT3, TMODE_TEXT3, YELLOW, BKG);
					}
					else if (flick_f)	// if flickering aim mode is ON turn it OFF
					{
						flick_f = 0;
						textout_ex(screen, font, "D KEY: 'flickering_aim' mode OFF", LMODE_TEXT3, TMODE_TEXT3, LIGHT_GRAY, BKG);
					}
					break;

				case KEY_F:
					if ( (!autom_f) && (!skeet_alive_f)) 	// if automatic-gun mode is OFF turn it ON
					{
						autom_f = 1;
						task_create(autom_shotgun, IDX_AUTOM, PER_AUTOM, DREL_AUTOM, PRIO_AUTOM);

						textout_ex(screen, font, "F KEY: 'auto-shot mode ON ", LMODE_TEXT4, TMODE_TEXT4, YELLOW, BKG);
						textout_ex(screen, font, "[automatic shotgun will shoot the target]", LDESCRIP_TEXT4, TDESCRIP_TEXT4, WHITE, BKG);
					}
					else if ( (autom_f) && (!skeet_alive_f)) 	// if automatic-gun mode is ON turn it OFF
					{
						autom_f = 0;
						textout_ex(screen, font, "F KEY: 'auto-shot mode OFF", LMODE_TEXT4, TMODE_TEXT4, LIGHT_GRAY, BKG);
						textout_ex(screen, font, "[---------------------------------------]", LDESCRIP_TEXT4, TDESCRIP_TEXT4, BKG, BKG);
					}
					else if (skeet_alive_f)
					{
						flash_f = 1;
						msg_user_new = 	MSG_USER_WAIT_MODE;
					}

					break;

				case KEY_C:
					if (skeet_alive_f == 0)		// if no skeet are in the window
					{
						textout_ex(screen, font, "CALLED         ", LINFO_STATE_SKEET, TINFO_TEXT_SKEET, NEON_CARROT, BKG);
						ammo = 2;								// reload the rifle
						blit(ammo_img, screen, 0, 0, XL_AMMO1_BP, YT_AMMO_BP, ammo_img->w, ammo_img->h); // draw ammo BITMAP1
						blit(ammo_img, screen, 0, 0, XL_AMMO2_BP, YT_AMMO_BP, ammo_img->w, ammo_img->h); // draw ammo BITMAP2

						skeet_alive_f = 1;
						task_create(skeet_task, IDX_SKEET_TASK, PER_SKEET_TASK, DREL_SKEET_TASK, PRIO_SKEET_TASK); // idx becomes 1

						// draw the reloadin sequence of the shotgun 
						reload_seq_f = 1;

						if (autom_f)
						{
						targx_f = 0;
						targy_f = 0;
						// reset plot x
						t_pltx = LHORIZ_UX; 		// reset x coordinate of plotting pixel
						rectfill(screen, LPLOT_UX, TPLOT_UX, LPLOT_UX+WPLOT_UX, TPLOT_UX+HPLOT_UX, BKG);  // reset plot
						rect(screen, LHORIZ_UX, THORIZ_UX, RHORIZ_UX, THORIZ_UX, WHITE);		// re-draw horizontal ux axis (time)
						// reset plot y
						t_plty = LHORIZ_UY; 		// reset y coordinate of plotting pixel
						rectfill(screen, LPLOT_UY, TPLOT_UY, LPLOT_UY+WPLOT_UY, TPLOT_UY+HPLOT_UY, BKG);  // reset plot
						rect(screen, LHORIZ_UY, THORIZ_UY, RHORIZ_UY, THORIZ_UY, WHITE);		// re-draw horizontal uy axis (time)
						}

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
					if (autom_f) break; // if automatic-gun mode is on user can' t
					
					if ((ammo > 0) && (!handle_anticip_f))		// second condition is for an unlucky case : second shot pressed,
					{											// while first explosion has begin yet (in anticip mode)
						ammo--;

						// tells to draw_skeet to draw the shot sequence
						shot_seq_f = 1;

						if(ammo == 1)
						rectfill(screen, XL_AMMO2_BP, YT_AMMO_BP, XL_AMMO2_BP + ammo_img->w, YT_AMMO_BP + ammo_img->h, BKG); //cancel ammo BITMAP2
						if(ammo == 0)
						rectfill(screen, XL_AMMO1_BP, YT_AMMO_BP, XL_AMMO1_BP + ammo_img->w, YT_AMMO_BP + ammo_img->h, BKG); //cancel ammo BITMAP1

						if(!skeet_awake_f)	// wasted shot, no need to compute if player hit skeet
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

				case KEY_TAB:
					if (!autom_f) break; 	// do nothing if mode auto-gun is not enable

					vert_sel_f--;
					if (vert_sel_f == -2)
						vert_sel_f = 1;

					if (oriz_sel_f == FS_f)
						handle_vert_FS();

					else if (oriz_sel_f == P_f)
						handle_vert_P();

					else if (oriz_sel_f == I_f)
						handle_vert_I();

					else if (oriz_sel_f == D_f)
						handle_vert_D();

					else if (oriz_sel_f == UMAX_f)
						handle_vert_UMAX();

					else if (oriz_sel_f == SLP_f)
						handle_vert_SLP();

					break;


				case KEY_LEFT:
				if(!autom_f) break;
					if (oriz_sel_f == FS_f){}	// don' t do nothing

					else if (oriz_sel_f == P_f)
					{
						oriz_sel_f--;

						// turn off P_f
						sprintf(s, "Px:%2.f", P_x); 			
						textout_ex(screen, font, s, LPARAM_P, TPARAM1, WHITE, BKG);	// text of monitor first line
						sprintf(s, "Py:%2.f", P_y); 			
						textout_ex(screen, font, s, LPARAM_P, TPARAM2, WHITE, BKG);	// text of monitor first line
						// turn on FS
						handle_vert_FS();
					}
					else if (oriz_sel_f == I_f)
					{
						oriz_sel_f--;

						// turn off I_f
						sprintf(s, "Ix:%2.1f", I_x); 			
						textout_ex(screen, font, s, LPARAM_I, TPARAM1, WHITE, BKG);	// text of monitor first line
						sprintf(s, "Iy:%2.1f", I_y); 			
						textout_ex(screen, font, s, LPARAM_I, TPARAM2, WHITE, BKG);	// text of monitor first line
						// turn on P
						handle_vert_P();
					}
					else if (oriz_sel_f == D_f)
					{
						oriz_sel_f--;
						// turn off D_f
						sprintf(s, "Dx:%2.1f", D_x); 			
						textout_ex(screen, font, s, LPARAM_D, TPARAM1, WHITE, BKG);	// text of monitor first line
						sprintf(s, "Dy:%2.1f", D_y); 			
						textout_ex(screen, font, s, LPARAM_D, TPARAM2, WHITE, BKG);	// text of monitor first line
						// turn on I
						handle_vert_I();

					}
					else if (oriz_sel_f == UMAX_f)
					{
						oriz_sel_f--;
						// turn off UMAX_f
						sprintf(s, "vx_max:%2.f", UX_SATMAX); 			
						textout_ex(screen, font, s, LPARAM_UMAX, TPARAM3, WHITE, BKG);	// text of monitor first line
						sprintf(s, "vy_max:%2.f", UY_SATMAX); 			
						textout_ex(screen, font, s, LPARAM_UMAX, TPARAM4, WHITE, BKG);	// text of monitor first line
						// turn on D
						handle_vert_D();

					}
					else if (oriz_sel_f == SLP_f)
					{
						oriz_sel_f--;
						// turn off SLP_f
						sprintf(s, "ax:%2.1f", SATX_SLP); 			
						textout_ex(screen, font, s, LPARAM_SLP, TPARAM3, WHITE, BKG);	// text of monitor first line
						sprintf(s, "ay:%2.1f", SATY_SLP); 			
						textout_ex(screen, font, s, LPARAM_SLP, TPARAM4, WHITE, BKG);	// text of monitor first line
						// turn on UMAX						
						handle_vert_UMAX();
					}

				break;

				case KEY_RIGHT:
				if(!autom_f) break;
					if (oriz_sel_f == SLP_f){}	// don' t do nothing

					else if (oriz_sel_f == FS_f)
					{
						oriz_sel_f++;

						// turn off FS
						sprintf(s, "FS_x:%2.1f", FS_x); 			
						textout_ex(screen, font, s, LPARAM_FSX, TPARAM_FSX, WHITE, BKG);	// text of monitor first line
						sprintf(s, "FS_y:%2.1f", FS_y); 			
						textout_ex(screen, font, s, LPARAM_FSY, TPARAM_FSY, WHITE, BKG);	// text of monitor first line
						// turn on P
						handle_vert_P();
					}
					else if (oriz_sel_f == P_f)
					{
						oriz_sel_f++;

						// turn off P_f
						sprintf(s, "Px:%2.f", P_x); 			
						textout_ex(screen, font, s, LPARAM_P, TPARAM1, WHITE, BKG);	// text of monitor first line
						sprintf(s, "Py:%2.f", P_y); 			
						textout_ex(screen, font, s, LPARAM_P, TPARAM2, WHITE, BKG);	// text of monitor first line
						// turn on D
						handle_vert_I();
					}
					else if (oriz_sel_f == I_f)
					{
						oriz_sel_f++;

						// turn off I_f
						sprintf(s, "Ix:%2.1f", I_x); 			
						textout_ex(screen, font, s, LPARAM_I, TPARAM1, WHITE, BKG);	// text of monitor first line
						sprintf(s, "Iy:%2.1f", I_y); 			
						textout_ex(screen, font, s, LPARAM_I, TPARAM2, WHITE, BKG);	// text of monitor first line
						// turn on D
						handle_vert_D();
					}
					else if (oriz_sel_f == D_f)
					{
						oriz_sel_f++;
						// turn off D_f
						sprintf(s, "Dx:%2.1f", D_x); 			
						textout_ex(screen, font, s, LPARAM_D, TPARAM1, WHITE, BKG);	// text of monitor first line
						sprintf(s, "Dy:%2.1f", D_y); 			
						textout_ex(screen, font, s, LPARAM_D, TPARAM2, WHITE, BKG);	// text of monitor first line
						// turn on UMAX
						handle_vert_UMAX();

					}
					else if (oriz_sel_f == UMAX_f)
					{
						oriz_sel_f++;
						// turn off UMAX_f
						sprintf(s, "vx_max:%2.f", UX_SATMAX); 			
						textout_ex(screen, font, s, LPARAM_UMAX, TPARAM3, WHITE, BKG);	// text of monitor first line
						sprintf(s, "vy_max:%2.f", UY_SATMAX); 			
						textout_ex(screen, font, s, LPARAM_UMAX, TPARAM4, WHITE, BKG);	// text of monitor first line
						// turn on SLP
						handle_vert_SLP();

					}
					else if (oriz_sel_f == SLP_f) {} // don' t do nothing

				break;

				case KEY_UP:
					if (!autom_f) break;

					edit_autom_param(1);
					break;

				case KEY_DOWN:
					if (!autom_f) break;

					edit_autom_param(-1);
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