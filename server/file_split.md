### file分割して、pthreadで並列に送るコード
compile
```console
gcc tcp_file_split_server_w_thread.c -o tcp_file_split_server_w_thread.out -lpthread
```
code内のSPLIT_LENで分割をコントロール可能。

```console
./tcp_file_split_server_w_thread.out test.pdf 0 10 20 30 
```

### file分割して、統合するコード
```console
gcc -o tcp_file_split_server.out tcp_file_split_server.c
./tcp_file_split_server.out test.pdf
```

### split
test.pdf -> text1, text2

### merge
text1, text2 -> text3.pdf