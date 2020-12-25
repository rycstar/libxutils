#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "x_timer.h"

static void vsync_cb(void * timer_id, void * userdata){
	printf("timer:%p,vsync msg send\n", timer_id);
}
static void blink_cb(void * timer_id, void * userdata){
	printf("timer:%p,blink msg send\n", timer_id);
}

int main(int argc, char * argv[]){
    char cmd[64] = {0};
    int op = 0;
    void * timer_vsync, * timer_blink, *timer_tmp = NULL;
    tXtimerManager *timer_mag = x_timer_initialize();

    if(!timer_mag){
	printf("timer init error. quit!\n");
	return -1;
    }
    
    timer_vsync = x_timer_add(timer_mag, 1000, vsync_cb,TIMER_PERIODIC, NULL, 0);
    timer_blink = x_timer_add(timer_mag, 5000, blink_cb,TIMER_SINGLE_SHOT, NULL, 0);
    while(1){
	op = 0;
	timer_tmp = NULL;
	memset(cmd,0,sizeof(cmd));
	printf("Input CMD:\n");
	gets(cmd);
	printf("get cmd:%s\n",cmd);
	if(strstr(cmd,"quit")) break;
	if(strstr(cmd, "start")) op = 0x01;
	else if(strstr(cmd, "stop")) op = 0x02;
	else if(strstr(cmd, "delete")) op = 0x04;
	if(strstr(cmd, "vsync")) timer_tmp = timer_vsync; 
	if(strstr(cmd, "blink")) timer_tmp = timer_blink; 
	if(timer_tmp){
		if(op == 0x01)
			x_timer_set_state(timer_mag, timer_tmp, 1);
		else if(op == 0x02)
			x_timer_set_state(timer_mag, timer_tmp, 0);
		else if(op == 0x04)
			x_timer_del(timer_mag, timer_tmp);
	}
	
    }
    printf("----end timer----\n");
    x_timer_finalize(timer_mag);
    printf("----end----\n");
    return 0;
}
