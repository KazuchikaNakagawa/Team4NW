#include "utils/icslab2_net.h"

static char* get_server_addr(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server_ipaddr>\n", argv[0]);
        exit(1);
    }
    return argv[1];
}

int main(int argc, char** argv)
{
    int sock0; /* 待ち受け用ソケットディスクリプタ */
    int sock; /* ソケットディスクリプタ */
    struct sockaddr_in myAddr; /* サーバ＝自分用アドレス構造体 */
    struct sockaddr_in serverAddr; /* サーバ＝相手用アドレス構造体 */
    struct sockaddr_in clientAddr; /* クライアント＝相手用アドレス構造体 */
    int addrLen; /* clientAddrのサイズ */

    char buf[BUF_LEN]; /* 受信バッファ */
    int n; /* 受信バイト数 */
    int isEnd = 0; /* 終了フラグ，0でなければ終了 */

    int fd; /* ファイルデスクリプタ */

    int yes = 1; /* setsockopt()用 */

    /* STEP 1: TCPソケットをオープンする */
    if ((sock0 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socke");
        return 1;
    }

    /* sock0のコネクションがTIME_WAIT状態でもbind()できるように設定 */
    setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR,
        (const char*)&yes, sizeof(yes));

    /* STEP 2: クライアントからの要求を受け付けるIPアドレスとポートを設定する */
    memset(&myAddr, 0, sizeof(myAddr)); /* ゼロクリア */
    myAddr.sin_family = AF_INET; /* Internetプロトコル */
    myAddr.sin_port = htons(TCP_SERVER_PORT); /* 待ち受けるポート */
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* どのIPアドレス宛でも */

    /* STEP 3: ソケットとアドレスをbindする */
    if (bind(sock0, (struct sockaddr*)&myAddr, sizeof(myAddr)) < 0) {
        perror("bind");
        return 1;
    }

    /* STEP 4: コネクションの最大同時受け入れ数を指定する */
    if (listen(sock0, 5) != 0) {
        perror("listen");
        return 1;
    }

    /* --receiving-- done */

    struct in_addr addr;
    int send_sock;
    char* server_ipaddr_str = get_server_addr(argc, argv);

    /* STEP 1: 宛先サーバのIPアドレスとポートを指定 */
    memset(&serverAddr, 0, sizeof(serverAddr)); /* 0クリア */
    serverAddr.sin_family = AF_INET; /* Internetプロトコル */
    serverAddr.sin_port = htons(TCP_SERVER_PORT); /* サーバの待受ポート */
    /* IPアドレス（文字列）から変換 */
    inet_pton(AF_INET, server_ipaddr_str, &serverAddr.sin_addr.s_addr);

    /* 確認用：IPアドレスを文字列に変換して表示 */
    addr.s_addr = serverAddr.sin_addr.s_addr;
    printf("ip address: %s\n", inet_ntoa(addr));
    printf("port#: %d\n", ntohs(serverAddr.sin_port));

    /* STEP 2: TCPソケットをオープン */
    if ((send_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;
    }

    /* STEP 3: サーバに接続（bind相当も実行） */
    if (connect(send_sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect");
        return 1;
    }

    /* --sending-- done */
    printf("Waiting for connection...\n");
    while (!isEnd) { /* 終了フラグが0の間は繰り返す */

        /* STEP 5: クライアントからの接続要求を受け付ける */
        printf("waiting connection...\n");
        addrLen = sizeof(clientAddr);
        sock = accept(sock0, (struct sockaddr*)&clientAddr, (socklen_t*)&addrLen);
        if (sock < 0) {
            perror("accept");
            return 1;
        }

        while ((n = read(sock, buf, BUF_LEN)) > 0) {
            write(send_sock, buf, n);
        }

        close(sock);
    }
    close(sock0);
    return 0;
}