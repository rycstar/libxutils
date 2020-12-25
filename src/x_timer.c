#include <stdint.h>
#include <string.h>
#include <sys/timerfd.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <poll.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> 
#include "x_timer.h"

#define MAX_TIMER_COUNT 128

typedef enum{
	X_TIMER_UNUSED = 0,
	X_TIMER_ADD,
	X_TIMER_DEL,
	X_TIMER_ENABLE,
	X_TIMER_DISABLE,
	X_TIMER_DEL_ALL,
	X_TIMER_INVALID
}eTimerEvt;

struct timer_node
{
    int                 fd;
    time_handler        callback;
    void *              user_data;
    unsigned int        interval;
    t_timer             type;
	int 				active;
    struct timer_node * next;
};

static void _timer_set_time(int fd, unsigned int interval, t_timer type){
	struct itimerspec new_value;
	new_value.it_value.tv_sec = interval / 1000;
    new_value.it_value.tv_nsec = (interval % 1000)* 1000000;

    if (type == TIMER_PERIODIC)
    {
      new_value.it_interval.tv_sec= interval / 1000;
      new_value.it_interval.tv_nsec = (interval %1000) * 1000000;
    }
    else
    {
      new_value.it_interval.tv_sec= 0;
      new_value.it_interval.tv_nsec = 0;
    }

    timerfd_settime(fd, 0, &new_value, NULL);
}

static int _timer_evt_send(int fd, eTimerEvt evt, size_t code){
	size_t evt_packet[2];
	evt_packet[0] = evt;
	evt_packet[1] = code;
	if(fd > 0)
		return write(fd, evt_packet, sizeof(evt_packet));
	return 0;
}
static void * _timer_thread(void * data);

tXtimerManager* x_timer_initialize()
{
	tXtimerManager * new_timer_manager = (tXtimerManager * )malloc(sizeof(tXtimerManager));
	
	if(!new_timer_manager) return NULL;
	
	memset(new_timer_manager, 0, sizeof(tXtimerManager));
	
	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, &(new_timer_manager->controlFdR))){
		goto fail;
	}
	
    if(pthread_create(&(new_timer_manager->p_id), NULL, _timer_thread, new_timer_manager))
    {
        goto fail;
    }

    return new_timer_manager;
fail:
	if(new_timer_manager){
		if(new_timer_manager->controlFdR > 0) close(new_timer_manager->controlFdR);
		if(new_timer_manager->controlFdW > 0) close(new_timer_manager->controlFdW);
		free(new_timer_manager);
	}
	return NULL;
}

void * x_timer_add(tXtimerManager * timer_mag, unsigned int interval, time_handler handler, t_timer type, void * user_data, int active){
	struct timer_node * new_node = NULL;
    

    new_node = (struct timer_node *)malloc(sizeof(struct timer_node));

    if(!new_node || ! timer_mag) return NULL;
	memset(new_node, 0, sizeof(struct timer_node));
	new_node->callback  = handler;
    new_node->user_data = user_data;
    new_node->interval  = interval;
    new_node->type      = type;
	new_node->active    = active;
	
	new_node->fd = timerfd_create(CLOCK_MONOTONIC, O_CLOEXEC);
	
	if (new_node->fd == -1)
    {
        free(new_node);
        return NULL;
    }
	
	if(active){
		_timer_set_time(new_node->fd, new_node->interval, new_node->type);
	}else{
		_timer_set_time(new_node->fd, 0, new_node->type);
	}
	
	_timer_evt_send(timer_mag->controlFdW, X_TIMER_ADD, (size_t)new_node);

    return (void *)new_node;
}

void x_timer_set_state(tXtimerManager * timer_mag, void * t_id, int active){
	int fd = timer_mag->controlFdW;
	if(active){
		_timer_evt_send(fd, X_TIMER_ENABLE, (size_t)t_id);
	}else{
		_timer_evt_send(fd, X_TIMER_DISABLE, (size_t)t_id);
	}
}

void x_timer_del(tXtimerManager * timer_mag, void * t_id)
{
	int fd = timer_mag->controlFdW;
	
	_timer_evt_send(fd, X_TIMER_DEL, (size_t)t_id);
}

void x_timer_finalize(tXtimerManager * timer_mag)
{
	if(!timer_mag) return;
	
	_timer_evt_send(timer_mag->controlFdW, X_TIMER_DEL_ALL, (size_t)0);
	
    pthread_cancel(timer_mag->p_id);
    pthread_join(timer_mag->p_id, NULL);
	
	if(timer_mag->controlFdR > 0) close(timer_mag->controlFdR);
	if(timer_mag->controlFdW > 0) close(timer_mag->controlFdW);
	free(timer_mag);
}

static void _timer_control_handle(tXtimerManager * timer_mag, size_t * msg){
	struct timer_node * tmp = timer_mag->list_head, *node = NULL;
	
	node = (struct timer_node *)msg[1];
	switch(msg[0]){
		case 	X_TIMER_ADD:
			if(node){
				node->next = timer_mag->list_head;
				timer_mag->list_head = node;
			}
			break;
		case    X_TIMER_DEL:
			if(node){
				if(node == tmp){
					timer_mag->list_head = (void *)tmp->next;
				}else{
					while(tmp && tmp->next != node) tmp = tmp->next;
					if(tmp) tmp->next = tmp->next->next;
				}
				close(node->fd);
				free(node);
			}
			break;
		case    X_TIMER_ENABLE:
			/*should find the node in the list first*/
			_timer_set_time(node->fd, node->interval, node->type);
			node->active = 1;
			break;
	    case    X_TIMER_DISABLE:
			/*should find the node in the list first*/
			_timer_set_time(node->fd, 0, node->type);
			node->active = 0;
			break;
	    case    X_TIMER_DEL_ALL:
			while(tmp){
				node = tmp;
				tmp = tmp->next;
				close(node->fd);
				free(node);
			}
			break;
		default:
			printf("Warning: we should not receive this msg\n");
			break;
	}
	
}

void * _timer_thread(void * data)
{
    struct pollfd ufds[MAX_TIMER_COUNT] = {{0}};
	struct timer_node * timer_node[MAX_TIMER_COUNT] = {0};
    int iMaxCount = 0;
    struct timer_node * tmp = NULL;
    int read_fds = 0, i, s;
    uint64_t exp;
	size_t evt_packet[2];
	
	tXtimerManager * timer_mag = (tXtimerManager *)data;
	prctl(PR_SET_NAME, "timer_manager");
	
    while(1)
    {
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_testcancel();
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

        iMaxCount = 0;
        memset(ufds, 0, sizeof(struct pollfd)*MAX_TIMER_COUNT);
		memset(timer_node, 0, sizeof(struct timer_node *) *MAX_TIMER_COUNT);
		
		ufds[0].fd = timer_mag->controlFdR;
		ufds[0].events = POLLIN;
		iMaxCount++;
		
		tmp = timer_mag->list_head;
        while(tmp && iMaxCount < MAX_TIMER_COUNT)
        {
			ufds[iMaxCount].fd = tmp->fd;
			ufds[iMaxCount].events = POLLIN;
			timer_node[iMaxCount] = tmp;
			iMaxCount++;
			
            tmp = tmp->next;
        }
        read_fds = poll(ufds, iMaxCount, 20);

        if (read_fds <= 0) continue;
		
		/*control msg handle*/
		if(ufds[0].revents & POLLIN){
			s = read(ufds[0].fd, evt_packet, sizeof(evt_packet));
			if (s == sizeof(evt_packet)){
				_timer_control_handle(timer_mag, evt_packet);
			}
		}
		
        for (i = 1; i < iMaxCount; i++)
        {
            if (ufds[i].revents & POLLIN)
            {
                s = read(ufds[i].fd, &exp, sizeof(uint64_t));

                if (s != sizeof(uint64_t)) continue;

                tmp = timer_node[i];

                if(tmp && tmp->active && tmp->callback) tmp->callback((void*)tmp, tmp->user_data);
            }
        }
    }

    return NULL;
}
