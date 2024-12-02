#include "utils/icslab2_net.h"
#include <netinet/in.h>

int tcpconnect(struct sockaddr_in* serverAddr, const char* server_ipaddr_str, int port)
{
    int sock;
    memset(serverAddr, 0, sizeof(*serverAddr)); /* 0クリア */
    serverAddr->sin_family = AF_INET; /* Internetプロトコル */
    serverAddr->sin_port = htons(port); /* サーバの待受ポート */
    /* IPアドレス（文字列）から変換 */
    inet_pton(AF_INET, server_ipaddr_str, &serverAddr->sin_addr.s_addr);

    /* 確認用：IPアドレスを文字列に変換して表示 */
    struct in_addr addr;
    addr.s_addr = serverAddr->sin_addr.s_addr;
    printf("ip address: %s\n", inet_ntoa(addr));
    printf("port#: %d\n", ntohs(serverAddr->sin_port));

    /* STEP 2: TCPソケットをオープン */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    /* ここで、ローカルでsocketをbind()してもよいが省略 */

    /* STEP 3: サーバに接続（bind相当も実行） */
    if (connect(sock, (struct sockaddr*)serverAddr, sizeof(*serverAddr)) < 0) {
        perror("connect");
        return -1;
    }

    return sock;
}