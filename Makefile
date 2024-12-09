mirror: lib/tcpconnect.c lib/tcpwait.c _mirror/main.c
	gcc lib/tcpconnect.c lib/tcpwait.c _mirror/main.c -o bin/mirror -I.

