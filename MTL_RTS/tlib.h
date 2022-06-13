#ifndef TLIB_H_
#define TLIB_H_

/*--------------------------------------------------------------------------------------------------------*/
/*  Function time_copy(*td, ts) copies a source time variable ts in a destination variable pointed by td  */
/*--------------------------------------------------------------------------------------------------------*/
void time_add_ms(struct timespec *t, int ms);

/*--------------------------------------------------------------------------------------------------*/
/*  Function time_add_ms(*t, ms) adds a value ms (in milliseconds) to a time variable pointed by t  */
/*--------------------------------------------------------------------------------------------------*/
void time_copy(struct timespec *td, struct timespec ts);

/*------------------------------------------------------------------------------------------------*/
/*  Function time_cmp(t1, t2) compares time variables t1 and t2 and returns 0 if they are equal,  */
/*  otherwise 1 if t1>t2, ‐1 if t1<t2                                                             */
/*------------------------------------------------------------------------------------------------*/
int  time_cmp(struct timespec t1, struct timespec t2);

#endif /* TLIB_H_ */
