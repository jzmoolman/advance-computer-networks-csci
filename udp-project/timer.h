#ifndef __ZCP_TIMER__
#define __ZCP_TIMER__

#include <pthread.h>

pthread_t tid;

int timer_started = 0;
int timeout_signal = 0;
int stop_timer_signal = 0;

void start_timer(int time);
void stop_timer();


#endif
