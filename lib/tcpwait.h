#ifndef TCP_WAIT_H
#define TCP_WAIT_H

struct sockaddr_in;
extern int waitpeer(struct sockaddr_in* serverAddr);

#endif /* TCP_WAIT_H */