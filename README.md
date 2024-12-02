# 使い方
## 中継させたい場合
送信側
```
tcp_file_client <中継機のアドレス> <ポート> <出力ファイル名>
```

受信側
```
tcp_file_server <送信したいファイル>
```

中継機
```
cd Team4NW
make mirror
./bin/mirror <ip address>
```

IPアドレスは`dig node2`などで調べることができます。