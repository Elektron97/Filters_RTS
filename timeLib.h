#ifndef TLIB_H_
#define TLIB_H_

void time_copy(struct timespec *td, struct timespec ts);
void time_add_ms(struct timespec *t, int ms);
int time_cmp(struct timespec t1, struct timespec t2);

#endif /* TLIB_H_ */