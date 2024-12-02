#include "utils/icslab2_net.h"
#include <netinet/in.h>
#include <sys/socket.h>

/*
    接続先が見つかるまでブロックします。

*/

int waitpeer(int waitsock, struct sockaddr_in* serverAddr)
{
    int sock0 = waitsock;
    int yes = 1;
    struct sockaddr_in clientAddr;
    struct in_addr addr;
    int addrLen;
    int sock;

    /* sock0のコネクションがTIME_WAIT状態でもbind()できるように設定 */
    setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR,
        (const char*)&yes, sizeof(yes));

    /* STEP 2: クライアントからの要求を受け付けるIPアドレスとポートを設定する */
    memset(serverAddr, 0, sizeof(*serverAddr)); /* ゼロクリア */
    serverAddr->sin_family = AF_INET; /* Internetプロトコル */
    serverAddr->sin_port = htons(TCP_SERVER_PORT); /* 待ち受けるポート */
    serverAddr->sin_addr.s_addr = htonl(INADDR_ANY); /* どのIPアドレス宛でも */

    /* STEP 3: ソケットとアドレスをbindする */
    if (bind(sock0, (struct sockaddr*)&serverAddr, sizeof(*serverAddr)) < 0) {
        perror("bind");
        return -1;
    }

    /* STEP 4: コネクションの最大同時受け入れ数を指定する */
    if (listen(sock0, 5) != 0) {
        perror("listen");
        return -1;
    }

    while (1) { /* 終了フラグが0の間は繰り返す */

        /* STEP 5: クライアントからの接続要求を受け付ける */
        printf("waiting connection...\n");
        addrLen = sizeof(clientAddr);
        sock = accept(sock0, (struct sockaddr*)&clientAddr, (socklen_t*)&addrLen);
        if (sock < 0) {
            perror("accept");
            return -1;
        }

        /* 受信パケットの送信元IPアドレスとポート番号を表示 */
        addr.s_addr = clientAddr.sin_addr.s_addr;
        printf("accepted:  ip address: %s, ", inet_ntoa(addr));
        printf("port#: %d\n", ntohs(clientAddr.sin_port));

        return sock;
    }
}