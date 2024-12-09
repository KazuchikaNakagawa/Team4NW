# variables
CLIENT = ./client.out
MIRROR = ./mirror.out
SERVER = ./server.out
DATA = ./data.dat
CFLAGS = -I.
RATES = 0 14285714 28571429 100000000

$(MIRROR): lib/tcpconnect.c lib/tcpwait.c _mirror/main.c
	gcc lib/tcpconnect.c lib/tcpwait.c _mirror/main.c -o $(MIRROR) $(CFLAGS)

$(SERVER): 
	gcc server/tcp_file_split_server_w_thread.c -o $(SERVER) -lpthread $(CFLAGS)

$(DATA):
	head -c 100m /dev/urandom > $(DATA)

$(CLIENT): 
	gcc client/tcp_file_client.c -o $(CLIENT) $(CFLAGS)

node1: $(CLIENT)
# node3 -> node1
	$(CLIENT) 172.21.0.30 10000 out2.dat &
	sleep 0.01
# node4 -> node1
	$(CLIENT) 172.22.0.40 10000 out3.dat &
	sleep 0.01
# node2 -> node1
	$(CLIENT) 172.20.0.20 10000 out1.dat &

node2: $(MIRROR)
# node3 -> node2
	$(MIRROR) 172.24.0.30 10000

node3: $(DATA) $(SERVER)
	$(SERVER) $(DATA) $(RATES)

node4: $(MIRROR)
# node3 -> node4
	$(MIRROR) 172.27.0.30 10000

clean:
	rm -f $(MIRROR) $(SERVER) $(DATA) $(CLIENT) out1.dat out2.dat out3.dat outall.dat