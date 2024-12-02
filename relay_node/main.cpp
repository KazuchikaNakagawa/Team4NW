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
    int sendsock; /* ソケットディスクリプタ */
    struct sockaddr_in serverAddr; /* サーバ＝自分用アドレス構造体 */
    struct sockaddr_in clientAddr; /* クライアント＝相手用アドレス構造体 */
    int addrLen; /* clientAddrのサイズ */

    char buf[BUF_LEN]; /* 受信バッファ */
    int n; /* 受信バイト数 */
    int isEnd = 0; /* 終了フラグ，0でなければ終了 */

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
    memset(&serverAddr, 0, sizeof(serverAddr)); /* ゼロクリア */
    serverAddr.sin_family = AF_INET; /* Internetプロトコル */
    serverAddr.sin_port = htons(TCP_SERVER_PORT); /* 待ち受けるポート */
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* どのIPアドレス宛でも */

    /* STEP 3: ソケットとアドレスをbindする */
    if (bind(sock0, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        return 1;
    }

    /* STEP 4: コネクションの最大同時受け入れ数を指定する */
    if (listen(sock0, 5) != 0) {
        perror("listen");
        return 1;
    }

    while (!isEnd) { /* 終了フラグが0の間は繰り返す */

        /* STEP 5: クライアントからの接続要求を受け付ける */
        printf("waiting connection from client...\n");
        addrLen = sizeof(clientAddr);
        sendsock = accept(sock0, (struct sockaddr*)&clientAddr, (socklen_t*)&addrLen);
        if (sendsock < 0) {
            perror("accept");
            return 1;
        }

        /* STEP 6: クライアントからのファイル要求の受信 */
        if ((n = read(sendsock, buf, BUF_LEN)) < 0) {
            close(sendsock);
            break;
        }

        /* 今回は表示するだけで中身は無視 */
        buf[n] = '\0';
        printf("recv req: %s\n", buf);

        printf("waiting for server to connect...\n");
        {
            char* server_ipaddr_str = "127.0.0.1"; /* サーバIPアドレス（文字列） */
            unsigned int port = TCP_SERVER_PORT; /* ポート番号 */
            char* filename = NULL;

            int receivesock; /* ソケットディスクリプタ */
            struct sockaddr_in serverAddr; /* サーバ＝相手用のアドレス構造体 */
            char buf[BUF_LEN]; /* 受信バッファ */
            int n; /* 読み込み／受信バイト数 */

            struct in_addr addr; /* アドレス表示用 */
            if (argc < 3) {
                fprintf(stderr, "Usage: %s <server_ipaddr> <port>\n", argv[0]);
                exit(1);
            }
            if (argc > 1) /* 宛先を指定のIPアドレスにする。 portはデフォルト */
                server_ipaddr_str = argv[1];
            if (argc > 2) /* 宛先を指定のIPアドレス、portにする */
                port = (unsigned int)atoi(argv[2]);

            /* STEP 1: 宛先サーバのIPアドレスとポートを指定 */
            memset(&serverAddr, 0, sizeof(serverAddr)); /* 0クリア */
            serverAddr.sin_family = AF_INET; /* Internetプロトコル */
            serverAddr.sin_port = htons(port); /* サーバの待受ポート */
            /* IPアドレス（文字列）から変換 */
            inet_pton(AF_INET, server_ipaddr_str, &serverAddr.sin_addr.s_addr);

            /* 確認用：IPアドレスを文字列に変換して表示 */
            addr.s_addr = serverAddr.sin_addr.s_addr;
            printf("ip address: %s\n", inet_ntoa(addr));
            printf("port#: %d\n", ntohs(serverAddr.sin_port));

            /* STEP 2: TCPソケットをオープン */
            if ((receivesock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("socket");
                return 1;
            }

            /* STEP 3: サーバに接続（bind相当も実行） */
            if (connect(receivesock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                perror("connect");
                return 1;
            }

            /* STEP 4: ダミーのファイル要求メッセージの作成と送信  \r\nをデリミタにする */
            sprintf(buf, "GET\r\n");
            write(receivesock, buf, strlen(buf)); /* 要求の送信 */

            /* STEP 5: 受信するたびにクライアントに中継 */
            while ((n = read(receivesock, buf, BUF_LEN)) > 0) {
                write(sendsock, buf, n); /* 中継 */
            }

            /* STEP 6: 出力ファイルのクローズ */
            close(sendsock);

            /* STEP 7: ソケットのクローズ */
            close(receivesock);
        }
    }
}