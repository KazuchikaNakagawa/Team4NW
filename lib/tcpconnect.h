#ifndef TCP_CONNECT_H
#define TCP_CONNECT_H

struct sockaddr_in;
extern int tcpconnect(struct sockaddr_in* serverAddr, const char* server_ipaddr_str, int port);

#endif /* TCP_CONNECT_H */