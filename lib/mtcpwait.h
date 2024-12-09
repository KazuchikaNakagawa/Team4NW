#ifndef MTCPWAIT_H
#define MTCPWAIT_H

#include "utils/icslab2_net.h"
#include <stdio.h>
#include <sys/epoll.h>

typedef struct {
    int fd;
    int timeout;
    int waitsock;
    struct epoll_event events[MAX_EVENTS];
} TCPManager;

#endif /* MTCPWAIT_H */