

#ifndef PLEXY_EVENT_LOOP_H
#define PLEXY_EVENT_LOOP_H

#include "plexy.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PlexyEventLoop PlexyEventLoop;

typedef int (*PlexyTimerCallback)(void* userdata);

typedef int (*PlexyIdleCallback)(void* userdata);

typedef int (*PlexyFdCallback)(int fd, void* userdata);

PlexyEventLoop* plexy_event_loop_create(PlexyConnection* conn);

void plexy_event_loop_destroy(PlexyEventLoop* loop);

uint32_t plexy_event_loop_add_timer(PlexyEventLoop* loop, 
                                     int interval_ms,
                                     PlexyTimerCallback callback,
                                     void* userdata);

void plexy_event_loop_remove_timer(PlexyEventLoop* loop, uint32_t timer_id);

uint32_t plexy_event_loop_add_idle(PlexyEventLoop* loop,
                                    PlexyIdleCallback callback,
                                    void* userdata);

void plexy_event_loop_remove_idle(PlexyEventLoop* loop, uint32_t idle_id);

uint32_t plexy_event_loop_add_fd(PlexyEventLoop* loop,
                                  int fd,
                                  PlexyFdCallback callback,
                                  void* userdata);

void plexy_event_loop_remove_fd(PlexyEventLoop* loop, uint32_t fd_id);

int plexy_event_loop_run(PlexyEventLoop* loop);

void plexy_event_loop_quit(PlexyEventLoop* loop);

int plexy_event_loop_is_running(PlexyEventLoop* loop);

#ifdef __cplusplus
}
#endif

#endif 
