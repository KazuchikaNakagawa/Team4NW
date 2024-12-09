/*  -*- coding: utf-8-unix; -*-                                     */
/*  FILENAME     :  tcp_echo_serv.c                                 */
/*  DESCRIPTION  :  TCP Echo Server                                 */
/*                                                                  */
/*  VERSION      :                                                  */
/*  DATE         :  Sep 01, 2020                                    */
/*  UPDATE       :                                                  */
/*                                                                  */

#include "icslab2_net.h"

int
main(int argc, char** argv)
{
    int     sock0;                  /* 待ち受け用ソケットディスクリプタ */
    int     sock;                   /* ソケットディスクリプタ */
    struct sockaddr_in  serverAddr; /* サーバ＝自分用アドレス構造体 */
    struct sockaddr_in  clientAddr; /* クライアント＝相手用アドレス構造体 */

    int     addrLen;                /* clientAddrのサイズ */
    char    buf[BUF_LEN];           /* 受信バッファ */
    int     n;                      /* 受信バイト数 */
    int     isEnd = 0;              /* 終了フラグ，0でなければ終了 */

    int     yes = 1;                /* setsockopt()用 */
    struct in_addr addr;            /* アドレス表示用 */

    int     fd = 1;
    char    *filename = NULL;

    if(argc > 1) {
        filename = argv[1];
    }


    /* STEP 1: TCPソケットをオープンする */
    if((sock0 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return  1;
    }

    /* sock0のコネクションがTIME_WAIT状態でもbind()できるように設定 */
    setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR,
               (const char *)&yes, sizeof(yes));

    /* STEP 2: クライアントからの要求を受け付けるIPアドレスとポートを設定する */
    memset(&serverAddr, 0, sizeof(serverAddr));     /* ゼロクリア */
    serverAddr.sin_family = AF_INET;                /* Internetプロトコル */
    serverAddr.sin_port = htons(TCP_SERVER_PORT);   /* 待ち受けるポート */
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* どのIPアドレス宛でも */

    /* STEP 3: ソケットとアドレスをbindする */
    if(bind(sock0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        return  1;
    }

    /* STEP 4: コネクションの最大同時受け入れ数を指定する */
    if(0 != listen(sock0, 5)) {
        perror("listen");
        return  1;
    }

    while(!isEnd) {     /* 終了フラグが0の間は繰り返す */

        /* STEP 5: クライアントからの接続要求を受け付ける */
        printf("waiting connection...\n");
        addrLen = sizeof(clientAddr);
        sock = accept(sock0, (struct sockaddr *)&clientAddr, (socklen_t *)&addrLen);
        if(sock < 0) {
            perror("accept");
            return  1;
        }

        /* 受信パケットの送信元IPアドレスとポート番号を表示 */        
        addr.s_addr = clientAddr.sin_addr.s_addr;
        printf("accepted:  ip address: %s\n", inet_ntoa(addr));
        printf("port#: %d\n", ntohs(clientAddr.sin_port));

	    /* STEP 7: 送信するファイルをオープン */
	    fd = open(filename, O_RDONLY);


        /* STEP 8: ファイル読み込むたびに送信 */
        int send_count = 0;
        while((n = read(fd, buf, BUF_LEN)) > 0) { /* 受信するたびに */
	        write(sock, buf, n);
            send_count++;    
        }
	    close(fd);

        /* STEP 8: 通信用のソケットのクローズ */
        close(sock);
        printf("closed\n");
        printf("send_count : %d\n", send_count);
    }

    /* STEP 8: 待ち受け用ソケットのクローズ */
    close(sock0);  

    return  0;
}

/* Local Variables: */
/* compile-command: "gcc tcp_echo_server.c -o tcp_echo_server.out" */
/* End: */
