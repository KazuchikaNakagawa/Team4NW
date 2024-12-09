#include "lib/mtcpwait.h"
#include "lib/tcpconnect.h"
#include "utils/icslab2_net.h"

#define MAX_USERS 10

int main(int argc, char** argv)
{
    int fds[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int members = 0;
    struct sockaddr_in serverAddr;
    TCPManager manager;
    if (TCPManagerInit(&manager, 1000) < 0) {
        return -1;
    }
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <mirror-server-address>\n", argv[0]);
        return -1;
    }
    const char* server_ipaddr_str = argv[1];
    char buf[BUF_LEN];
    int nfds;

    while (1) {
        if ((nfds = TCPManagerMainloop(&manager)) < 0) {
            return -1;
        }

        for (int i = 0; i < nfds; i++) {
            if (manager.events[i].data.fd == manager.waitsock) {
                struct sockaddr_in clientAddr;
                struct in_addr addr;
                int addrLen;
                int clientsock = accept(manager.waitsock, (struct sockaddr*)&clientAddr, (socklen_t*)&addrLen);
                if (clientsock < 0) {
                    perror("accept");
                    return -1;
                }

                addr.s_addr = clientAddr.sin_addr.s_addr;
                printf("accepted:  ip address: %s, ", inet_ntoa(addr));
                printf("port#: %d\n", ntohs(clientAddr.sin_port));

                TCPManagerAdd(&manager, clientsock);
                fds[members++] = clientsock;
                continue;
            }

            // on data
            int clientsock = manager.events[i].data.fd;
            if (argc < 2) {
                fprintf(stderr, "Usage: %s <mirror-server-address>\n", argv[0]);
                return -1;
            }

            int n;
            if ((n = read(clientsock, buf, BUF_LEN)) < 0) {
                close(clientsock);
                break;
            }
            buf[n] = '\0';
            printf("recv req: %s\n", buf);

            printf("connecting to %s\n", server_ipaddr_str);
            int serversock = tcpconnect(&serverAddr, server_ipaddr_str, TCP_SERVER_PORT);
            if (serversock < 0) {
                perror("tcpconnect");
                return -1;
            }
            write(serversock, buf, n);

            while ((n = read(serversock, buf, BUF_LEN)) > 0) {
                TCPManagerMainloop(&manager);
                int size = n / members;
                for (int i = 0; i < members; i++) {
                    write(fds[i], buf + i * size, size);
                }
            }
            close(serversock);
            for (int i = 0; i < members; i++) {
                close(fds[i]);
            }
            printf("closed\n");
        }
    }
}