#include "icslab2_net.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// #define SPLIT_LEN (50ULL * 1024ULL * 1024ULL) // 50MB
#define SPLIT_LEN 100

struct thread_arg {
    int sock;
    char filename[256];
    int part; // 1回目か2回目かを表す
};

void* client_handler(void* arg) {
    struct thread_arg *targ = (struct thread_arg *)arg;
    int sock = targ->sock;
    char filename[256];
    strcpy(filename, targ->filename);
    int part = targ->part;

    char buf[BUF_LEN];
    ssize_t n;

    // クライアントから要求を読む(例:何か要求データ受信)
    if((n = read(sock, buf, BUF_LEN)) < 0) {
        perror("read");
        close(sock);
        free(targ);
        return NULL;
    }
    buf[n] = '\0';
    printf("recv req from client (part %d): %s\n", part, buf);

    // ファイルを開いて送信
    int fd = open(filename, O_RDONLY);
    if(fd < 0) {
        perror("open");
        close(sock);
        free(targ);
        return NULL;
    }

    // 送信オフセットと送信サイズの決定
    off_t offset = 0;
    off_t to_send = SPLIT_LEN; // 50MB
    if (part == 2) {
        // 2回目の接続は50MB後ろから開始
        offset = SPLIT_LEN;
    }

    // ファイル位置を設定
    if(lseek(fd, offset, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        close(sock);
        free(targ);
        return NULL;
    }

    // 送信ループ
    off_t sent_total = 0;
    while (sent_total < to_send) {
        off_t remaining = to_send - sent_total;
        ssize_t to_read = (remaining < BUF_LEN) ? remaining : BUF_LEN;

        ssize_t read_bytes = read(fd, buf, to_read);
        if (read_bytes < 0) {
            perror("read file");
            break;
        } else if (read_bytes == 0) {
            // ファイル終端（理論上100MBある想定だがファイルがそれ未満ならここで終わる）
            break;
        }

        // ソケットへ書き込み
        ssize_t written = write(sock, buf, read_bytes);
        if (written < 0) {
            perror("write");
            break;
        }

        sent_total += written;
    }

    close(fd);
    close(sock);
    free(targ);
    printf("Client handler (part %d) finished. Sent %ld bytes.\n", part, (long)sent_total);
    return NULL;
}


int main(int argc, char** argv)
{
    int sock0;
    struct sockaddr_in serverAddr;
    int yes = 1;

    if(argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
        return 0;
    }

    // 接続回数を数えるカウンタ
    // 本例では2回接続があったら2回に分けて送る
    // スレッド安全を保つためにaccept前後で扱うならメインスレッドのみで使うのでmutex不要
    int connection_count = 0;

    if((sock0 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;
    }

    setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

    memset(&serverAddr, 0, sizeof(serverAddr));     
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        return 1;
    }

    if(listen(sock0, 5) != 0) {
        perror("listen");
        return 1;
    }

    printf("waiting connection...\n");

    while(1) {
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int sock = accept(sock0, (struct sockaddr *)&clientAddr, &addrLen);
        if(sock < 0) {
            perror("accept");
            break;
        }

        // 接続回数をインクリメント
        connection_count++;

        if (connection_count > 2) {
            // 3回目以降は送るデータなしとする場合
            // あるいはエラー処理。ここでは閉じておく
            close(sock);
            printf("No more data to send. Closing extra connection.\n");
            continue;
        }

        // スレッド引数確保
        struct thread_arg *targ = (struct thread_arg *)malloc(sizeof(struct thread_arg));
        targ->sock = sock;
        strcpy(targ->filename, argv[1]);
        targ->part = connection_count; // 1回目はpart=1, 2回目はpart=2

        // スレッド生成
        pthread_t tid;
        if(pthread_create(&tid, NULL, client_handler, (void*)targ) != 0) {
            perror("pthread_create");
            close(sock);
            free(targ);
            continue;
        }

        // スレッド分離
        pthread_detach(tid);

        // 2回送信したら終了する場合はここでbreakしても良い
        // 今回は特に記述しないが、必要なら:
        // if (connection_count == 2) {
        //     printf("Both halves sent, exiting server.\n");
        //     break;
        // }
    }

    close(sock0);
    return 0;
}
