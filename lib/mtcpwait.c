#include "mtcpwait.h"

#define MAX_EVENTS 10

int epoll_ctl_add_in(int epfd, int fd)
{
    struct epoll_event ev; /* イベント */

    memset(&ev, 0, sizeof(ev)); /* 0クリア */
    ev.events = EPOLLIN; /* read()可能というイベント */
    ev.data.fd = fd; /* 関連付けるfd */
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        perror("epoll_ctl");
        return 1;
    }
    return 0;
}

int TCPManagerInit(TCPManager* manager, int timeout)
{
    struct sockaddr_in serverAddr;
    manager->fd = epoll_create(MAX_EVENTS);
    if (manager->fd < 0) {
        perror("epoll_create");
        return -1;
    }

    manager->timeout = timeout;
    if ((manager->waitsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socke");
        return -1;
    }

    int yes = 1;
    setsockopt(manager->waitsock, SOL_SOCKET, SO_REUSEADDR,
        (const char*)&yes, sizeof(yes));

    memset(&serverAddr, 0, sizeof(serverAddr)); /* ゼロクリア */
    serverAddr.sin_family = AF_INET; /* Internetプロトコル */
    serverAddr.sin_port = htons(TCP_SERVER_PORT); /* 待ち受けるポート */
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* どのIPアドレス宛でも */

    if (bind(manager->waitsock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(manager->waitsock, 5) != 0) {
        perror("listen");
        return -1;
    }

    TCPManagerAdd(manager, manager->waitsock);
}

int TCPManagerAdd(TCPManager* manager, int fd)
{
    return epoll_ctl_add_in(manager->fd, fd);
}

int TCPManagerMainloop(TCPManager* manager)
{
    int nfds = epoll_wait(manager->fd, &manager->events, MAX_EVENTS, manager->timeout);
    if (nfds < 0) {
        perror("epoll_wait");
        return -1;
    }

    for (int i = 0; i < nfds; i++) {
        if (manager->events[i].data.fd == manager->waitsock) {
            struct sockaddr_in clientAddr;
            struct in_addr addr;
            int addrLen;
            int sock = accept(manager->waitsock, (struct sockaddr*)&clientAddr, (socklen_t*)&addrLen);
            if (sock < 0) {
                perror("accept");
                return -1;
            }

            addr.s_addr = clientAddr.sin_addr.s_addr;
            printf("accepted:  ip address: %s, ", inet_ntoa(addr));
            printf("port#: %d\n", ntohs(clientAddr.sin_port));

            TCPManagerAdd(manager, sock);
        }
    }

    return nfds;
}
