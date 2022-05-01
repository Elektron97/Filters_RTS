#ifndef BALLLIB_H_
#define BALLLIB_H_

//-----------------------------------------------------
// GRAPHICS CONSTANTS
//-----------------------------------------------------
#define XWIN 1440	// window x resolution
#define YWIN 852	// window y resolution
#define BKG	0			// background color
#define	BCOL 0		// box color
#define MCOL 5		// menu box color
#define ICOL 9		// info box color
#define TXTCOL 15	// text color
#define TCOL 15		// trail color
#define XBOX 852	// X length of the ball box
#define YBOX 852	// Y height of the ball box
#define MAXZOOM 3	// max zoom
//-----------------------------------------------------
// BALL CONSTANTS
//-----------------------------------------------------
#define MAX_BALLS 14	// max number of balls
#define G0 10		// initial acceleration of gravity
#define MAXG 5*27	// max gravity
#define MTLEN	27	// max trail length
#define HMIN 200	// min initial height
#define HMAX 390	// max initial height
#define VMIN -10	// min initial speed
#define VMAX	10	// max initial speed
#define DUMP 0.8	// dumping coefficient
#define TSCALE 0.01	// time scale factor
#define dt	(TSCALE * BPER)	// discrete time interval
#define	RMIN 10		// minimum radius
#define	RMAX (XBOX/20)		// maximum radius
//-----------------------------------------------------
// TASK CONSTANTS
//-----------------------------------------------------
#define UPER 40	// user task period
#define DPER 20 // display task period
#define BPER 15 // ball task period
#define	UPRIO	1	// user interaction task priority
#define DPRIO 2	// display task priority
#define	BPRIO 3	// ball task priority
#define UIDX (MAX_BALLS+2)	// user task index
#define	DIDX (MAX_BALLS+1)	// display task index
//-----------------------------------------------------
//ball status/info//
struct status{
	int	c;		//color [1,15]
	float	r;	//radius (m)
	float m;	//mass (kg)
	float	x;	//coordinate x (m)
	float	y;	//coordinate y (m)
	float	vx;	//horizontal velocity (m/s)
	float	vy;	//vertical velocity (m/s)
};

//circular buffer//
struct	cbuf{
	int	top;
	int	x[MTLEN];
	int	y[MTLEN];
};

struct status ball[MAX_BALLS];
struct cbuf trail[MAX_BALLS];

void	storeTrail(int	idx);
void	drawTrail(int	idx, int	len, BITMAP	*scr);
void	drawBall(int	idx, BITMAP	*scr);
void	drawInfo();
void	command();
int		touch(int	idx1, int	idx2);
void	setBounceVel(int	idx1, int	idx2);
void	handleBounce(int idx);
void	initBall(int idx);
void	*ballTask(void	*arg);
void	*displayTask(void	*arg);
void	*userTask(void	*arg);
void	init(void);
void	zoom(BITMAP	*copy);

#endif /* BALLLIB_H_ */