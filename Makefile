# variables
CLIENT = ./fclient
MIRROR = ./bin/mirror
SERVER = ./fserver
DATA = ./data.dat

mirror: lib/tcpconnect.c lib/tcpwait.c _mirror/main.c
	gcc lib/tcpconnect.c lib/tcpwait.c _mirror/main.c -o $(MIRROR) -I.

node1:
# node2 -> node1
	$(CLIENT) 172.20.0.20 10000 out1.dat 

node2:
# node3 -> node2
	$(MIRROR) 172.24.0.30 10000

node3:
	$(SERVER) $(DATA)