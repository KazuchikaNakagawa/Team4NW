#include "utils/icslab2_net.h"

int main(int argc, char** argv)
{
    char* server_ipaddr_str = "127.0.0.1"; /* サーバIPアドレス（文字列） */
    unsigned int port = TCP_SERVER_PORT; /* ポート番号 */
    char* filename = NULL;
    int fd = 1; /* 標準出力 */
    char* dummy_file = "HELLO.txt"; /* ダミーのリクエストメッセージ */

    int sock; /* ソケットディスクリプタ */
    struct sockaddr_in serverAddr; /* サーバ＝相手用のアドレス構造体 */
    char buf[BUF_LEN]; /* 受信バッファ */
    int n; /* 読み込み／受信バイト数 */

    struct in_addr addr; /* アドレス表示用 */

    /* コマンドライン引数の処理 */
    if (argc == 2 && strncmp(argv[1], "-h", 2) == 0) {
        printf("Usage: %s [dst_ip_addr] [port] [o_filename]\n", argv[0]);
        return 0;
    }
    if (argc > 1) /* 宛先を指定のIPアドレスにする。 portはデフォルト */
        server_ipaddr_str = argv[1];
    if (argc > 2) /* 宛先を指定のIPアドレス、portにする */
        port = (unsigned int)atoi(argv[2]);
    if (argc > 3) { /* 出力ファイルの指定 */
        printf("set outputfile: %s", argv[3]);
        filename = argv[3];
        /* STEP 0: 出力ファイルのオープン */
        fd = open(filename, O_CREAT | O_WRONLY, 0644);
        if (fd < 0) {
            perror("open");
            return 1;
        }
    }

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
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;
    }

    /* STEP 3: サーバに接続（bind相当も実行） */
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect");
        return 1;
    }

    /* STEP 4: ダミーのファイル要求メッセージの作成と送信  \r\nをデリミタにする */
    sprintf(buf, "GET %s\r\n", dummy_file);
    write(sock, buf, strlen(buf)); /* 要求の送信 */

    /* STEP 5: 受信するたびにファイルに出力 */
    while ((n = read(sock, buf, BUF_LEN)) > 0) {
        write(fd, buf, n); /* ファイルに出力 */
    }

    /* STEP 6: 出力ファイルのクローズ */
    close(fd);

    /* STEP 7: ソケットのクローズ */
    close(sock);

    return 0;
}
/*--------------------------- <end> --------------------------------*/
