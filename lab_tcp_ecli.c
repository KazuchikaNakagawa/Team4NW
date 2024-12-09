/*  -*- coding: utf-8-unix; -*-                                     */
/*  FILENAME     :  tcp_echo_client.c                               */
/*  DESCRIPTION  :  TCP Echo Client                                 */
/*  USAGE        :  tcp_echo_client.out [dst_ip_addr] [port]        */
/*  DATE         :  Sep 01, 2020                                    */
/*                                                                  */

#include "icslab2_net.h"

int
main(int argc, char** argv)
{
    char *server_ipaddr_str = "127.0.0.1";      /* サーバIPアドレス（文字列） */
    unsigned int port = TCP_SERVER_PORT;        /* ポート番号 */
    char *filename = NULL;
    int fd = 1;                             /* 標準出力 */
    char *dummy_file = "HELLO.txt";               /* ダミーのリクエストメッセージ */
    
    int     sock;                   /* ソケットディスクリプタ */
    struct sockaddr_in serverAddr;  /* サーバ＝相手用のアドレス構造体 */
    char    buf[BUF_LEN];           /* 受信バッファ */
    int     n;                      /* 読み込み／受信バイト数 */
    
    struct in_addr addr;            /* アドレス表示用 */

    unsigned int sec;
    int nsec;
    double time;
    struct timespec start_time, end_time;

    /* コマンドライン引数の処理 */
    if(argc == 2 && strncmp(argv[1], "-h", 2) == 0) {
        printf("Usage: %s [dst_ip_addr] [port]\n", argv[0]);
        return 0;
    }
    if(argc > 1)    /* 宛先を指定のIPアドレスにする。 portはデフォルト */
        server_ipaddr_str = argv[1];
    if(argc > 2)    /* 宛先を指定のIPアドレス、portにする */
        port = (unsigned int)atoi(argv[2]);
    if(argc > 3) {
        filename = argv[3];
        fd = open(filename, O_CREAT|O_WRONLY, 0644);
	    if(fd < 0) {
           perror("open");
           return  1;
    	}
    }

    /* STEP 1: 宛先サーバのIPアドレスとポートを指定する */
    memset(&serverAddr, 0, sizeof(serverAddr));     /* 0クリア */
    serverAddr.sin_family = AF_INET;                /* Internetプロトコル */
    serverAddr.sin_port = htons(port);              /* サーバの待受ポート */
    /* IPアドレス（文字列）から変換 */
    inet_pton(AF_INET, server_ipaddr_str, &serverAddr.sin_addr.s_addr);

    /* 確認用：IPアドレスを文字列に変換して表示 */
    addr.s_addr = serverAddr.sin_addr.s_addr;
    printf("ip address: %s\n", inet_ntoa(addr));
    printf("port#: %d\n", ntohs(serverAddr.sin_port));

    /* STEP 2x: TCPソケットをオープンする */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket");
        return  1;
    }

    /* ここで、ローカルでsocketをbind()してもよいが省略する */

    /* 処理前の時刻計測 */
    clock_gettime(CLOCK_REALTIME, &start_time);

    /* STEP 3x: サーバに接続する（bind相当も実行） */
    if(connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect");
        return  1;
    }

    /* STEP 4: ダミーのファイル要求メッセージの作成と送信  \r\nをデリミタにする */
    //sprintf(buf, "GET %s\r\n", dummy_file); 
    //write(sock, buf, strlen(buf));          /* 要求の送信 */

    /* STEP 5: 受信するたびにファイルに出力 */
    int receive_count = 0;
    int total_bytes = 0;
    while((n = read(sock, buf, BUF_LEN)) > 0) {    
        write(fd, buf, n);                    /* 読み込んだ内容を送信 */
        //usleep(10000);
        receive_count++;
        total_bytes += n;
    }

    /* 処理後の時刻計測 */
    clock_gettime(CLOCK_REALTIME, &end_time);

    close(fd);

    /* STEP 6: ソケットのクローズ */
    close(sock);
    printf("closed\n");

    /* 経過時間の計算 */
    sec = end_time.tv_sec - start_time.tv_sec;
    nsec = end_time.tv_nsec - start_time.tv_nsec;
    time = (double)sec + (double)nsec * 1e-9;

    printf("time: %f [s]\n", time);
    printf("total_bytes: %d\n",total_bytes);
    printf("goodput : %3f [Mbps]\n", (total_bytes * 8) / (time * 1000000) );
    printf("BUF_LEN : %d\n", BUF_LEN);
    printf("receive_count : %d\n", receive_count);

    return 0;
}


/* Local Variables: */
/* compile-command: "gcc tcp_echo_client.c -o tcp_echo_client.out " */
/* End: */
