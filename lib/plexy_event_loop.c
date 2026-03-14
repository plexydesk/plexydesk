

#define _POSIX_C_SOURCE 200809L

#include "../include/plexy_event_loop.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

#define MAX_TIMERS 32
#define MAX_IDLES 16
#define MAX_FDS 16

typedef struct {
    uint32_t id;
    int interval_ms;
    PlexyTimerCallback callback;
    void* userdata;
    uint64_t next_fire_ms;
    int active;
} Timer;

typedef struct {
    uint32_t id;
    PlexyIdleCallback callback;
    void* userdata;
    int active;
} Idle;

typedef struct {
    uint32_t id;
    int fd;
    PlexyFdCallback callback;
    void* userdata;
    int active;
} FdWatch;

struct PlexyEventLoop {
    PlexyConnection* conn;
    int conn_fd;
    int running;
    
    Timer timers[MAX_TIMERS];
    int num_timers;
    uint32_t next_timer_id;
    
    Idle idles[MAX_IDLES];
    int num_idles;
    uint32_t next_idle_id;
    
    FdWatch fds[MAX_FDS];
    int num_fds;
    uint32_t next_fd_id;
};

static uint64_t get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

PlexyEventLoop* plexy_event_loop_create(PlexyConnection* conn) {
    if (!conn) return NULL;
    
    PlexyEventLoop* loop = calloc(1, sizeof(PlexyEventLoop));
    if (!loop) return NULL;
    
    loop->conn = conn;
    loop->conn_fd = plexy_get_fd(conn);
    loop->running = 0;
    loop->next_timer_id = 1;
    loop->next_idle_id = 1;
    loop->next_fd_id = 1;
    
    if (loop->conn_fd < 0) {
        free(loop);
        return NULL;
    }
    
    return loop;
}

void plexy_event_loop_destroy(PlexyEventLoop* loop) {
    if (!loop) return;
    free(loop);
}

uint32_t plexy_event_loop_add_timer(PlexyEventLoop* loop, 
                                     int interval_ms,
                                     PlexyTimerCallback callback,
                                     void* userdata) {
    if (!loop || !callback || interval_ms <= 0) return 0;
    if (loop->num_timers >= MAX_TIMERS) return 0;
    
    uint32_t id = loop->next_timer_id++;
    uint64_t now = get_time_ms();
    
    Timer* timer = &loop->timers[loop->num_timers++];
    timer->id = id;
    timer->interval_ms = interval_ms;
    timer->callback = callback;
    timer->userdata = userdata;
    timer->next_fire_ms = now + interval_ms;
    timer->active = 1;
    
    return id;
}

void plexy_event_loop_remove_timer(PlexyEventLoop* loop, uint32_t timer_id) {
    if (!loop) return;
    
    for (int i = 0; i < loop->num_timers; i++) {
        if (loop->timers[i].id == timer_id) {
            loop->timers[i].active = 0;
            
            memmove(&loop->timers[i], &loop->timers[i + 1],
                    (loop->num_timers - i - 1) * sizeof(Timer));
            loop->num_timers--;
            return;
        }
    }
}

uint32_t plexy_event_loop_add_idle(PlexyEventLoop* loop,
                                    PlexyIdleCallback callback,
                                    void* userdata) {
    if (!loop || !callback) return 0;
    if (loop->num_idles >= MAX_IDLES) return 0;
    
    uint32_t id = loop->next_idle_id++;
    
    Idle* idle = &loop->idles[loop->num_idles++];
    idle->id = id;
    idle->callback = callback;
    idle->userdata = userdata;
    idle->active = 1;
    
    return id;
}

void plexy_event_loop_remove_idle(PlexyEventLoop* loop, uint32_t idle_id) {
    if (!loop) return;
    
    for (int i = 0; i < loop->num_idles; i++) {
        if (loop->idles[i].id == idle_id) {
            loop->idles[i].active = 0;
            
            memmove(&loop->idles[i], &loop->idles[i + 1],
                    (loop->num_idles - i - 1) * sizeof(Idle));
            loop->num_idles--;
            return;
        }
    }
}

uint32_t plexy_event_loop_add_fd(PlexyEventLoop* loop,
                                  int fd,
                                  PlexyFdCallback callback,
                                  void* userdata) {
    if (!loop || !callback || fd < 0) return 0;
    if (loop->num_fds >= MAX_FDS) return 0;
    
    uint32_t id = loop->next_fd_id++;
    
    FdWatch* watch = &loop->fds[loop->num_fds++];
    watch->id = id;
    watch->fd = fd;
    watch->callback = callback;
    watch->userdata = userdata;
    watch->active = 1;
    
    return id;
}

void plexy_event_loop_remove_fd(PlexyEventLoop* loop, uint32_t fd_id) {
    if (!loop) return;
    
    for (int i = 0; i < loop->num_fds; i++) {
        if (loop->fds[i].id == fd_id) {
            loop->fds[i].active = 0;
            
            memmove(&loop->fds[i], &loop->fds[i + 1],
                    (loop->num_fds - i - 1) * sizeof(FdWatch));
            loop->num_fds--;
            return;
        }
    }
}

void plexy_event_loop_quit(PlexyEventLoop* loop) {
    if (loop) {
        loop->running = 0;
    }
}

int plexy_event_loop_is_running(PlexyEventLoop* loop) {
    return loop ? loop->running : 0;
}

int plexy_event_loop_run(PlexyEventLoop* loop) {
    if (!loop) return -1;
    
    loop->running = 1;
    
    while (loop->running) {
        uint64_t now = get_time_ms();
        int timeout_ms = -1;  
        
        
        for (int i = 0; i < loop->num_timers; i++) {
            Timer* timer = &loop->timers[i];
            if (!timer->active) continue;
            
            int64_t time_until = timer->next_fire_ms - now;
            if (time_until < 0) time_until = 0;
            
            if (timeout_ms < 0 || time_until < timeout_ms) {
                timeout_ms = (int)time_until;
            }
        }
        
        
        if (loop->num_idles > 0 && timeout_ms < 0) {
            timeout_ms = 0;
        }
        
        
        struct pollfd pfds[1 + MAX_FDS];
        int nfds = 0;
        
        
        pfds[nfds].fd = loop->conn_fd;
        pfds[nfds].events = POLLIN;
        pfds[nfds].revents = 0;
        nfds++;
        
        
        for (int i = 0; i < loop->num_fds; i++) {
            if (!loop->fds[i].active) continue;
            pfds[nfds].fd = loop->fds[i].fd;
            pfds[nfds].events = POLLIN;
            pfds[nfds].revents = 0;
            nfds++;
        }
        
        
        int ret = poll(pfds, nfds, timeout_ms);
        
        if (ret < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        
        
        if (pfds[0].revents & POLLIN) {
            
            int flags = fcntl(loop->conn_fd, F_GETFL, 0);
            fcntl(loop->conn_fd, F_SETFL, flags | O_NONBLOCK);
            int dispatch_ret;
            while ((dispatch_ret = plexy_dispatch(loop->conn)) > 0)
                ;
            fcntl(loop->conn_fd, F_SETFL, flags);
            if (dispatch_ret < 0) {
                loop->running = 0;
                return -1;
            }
        }
        
        if (pfds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            loop->running = 0;
            return -1;
        }
        
        
        for (int i = 1; i < nfds; i++) {
            if (pfds[i].revents & POLLIN) {
                
                for (int j = 0; j < loop->num_fds; j++) {
                    if (loop->fds[j].active && loop->fds[j].fd == pfds[i].fd) {
                        int keep = loop->fds[j].callback(pfds[i].fd, loop->fds[j].userdata);
                        if (!keep) {
                            plexy_event_loop_remove_fd(loop, loop->fds[j].id);
                        }
                        break;
                    }
                }
            }
        }
        
        
        now = get_time_ms();
        for (int i = 0; i < loop->num_timers; i++) {
            Timer* timer = &loop->timers[i];
            if (!timer->active) continue;
            
            if (now >= timer->next_fire_ms) {
                int keep = timer->callback(timer->userdata);
                if (keep) {
                    
                    timer->next_fire_ms = now + timer->interval_ms;
                } else {
                    
                    plexy_event_loop_remove_timer(loop, timer->id);
                    i--;  
                }
            }
        }
        
        
        if (ret == 0 && loop->num_idles > 0) {
            for (int i = 0; i < loop->num_idles; i++) {
                if (!loop->idles[i].active) continue;
                
                int keep = loop->idles[i].callback(loop->idles[i].userdata);
                if (!keep) {
                    plexy_event_loop_remove_idle(loop, loop->idles[i].id);
                    i--;  
                }
            }
        }
    }
    
    return 0;
}
