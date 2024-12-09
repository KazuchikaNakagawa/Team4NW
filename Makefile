# variables
CLIENT = ./fclient
MIRROR = mirror.out
SERVER = server.out
DATA = data.dat
CFLAGS = -I.

$(MIRROR): lib/tcpconnect.c lib/tcpwait.c _mirror/main.c
	gcc lib/tcpconnect.c lib/tcpwait.c _mirror/main.c -o $(MIRROR) $(CFLAGS)

$(SERVER): 
	gcc server/tcp_file_split_server_w_thread.c -o $(SERVER) -lpthread $(CFLAGS)

$(DATA):
	head -c 100m /dev/urandom > test.dat

node1: $(CLIENT)
# node2 -> node1
	$(CLIENT) 172.20.0.20 10000 out1.dat
# node3 -> node1
	$(CLIENT) 172.21.0.30 10001 out2.dat

node2: $(MIRROR)
# node3 -> node2
	$(MIRROR) 172.24.0.30 10000

node3: $(DATA) $(SERVER)
	$(SERVER) $(DATA)