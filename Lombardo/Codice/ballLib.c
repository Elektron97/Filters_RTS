#include	<stdlib.h>
#include	<stdio.h>
#include	<math.h>
#include	<pthread.h>
#include	<sched.h>
#include	<allegro.h>
#include	<time.h>
#include	"ballLib.h"
#include	"taskLib.h"
#include	"timeLib.h"

//-----------------------------------------------------
// GLOBAL VARIABLES
//-----------------------------------------------------
int	nab = 0;		//active balls number
int	tflag = 0;	//trail flag
int	tlen = 10;	//actual trail length
int	end = 0;		//end flag
float	g = G0;		//gravity constant
int	zoomLvl = 1; 	//zoom level 
//-----------------------------------------------------

//storeTrail//
void	storeTrail(int	idx){
	int	k;

	if (idx >= MAX_BALLS) return;
	k = trail[idx].top;	//get cBuf last data
	k = (k + 1) % MTLEN;	//increment
	trail[idx].x[k] = ball[idx].x;	//save
	trail[idx].y[k] = ball[idx].y;
	trail[idx].top = k;	//set cBuf index
}

//drawTrail draws just "len" frames//
void	drawTrail(int	idx, int	len, BITMAP	*scr){
	int	j, k;
	int	x, y;

	for (j=0; j<len; j++){
		k = (trail[idx].top + tlen - j) % tlen;
		x = trail[idx].x[k];
		y = trail[idx].y[k];
		putpixel(scr, x, y, TCOL);
	}
}

//drawBall//
void	drawBall(int	idx, BITMAP	*scr){
	int	x, y;

	x = ball[idx].x;
	y = ball[idx].y;
	circlefill(scr, x, y, ball[idx].r, ball[idx].c);
}

//drawInfo//
void	drawInfo(){
	int	i;
	int	dlmiss = 0;
	char dls[100];
	BITMAP*	copy;
	copy = create_bitmap(XWIN, YWIN);	// proxy screen
	
	rectfill(copy, XBOX, 0, XWIN, YBOX/2, ICOL);
	textout(copy, font, "GRAVITY METER [5 m/s^2]", 880, 30, TXTCOL);
	textout(copy, font, "TRAILS", 880, 80, TXTCOL);
	textout(copy, font, "BALLS", 880, 130, TXTCOL);
	textout(copy, font, "ZOOM FACTOR", 880, 180, TXTCOL);
	textout(copy, font, "DEADLINE MISSES", 880, 230, TXTCOL);
	for (i=0; i<g/5; i++){
		rectfill(copy, 880 + i*20, 50, 890 + i*20, 60, TXTCOL);	// displays gravity meter
	}
	for (i=0; i<tlen; i++){
		if (tflag)
			rectfill(copy, 880 + i*20, 100, 890 + i*20, 110, TXTCOL);	// displays trails meter
		else
			rect(copy, 880 + i*20, 100, 890 + i*20, 110, TXTCOL);
	}
	for (i=0; i<nab; i++){
		circlefill(copy, 890 + i*20, 160, 7, TXTCOL);	// displays balls number
		dlmiss += param[i].dMiss;	// sums all balls dl misses
	}
	for (i=0; i<zoomLvl; i++){
		circle(copy, 890 + i*20, 210, 7, TXTCOL);	// displays zoom level
		circlefill(copy, 890 + i*20, 210, 2, TXTCOL);
	}
	sprintf(dls, "BALLS: %d DISPLAY: %d USER: %d", dlmiss, param[DIDX].dMiss, param[UIDX].dMiss);	
	textout(copy, font, dls, 890, 250, TXTCOL);
	blit(copy, screen, XBOX, 0, XBOX, 0, XWIN-XBOX, YWIN/2);
}

//command translates user inputs//
void	command(){
	char	scan = get_scancode();

	switch (scan) {
		case KEY_ENTER:	// create new ball
			if (nab < MAX_BALLS){
				nab++;
				task_create(ballTask, nab-1, BPER, BPER, BPRIO);
			}
			break;
		case KEY_UP:	// increment gravity
			if (g < MAXG) g = g + 5;
			break;
		case KEY_DOWN:	// decrement gravity
			if (g > 1) g = g - 5;
			break;
		case KEY_RIGHT:	// increment trails length
			if (tlen < MTLEN) tlen = tlen + 1;
			break;
		case KEY_LEFT:	// decrement trails length
			if (tlen > 1) tlen = tlen - 1;
			break;
		case KEY_T:	// toggle trails 
			if (tflag == 0) 
				tflag = 1;
			else
				tflag = 0;
			break;
		case KEY_SPACE: // make every ball shake
			for (int	i=0; i<nab; i++){
				ball[i].vy = frand(VMIN, 0)*20;
				ball[i].vx = frand(VMIN, VMAX)*20;
			}
			break;
		case KEY_I:	// increment zoom level
			if (zoomLvl < MAXZOOM){
				zoomLvl = zoomLvl + 1;
			}
				break;
		case KEY_O:	// decrement zoom level
			if (zoomLvl > 1){
				zoomLvl = zoomLvl - 1;
			}
				break;
		case KEY_ESC:	// exit
			end = 1;
			break;
		default: break;
	}
}

//overlapping returns how much two balls are overlapping//
float	overlapping(int	idx1, int	idx2){
	float	ol;

	ol = - pow(ball[idx1].x - ball[idx2].x, 2) - pow(ball[idx1].y - ball[idx2].y, 2) + pow(ball[idx1].r + ball[idx2].r, 2);
	return	ol;
}

//contact returns index of the first ball found colliding with the "idx" ball, if existing
int	contact(int	idx){
	float	tol = 1;	//hitboxes tolerance

	for (int i=0; i<nab; i++){
		if (idx != i){
			if (overlapping(idx, i) > tol){	//detects overlapping
		 		return	i;
			}
		}
	}
	return	-1;
}

//setBounceVel sets the speed of the two balls colliding (elasticly)//
void	setBounceVel(int	idx1, int	idx2){
	float	vx1 = ball[idx1].vx;
	float	vy1 = ball[idx1].vy;
	float	vx2 = ball[idx2].vx;
	float	vy2 = ball[idx2].vy;
	float	m1 = ball[idx1].m;
	float	m2 = ball[idx2].m;

	//conservation of momenta
	ball[idx1].vx = ( (m1-m2)*vx1 + 2*m2*vx2 )/(m1+m2);
	ball[idx2].vx = ( (m2-m1)*vx2 + 2*m1*vx1 )/(m1+m2);
	ball[idx1].vy = ( (m1-m2)*vy1 + 2*m2*vy2 )/(m1+m2);
	ball[idx2].vy = ( (m2-m1)*vy2 + 2*m1*vy1 )/(m1+m2);
}

//handleBounce implements the physics of bounces using global physical constants editable by the user//
void	handleBounce(int idx){
	int	collIdx;	// index of the colliding ball
	float	alpha;	// angle of the rect joining the two balls centers
	float	overlap;// overlap magnitude

	//collision with other ball
	collIdx = contact(idx);
	if (collIdx != -1){		//collIdx becomes the index of the colliding ball via contact() function: if -1 no ball have collided and if exits
		overlap = overlapping(idx, collIdx);
		if (ball[idx].x > ball[collIdx].x){
			alpha = atan( (ball[idx].y-ball[collIdx].y) / (ball[idx].x-ball[collIdx].x) );
			ball[idx].x += cosf(alpha) * sqrtf(abs(overlap)); // the balls are resetted in the most realistic position
			ball[idx].y -= sinf(alpha) * sqrtf(abs(overlap));	// 
		}
		else{
			alpha = atan( (ball[collIdx].y-ball[idx].y) / (ball[collIdx].x-ball[idx].x) );
			ball[idx].x -= cosf(alpha) * sqrtf(abs(overlap));
			ball[idx].y -= sinf(alpha) * sqrtf(abs(overlap));
		}
		setBounceVel(idx, collIdx);
	}

	//collision with box
	if (ball[idx].y <= ball[idx].r){
		ball[idx].y = ball[idx].r;
		ball[idx].vy = -DUMP*ball[idx].vy;
	}
	if (ball[idx].y >= YBOX - ball[idx].r) {
		ball[idx].y = YBOX - ball[idx].r;
		ball[idx].vy = -DUMP*ball[idx].vy;
	}
	if (ball[idx].x >= XBOX - ball[idx].r) {
		ball[idx].x = XBOX - ball[idx].r;
		ball[idx].vx = -DUMP*ball[idx].vx;
	}
	if (ball[idx].x <= ball[idx].r) {
		ball[idx].x = ball[idx].r;
		ball[idx].vx = -DUMP*ball[idx].vx;
	}
}

//initBall initializes randomly a ball//
void	initBall(int idx){
	ball[idx].c = floor(frand(0, 255));
	ball[idx].r = frand(RMIN, RMAX);
	ball[idx].m = ball[idx].r / 50;	//size matches mass
	ball[idx].x = frand(ball[idx].r+1, XBOX-ball[idx].r-1);
	ball[idx].y = frand(ball[idx].r+1, YBOX-ball[idx].r-1);
	ball[idx].vx = frand(VMIN, VMAX);
	ball[idx].vy = frand(VMIN, VMAX);
}

//balltask//
void	*ballTask(void* arg){
	int	idx;

	idx = get_task_index(arg);
	set_activation(idx);
	initBall(idx);
	while (!end) {
		ball[idx].vy += g*dt;
		ball[idx].x += ball[idx].vx*dt;
		ball[idx].y += ball[idx].vy*dt;
		handleBounce(idx);
		storeTrail(idx);
		deadline_miss(idx);
		wait_for_activation(idx);
	}
	return	NULL;
}

//display task displays dynamic graphics//
void *displayTask(void *arg){
	int	idx;
	int	i;
	BITMAP	*copy;	// proxy screen, everything dynamic is drawn on it

	idx = get_task_index(arg);
	set_activation(idx);
	copy = create_bitmap(XBOX, YBOX);	// proxy screen initialization
	while (!end) {
		rectfill(copy, 0, 0, XBOX, YBOX, BCOL);			
		for (i=0; i<nab; i++) {
			drawBall(i, copy);
			if (tflag) drawTrail(i, tlen, copy);
		}
		zoom(copy);	// proxy screen copied on the real one
		drawInfo();
		deadline_miss(idx);
		wait_for_activation(idx);
	}
	return	NULL;
}

//user task implements user commands//
void *userTask(void *arg){
	int	idx;

	idx = get_task_index(arg);
	set_activation(idx);
	while (!end) {
		command();
		deadline_miss(idx);
		wait_for_activation(idx);
	}
	return	NULL;
}

//init displays static graphics and initializes allegro//
void	init(void){
	allegro_init();
	set_color_depth(8);
	srand(time(0));
	set_gfx_mode(GFX_AUTODETECT_WINDOWED,XWIN, YWIN, BKG, BKG);
	clear_to_color(screen,0);
	install_keyboard();
	rectfill(screen, XBOX, (YBOX/2) + 1, XWIN, YBOX, MCOL);
	textout(screen, font, "PRESS:", 880, (YBOX/2) + 30, TXTCOL);
	textout(screen, font, "ENTER TO CREATE A NEW BALL", 930, (YBOX/2) + 50, TXTCOL);
	textout(screen, font, "T TO TOGGLE TRAILS", 930, (YBOX/2) + 70, TXTCOL);
	textout(screen, font, "->/<- TO INCREASE/DECREASE TRAIL LENGTH", 930, (YBOX/2) + 90, TXTCOL);
	textout(screen, font, "UP/DOWN TO INCREASE/DECREASE GRAVITY", 930, (YBOX/2) + 110, TXTCOL);
	textout(screen, font, "SPACE TO BOOST", 930, (YBOX/2) + 130, TXTCOL);
	textout(screen, font, "I/O TO ZOOM IN/OUT", 930, (YBOX/2) + 150, TXTCOL);
	textout(screen, font, "ESC TO EXIT", 930, (YBOX/2) + 170, TXTCOL);
}

//zoom copies the proxy screen "scr" on the real one, zooming if requested//
void	zoom(BITMAP	*copy){
	int	zoom = zoomLvl;

	if (zoom == 1){
		blit(copy, screen, 0, 0, 0, 0, XBOX, YBOX);	// no alteration
	}
	if (zoom == 2){
		stretch_blit(copy, screen, XBOX/4, YBOX/4, 2*XBOX/4, 2*YBOX/4, 0, 0, XBOX, YBOX);
		}
	if (zoom == 3){
		stretch_blit(copy, screen, XBOX/3, YBOX/3, XBOX/3, YBOX/3, 0, 0, XBOX, YBOX);
	}
}