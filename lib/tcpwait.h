#ifndef TCP_WAIT_H
#define TCP_WAIT_H

struct sockaddr_in;
extern int waitpeer(int waitsock, struct sockaddr_in* serverAddr);

#endif /* TCP_WAIT_H */