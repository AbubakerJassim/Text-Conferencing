CC = gcc
CFLAGS = -Wall -Wextra -g -pthread
OBJ_CLIENT = client.o utils.o
OBJ_SERVER = server.o utils.o

all: client server

client: $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o client $(OBJ_CLIENT)

server: $(OBJ_SERVER)
	$(CC) $(CFLAGS) -o server $(OBJ_SERVER)

client.o: client.c utils.h
	$(CC) $(CFLAGS) -c client.c

server.o: server.c utils.h
	$(CC) $(CFLAGS) -c server.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f *.o client server
