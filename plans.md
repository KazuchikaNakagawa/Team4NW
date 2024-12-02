# 転送プロトコル
## メンバー
有村くん
小山くん
伊藤くん
中川くん

## 工夫できる点
- node2,4,5にも通信を担当してもらう
- 最短ルートになるべく多く流す
- Flowを計算する
- 新たなプロトコルを作る

## iperf3
CWNDの自動調節等有利に進みそうなのでTCPを
利用する。
### node1
受け取り側なので省略
### node2
- to node1 49.2Mbits/sec
- to node4 1.48Mbits/sec
- to node5 9.58Mbits/sec

### node3
- to node1 9.73Mbits/sec
- to node2 49.2Mbits/sec
- to node4 49.2Mbits/sec
- to node5 1.24Mbits/sec

### node4
- to node1 10.1Mbits/sec
- to node2 1.25Mbits/sec
- to node5 9.54Mbits/sec

### node5
- to node1 47.8Mbits/sec
- to node2 9.54Mbits/sec
- to node4 47.8Mbits/sec

## 計画
### 1. 分割して送る
### 2. 3から2、2から1に送る
### 3. 他のルートを使う
### 4. 経路計算ツールを作る