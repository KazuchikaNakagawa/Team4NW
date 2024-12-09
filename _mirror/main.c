#include "lib/tcpconnect.h"
#include "lib/tcpwait.h"
#include "utils/icslab2_net.h"

int main(int argc, char** argv)
{
    struct sockaddr_in clientAddr;
    struct sockaddr_in serverAddr;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <mirror-server-address>\n", argv[0]);
        return -1;
    }
    const char* server_ipaddr_str = argv[1];
    char buf[BUF_LEN];

    while (1) {
        int clientsock = waitpeer(0, &clientAddr);
        if (clientsock < 0) {
            perror("waitpeer");
            return -1;
        }

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
            write(clientsock, buf, n);
            // write(0, buf, n);
        }
        close(serversock);
        close(clientsock);
        printf("closed\n");
    }
}