#ifndef X_TIMER_H
#define X_TIMER_H
#include <stdlib.h>

typedef enum
{
TIMER_SINGLE_SHOT = 0,
TIMER_PERIODIC
} t_timer;

typedef struct{
	pthread_t p_id;
	void * list_head;
	/* pair of sockets for event loop control, Reader and Writer */
    int controlFdR;
    int controlFdW;
}tXtimerManager;
typedef void (*time_handler)(void * timer_id, void * user_data);

tXtimerManager* x_timer_initialize();
void * x_timer_add(tXtimerManager * timer_mag, unsigned int interval, time_handler handler, t_timer type, void * user_data, int active);
void x_timer_set_state(tXtimerManager * timer_mag, void * t_id, int active);
void x_timer_del(tXtimerManager * timer_mag, void * t_id);
void x_timer_finalize(tXtimerManager * timer_mag);

#endif
